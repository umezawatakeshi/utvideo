/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "SymPack_x86x64.h"

#if !defined(GENERATE_AVX512_ICL)
#error
#endif

template<int F>
static inline FORCEINLINE __m512i VECTORCALL GetVRemBits(__m512i x)
{
	__mmask8 kisnotzero = _mm512_cmpneq_epi64_mask(x, _mm512_set1_epi8(0));
	__m512i tmp = _mm512_and_si512(x, _mm512_set1_epi8((char)0x80)); // xが0以上なら0、0未満なら0x80
	__m512i negative = _mm512_adds_epu8(tmp, tmp); // xが0以上なら0、0未満なら0xff
	__m512i z = _mm512_xor_si512(x, negative);

	z = _mm512_or_si512(z, _mm512_slli_epi64(z, 32));
	z = _mm512_or_si512(z, _mm512_slli_epi64(z, 16));
	z = _mm512_or_si512(_mm512_or_si512(z, _mm512_set1_epi64(1ULL << 56)), _mm512_slli_epi64(z, 8));
	__m512i lz = _mm512_lzcnt_epi64(z);

	__m512i rembits = _mm512_mask_sub_epi64(_mm512_set1_epi64(8), kisnotzero, lz, _mm512_set1_epi64(1));

	return rembits;
};

template<int F, bool Delta>
static inline FORCEINLINE void VECTORCALL PackElement(uint8_t*& q, uint8_t*& r, __m512i w, __m512i t)
{
	static constexpr int MODEBITS = Delta ? 4 : 3;
	static constexpr uint32_t MODEPEXT32 = Delta ? 0x0f0f0f0f : 0x07070707;
	static constexpr uint64_t MODEPEXT64 = Delta ? 0x0f0f0f0f0f0f0f0fULL : 0x0707070707070707ULL;

	auto rembitsw = GetVRemBits<F>(w);
	__m512i rembits;
	__mmask8 ktemporal;
	if (!Delta)
	{
		rembits = rembitsw;
	}
	else
	{
		auto rembitst = GetVRemBits<F>(t);
		ktemporal = _mm512_cmpge_epi64_mask(rembitst, rembitsw);

		w = _mm512_mask_mov_epi64(w, ktemporal, t);
		rembits = _mm512_mask_mov_epi64(rembitsw, ktemporal, rembitst);
	}

	uint32_t modes;
	__m128i vmask;
	if (Delta)
	{
		__m512i vmodes = _mm512_subs_epu8(_mm512_set1_epi64(7), rembits);
		vmodes = _mm512_mask_or_epi64(vmodes, ktemporal, vmodes, _mm512_set1_epi64(8));
		__m128i vmodes64 = _mm512_castsi512_si128(_mm512_permutexvar_epi8(_mm512_set8_epi8(56, 48, 40, 32, 24, 16, 8, 0), vmodes));
		modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes64), MODEPEXT64);

		vmask = _mm_shuffle_epi8(_mm_set8_epi8((char)0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0), vmodes64);
	}
	else
	{
		__m128i rembits64 = _mm512_castsi512_si128(_mm512_permutexvar_epi8(_mm512_set8_epi8(56, 48, 40, 32, 24, 16, 8, 0), rembits));
		__m128i vmodes = _mm_subs_epu8(_mm_set1_epi8(7), rembits64);
		modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes), MODEPEXT64);

		vmask = _mm_shuffle_epi8(_mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, (char)0xff), rembits64);
	}

	uint64_t mask = _mm_cvtsi128_si64(vmask);
	__mmask64 kmask = mask;
	// ↑VPSHUFBITQMB を使うと1命令少なくなるが、その場合でも後で POPCNT に渡すために KMOV が現れるので意味がない

	w = _mm512_add_epi8(w, _mm512_srlv_epi64(_mm512_set1_epi8((char)0x80), rembits));
	w = _mm512_or_si512(_mm512_and_si512(w, _mm512_set1_epi16(0x00ff)), _mm512_srlv_epi64(_mm512_andnot_si512(_mm512_set1_epi16(0x00ff), w), rembits));
	rembits = _mm512_slli_epi64(rembits, 1);
	w = _mm512_or_si512(_mm512_and_si512(w, _mm512_set1_epi32(0x0000ffff)), _mm512_srlv_epi64(_mm512_andnot_si512(_mm512_set1_epi32(0x0000ffff), w), rembits));
	rembits = _mm512_slli_epi64(rembits, 1);
	w = _mm512_or_si512(_mm512_and_si512(w, _mm512_set1_epi64(0x00000000ffffffffULL)), _mm512_srlv_epi64(_mm512_andnot_si512(_mm512_set1_epi64(0x00000000ffffffffULL), w), rembits));

	_mm512_mask_compressstoreu_epi8(q, kmask, w);
	q += _mm_popcnt_u64(mask);
	*(uint32_t*)r = modes;
	r += MODEBITS;
}

template<int F>
static inline FORCEINLINE void PackForIntra(uint8_t*& q, uint8_t*& r, __m512i w)
{
	PackElement<F, false>(q, r, w, _mm512_setzero_si512());
}

template<int F>
static inline FORCEINLINE void PackForDelta(uint8_t*& q, uint8_t*& r, __m512i w, __m512i t)
{
	PackElement<F, true>(q, r, w, t);
}

template<>
void tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride)
{
	constexpr int F = CODEFEATURE_AVX512_ICL;

	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 3);

	{
		__m512i prev = _mm512_set1_epi8((char)0x80);

		for (auto p = pSrcBegin; p != pSrcBegin + cbStride; p += 64)
		{
			__m512i value = _mm512_loadu_si512((const __m512i *)p);
			__m512i residual = tuned_PredictLeft8Element<F>(prev, value);
			prev = value;

			PackForIntra<CODEFEATURE_AVX512_ICL>(q, r, residual);
		}
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		__m512i prev = _mm512_setzero_si512();

		for (auto p = pp; p != pp + cbStride; p += 64)
		{
			__m512i value = _mm512_sub_epi8(_mm512_loadu_si512((const __m512i *)p), _mm512_loadu_si512((const __m512i *)(p - cbStride)));
			__m512i residual = tuned_PredictLeft8Element<F>(prev, value);
			prev = value;

			PackForIntra<CODEFEATURE_AVX512_ICL>(q, r, residual);
		}
	}

	*cbPacked = q - pPacked;
}

struct UNPACK_FOR_DELTA_RESULT
{
	__m512i s;
	__mmask8 ktemporalq;
};

template<int F, bool Delta>
static inline FORCEINLINE auto UnpackElement(const uint8_t*& q, const uint8_t *& r)
{
	static constexpr int MODEBITS = Delta ? 4 : 3;
	static constexpr uint32_t MODEPEXT32 = Delta ? 0x0f0f0f0f : 0x07070707;
	static constexpr uint64_t MODEPEXT64 = Delta ? 0x0f0f0f0f0f0f0f0fULL : 0x0707070707070707ULL;

	uint32_t modes = *(uint32_t*)r;
	r += MODEBITS;

	__mmask8 ktemporalq = _pext_u32(modes, 0x88888888);
	if (Delta)
		modes &= 0x77777777;
	__m128i vmodes = _mm_cvtsi64_si128(_pdep_u64(modes, MODEPEXT64));
	__m128i vmask = _mm_shuffle_epi8(_mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, (char)0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x00), vmodes);
	__mmask64 kmask = _mm_cvtsi128_si64(vmask);
	__m512i w = _mm512_maskz_expandloadu_epi8(kmask, q);
	q += _mm_popcnt_u64(kmask);

	// VPMULTISHIFTQB を使う場合
	// AVX2 までと同様に VPSLL(V)Q を使った場合より速いかどうかは不明
	/*
		VPMULTISHIFTQBを使う場合
		vpermb
		vpermq
		vpmultishiftqb
		vpshufb
		vpand
		vpshufb
		vpsub

		VPSLLVQ を使う場合
		vpsllq
		vpsllvq
		vpblendw
		vpsrlq
		vpsllvq
		vpblendw
		vpsllvq
		vpblendvb
		vpsllvq
		vpsubq
		vpand
		vpsrlvq
		vpand
		vpsub

		さすがにここまで命令数に差があるとVPMULTISHIFTQBを使う方が速そう
	*/
	__m512i vmodes512 = _mm512_permutexvar_epi8(_mm512_set_epi8(
		7, 7, 7, 7, 7, 7, 7, 7,
		6, 6, 6, 6, 6, 6, 6, 6,
		5, 5, 5, 5, 5, 5, 5, 5,
		4, 4, 4, 4, 4, 4, 4, 4,
		3, 3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2, 2, 2, 2,
		1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0
	), _mm512_castsi128_si512(vmodes)); // VPERMQ (_mm512_permutevar_epi64) はインデックスの上位にゴミがあっても動作は変わらないので、下のpshufbにも直接渡せるように同じバイトを複製したものを使ってよい
	__m512i vshifts = _mm512_permutexvar_epi64(vmodes512, _mm512_set_epi8(
		56, 48, 40, 32, 24, 16,  8,  0,
		49, 42, 35, 28, 21, 14,  7,  0,
		42, 36, 30, 24, 18, 12,  6,  0,
		35, 30, 25, 20, 15, 10,  5,  0,
		28, 24, 20, 16, 12,  8,  4,  0,
		21, 18, 15, 12,  9,  6,  3,  0,
		14, 12, 10,  8,  6,  4,  2,  0,
		 7,  6,  5,  4,  3,  2,  1,  0
	));
	w = _mm512_multishift_epi64_epi8(vshifts, w);
	w = _mm512_and_si512(w, _mm512_shuffle_epi8(_mm512_set16_epi8(0, 0, 0, 0, 0, 0, 0, 0, (char)0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x00), vmodes512));
	w = _mm512_sub_epi8(w, _mm512_shuffle_epi8(_mm512_set16_epi8(0, 0, 0, 0, 0, 0, 0, 0, (char)0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00), vmodes512));

	if constexpr (!Delta)
		return w;
	else
		return UNPACK_FOR_DELTA_RESULT{ w, ktemporalq };
}

template<int F>
static inline FORCEINLINE __m512i UnpackForIntra(const uint8_t*& q, const uint8_t*& r)
{
	return UnpackElement<F, false>(q, r);
}

template<int F>
static inline FORCEINLINE UNPACK_FOR_DELTA_RESULT UnpackForDelta(const uint8_t*& q, const uint8_t*& r)
{
	return UnpackElement<F, true>(q, r);
}

template<>
void tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX512_ICL>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbStride)
{
	constexpr int F = CODEFEATURE_AVX512_ICL;

	auto q = pPacked;
	auto r = pControl;

	{
		__m512i prev = _mm512_set1_epi8((char)0x80);

		for (auto p = pDstBegin; p != pDstBegin + cbStride; p += 64)
		{
			__m512i s0 = UnpackForIntra<F>(q, r);
			auto result = tuned_RestoreLeft8Element<F>(prev, s0);
			_mm512_storeu_si512((__m256i *)p, result.v0);
			prev = result.v1;
		}
	}

	for (auto pp = pDstBegin + cbStride; pp != pDstEnd; pp += cbStride)
	{
		__m512i prev = _mm512_set1_epi8((char)0);

		for (auto p = pp; p != pp + cbStride; p += 64)
		{
			__m512i s0 = UnpackForIntra<F>(q, r);
			auto result = tuned_RestoreLeft8Element<F>(prev, s0);
			_mm512_storeu_si512((__m512i*)p, _mm512_add_epi8(result.v0, _mm512_loadu_si512((const __m512i*)(p - cbStride))));
			prev = result.v1;
		}
	}
}



template<>
void tuned_Pack8SymWithDiff8<CODEFEATURE_AVX512_ICL>(uint8_t* pPacked, size_t* cbPacked, uint8_t* pControl, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, const uint8_t* pPrevBegin, size_t cbStride)
{
	static constexpr int F = CODEFEATURE_AVX512_ICL;

	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 4);

	{
		__m512i prev = _mm512_set1_epi8((char)0x80);

		for (auto p = pSrcBegin, t = pPrevBegin; p != pSrcBegin + cbStride; p += 64, t += 64)
		{
			__m512i value0 = _mm512_loadu_si512((const __m512i*)p);
			__m512i error0 = tuned_PredictLeft8Element<F>(prev, value0);
			__m512i t0 = _mm512_sub_epi8(value0, _mm512_loadu_si512((const __m512i*)t));
			prev = value0;

			PackForDelta<CODEFEATURE_AVX512_ICL>(q, r, error0, t0);
		}
	}

	for (auto pp = pSrcBegin + cbStride, tt = pPrevBegin + cbStride; pp != pSrcEnd; pp += cbStride, tt += cbStride)
	{
		__m512i prev = _mm512_setzero_si512();

		for (auto p = pp, t = tt; p != pp + cbStride; p += 64, t += 64)
		{
			__m512i value0 = _mm512_loadu_si512((const __m512i*)p);
			__m512i top0 = _mm512_loadu_si512((const __m512i*)(p - cbStride));
			__m512i tmp0 = _mm512_sub_epi8(value0, top0);
			__m512i error0 = tuned_PredictLeft8Element<F>(prev, tmp0);
			__m512i t0 = _mm512_sub_epi8(value0, _mm512_loadu_si512((const __m512i*)t));
			prev = tmp0;

			PackForDelta<CODEFEATURE_AVX512_ICL>(q, r, error0, t0);
		}
	}

	*cbPacked = q - pPacked;
}

template<>
void tuned_Unpack8SymWithDiff8<CODEFEATURE_AVX512_ICL>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pPacked, const uint8_t* pControl, const uint8_t* pPrevBegin, size_t cbStride)
{
	static const uint32_t addmask[256] = {
		0x0f3355, 0x0f3355, 0x0f3357, 0x0f3357, 0x0f3f55, 0x0f3f55, 0x0f3f57, 0x0f3f57,
		0x0f3b5d, 0x0f3b5d, 0x0f3b5f, 0x0f3b5f, 0x0f3f5d, 0x0f3f5d, 0x0f3f5f, 0x0f3f5f,
		0xff3355, 0xff3355, 0xff3357, 0xff3357, 0xff3f55, 0xff3f55, 0xff3f57, 0xff3f57,
		0xff3b5d, 0xff3b5d, 0xff3b5f, 0xff3b5f, 0xff3f5d, 0xff3f5d, 0xff3f5f, 0xff3f5f,
		0xef3375, 0xef3375, 0xef3377, 0xef3377, 0xef3f75, 0xef3f75, 0xef3f77, 0xef3f77,
		0xef3b7d, 0xef3b7d, 0xef3b7f, 0xef3b7f, 0xef3f7d, 0xef3f7d, 0xef3f7f, 0xef3f7f,
		0xff3375, 0xff3375, 0xff3377, 0xff3377, 0xff3f75, 0xff3f75, 0xff3f77, 0xff3f77,
		0xff3b7d, 0xff3b7d, 0xff3b7f, 0xff3b7f, 0xff3f7d, 0xff3f7d, 0xff3f7f, 0xff3f7f,
		0xcff355, 0xcff355, 0xcff357, 0xcff357, 0xcfff55, 0xcfff55, 0xcfff57, 0xcfff57,
		0xcffb5d, 0xcffb5d, 0xcffb5f, 0xcffb5f, 0xcfff5d, 0xcfff5d, 0xcfff5f, 0xcfff5f,
		0xfff355, 0xfff355, 0xfff357, 0xfff357, 0xffff55, 0xffff55, 0xffff57, 0xffff57,
		0xfffb5d, 0xfffb5d, 0xfffb5f, 0xfffb5f, 0xffff5d, 0xffff5d, 0xffff5f, 0xffff5f,
		0xeff375, 0xeff375, 0xeff377, 0xeff377, 0xefff75, 0xefff75, 0xefff77, 0xefff77,
		0xeffb7d, 0xeffb7d, 0xeffb7f, 0xeffb7f, 0xefff7d, 0xefff7d, 0xefff7f, 0xefff7f,
		0xfff375, 0xfff375, 0xfff377, 0xfff377, 0xffff75, 0xffff75, 0xffff77, 0xffff77,
		0xfffb7d, 0xfffb7d, 0xfffb7f, 0xfffb7f, 0xffff7d, 0xffff7d, 0xffff7f, 0xffff7f,
		0x8fb3d5, 0x8fb3d5, 0x8fb3d7, 0x8fb3d7, 0x8fbfd5, 0x8fbfd5, 0x8fbfd7, 0x8fbfd7,
		0x8fbbdd, 0x8fbbdd, 0x8fbbdf, 0x8fbbdf, 0x8fbfdd, 0x8fbfdd, 0x8fbfdf, 0x8fbfdf,
		0xffb3d5, 0xffb3d5, 0xffb3d7, 0xffb3d7, 0xffbfd5, 0xffbfd5, 0xffbfd7, 0xffbfd7,
		0xffbbdd, 0xffbbdd, 0xffbbdf, 0xffbbdf, 0xffbfdd, 0xffbfdd, 0xffbfdf, 0xffbfdf,
		0xefb3f5, 0xefb3f5, 0xefb3f7, 0xefb3f7, 0xefbff5, 0xefbff5, 0xefbff7, 0xefbff7,
		0xefbbfd, 0xefbbfd, 0xefbbff, 0xefbbff, 0xefbffd, 0xefbffd, 0xefbfff, 0xefbfff,
		0xffb3f5, 0xffb3f5, 0xffb3f7, 0xffb3f7, 0xffbff5, 0xffbff5, 0xffbff7, 0xffbff7,
		0xffbbfd, 0xffbbfd, 0xffbbff, 0xffbbff, 0xffbffd, 0xffbffd, 0xffbfff, 0xffbfff,
		0xcff3d5, 0xcff3d5, 0xcff3d7, 0xcff3d7, 0xcfffd5, 0xcfffd5, 0xcfffd7, 0xcfffd7,
		0xcffbdd, 0xcffbdd, 0xcffbdf, 0xcffbdf, 0xcfffdd, 0xcfffdd, 0xcfffdf, 0xcfffdf,
		0xfff3d5, 0xfff3d5, 0xfff3d7, 0xfff3d7, 0xffffd5, 0xffffd5, 0xffffd7, 0xffffd7,
		0xfffbdd, 0xfffbdd, 0xfffbdf, 0xfffbdf, 0xffffdd, 0xffffdd, 0xffffdf, 0xffffdf,
		0xeff3f5, 0xeff3f5, 0xeff3f7, 0xeff3f7, 0xeffff5, 0xeffff5, 0xeffff7, 0xeffff7,
		0xeffbfd, 0xeffbfd, 0xeffbff, 0xeffbff, 0xeffffd, 0xeffffd, 0xefffff, 0xefffff,
		0xfff3f5, 0xfff3f5, 0xfff3f7, 0xfff3f7, 0xfffff5, 0xfffff5, 0xfffff7, 0xfffff7,
		0xfffbfd, 0xfffbfd, 0xfffbff, 0xfffbff, 0xfffffd, 0xfffffd, 0xffffff, 0xffffff,
	};
	/*
		for (int m = 0; m < 0x100; ++m)
		{
			uint32_t am = 0;
			for (int r = 0; r < 3; ++r)
			{
				uint8_t tmp = m;
				for (int i = 0; i < 8; i += (2 << r))
				{
					int j = i;
					for (; j < i + (1 << r); ++j)
						tmp |= 1 << j;
					for (; j < (i + (2 << r)); ++j)
						if (tmp & (1 << j))
							break;
					for (; j < (i + (2 << r)); ++j)
						tmp |= 1 << j;
				}
				am |= tmp << (8 * r);
			}
			addmask[m] = am;
		}
	 */

	auto q = pPacked;
	auto r = pControl;

	{
		__m512i prev = _mm512_set_epi8(
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char)0x80, (char)0x80);

		auto t = pPrevBegin;
		for (auto p = pDstBegin; p != pDstBegin + cbStride; p += 64, t += 64)
		{
			auto [s0, ktemporalq] = UnpackForDelta<CODEFEATURE_AVX512_ICL>(q, r);

			__m512i t0 = _mm512_add_epi8(s0, _mm512_loadu_si512((const __m256i*)t));
			uint32_t mm = addmask[ktemporalq];
			__m512i ctl8 = _mm512_mask_mov_epi64(_mm512_set16_epi8(7, 7, 7, 7, 7, 7, 7, 7, -1, -1, -1, -1, -1, -1, -1, -1), mm, _mm512_set1_epi8(-1));

			s0 = _mm512_add_epi8(_mm512_add_epi8(s0, prev), _mm512_slli_epi64(s0, 8));
			s0 = _mm512_add_epi8(s0, _mm512_slli_epi64(s0, 16));
			s0 = _mm512_add_epi8(s0, _mm512_slli_epi64(s0, 32));
			s0 = _mm512_mask_mov_epi64(s0, ktemporalq, t0);

			__m512i stmp;

			s0 = _mm512_add_epi8(s0, _mm512_shuffle_epi8(s0, ctl8));

			stmp = _mm512_add_epi8(s0, _mm512_permutexvar_epi8(_mm512_set_epi8(
				47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			), s0));
			s0 = _mm512_mask_mov_epi64(stmp, mm >> 8, s0);

			stmp = _mm512_add_epi8(s0, _mm512_permutexvar_epi8(_mm512_set_epi8(
				31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
				31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			), s0));
			s0 = _mm512_mask_mov_epi64(stmp, mm >> 16, s0);

			_mm512_storeu_si512((__m512i*)p, s0);

			/*
			 * この maskz による追加のレイテンシ (2clk) は UnpackForDelta によって完全に隠蔽されるので、
			 *   - VPERMB してから VPAND
			 *   - VMOVDQU64 してから VPERMB
			 * などに置き換えると逆に遅くなる。
			 */
			prev = _mm512_maskz_permutexvar_epi8(3, _mm512_set_epi8(
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, 63, 63
			), s0);
		}
	}

	int shift = 0;
	auto tt = pPrevBegin + cbStride;
	for (auto pp = pDstBegin + cbStride; pp != pDstEnd; pp += cbStride, tt += cbStride)
	{
		__m512i prev = _mm512_set1_epi8((char)0);

		auto t = tt;
		for (auto p = pp; p != pp + cbStride; p += 64, t += 64)
		{
			auto [s0, ktemporalq] = UnpackForDelta<CODEFEATURE_AVX512_ICL>(q, r);

			__m512i top = _mm512_loadu_si512((const __m512i*)(p - cbStride));
			__m512i t0 = _mm512_sub_epi8(_mm512_add_epi8(s0, _mm512_loadu_si512((const __m512i*)t)), top);
			uint32_t mm = addmask[ktemporalq];
			__m512i ctl8 = _mm512_mask_mov_epi64(_mm512_set16_epi8(7, 7, 7, 7, 7, 7, 7, 7, -1, -1, -1, -1, -1, -1, -1, -1), mm, _mm512_set1_epi8(-1));

			s0 = _mm512_add_epi8(_mm512_add_epi8(s0, prev), _mm512_slli_epi64(s0, 8));
			s0 = _mm512_add_epi8(s0, _mm512_slli_epi64(s0, 16));
			s0 = _mm512_add_epi8(s0, _mm512_slli_epi64(s0, 32));
			s0 = _mm512_mask_mov_epi64(s0, ktemporalq, t0);

			__m512i stmp;

			s0 = _mm512_add_epi8(s0, _mm512_shuffle_epi8(s0, ctl8));

			stmp = _mm512_add_epi8(s0, _mm512_permutexvar_epi8(_mm512_set_epi8(
				47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			), s0));
			s0 = _mm512_mask_mov_epi64(stmp, mm >> 8, s0);

			stmp = _mm512_add_epi8(s0, _mm512_permutexvar_epi8(_mm512_set_epi8(
				31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
				31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			), s0));
			s0 = _mm512_mask_mov_epi64(stmp, mm >> 16, s0);

			_mm512_storeu_si512((__m512i*)p, _mm512_add_epi8(s0, top));

			prev = _mm512_maskz_permutexvar_epi8(3, _mm512_set_epi8(
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, 63, 63
			), s0);
		}
	}
}
