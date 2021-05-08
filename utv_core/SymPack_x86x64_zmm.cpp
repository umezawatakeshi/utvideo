/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "SymPack_x86x64.h"

#if !defined(GENERATE_AVX512_ICL)
#error
#endif

struct VREMBITS_RESULT
{
	__m512i rembits;
	__mmask8 kiszero;
};

template<int F>
static inline FORCEINLINE VREMBITS_RESULT VECTORCALL GetVRemBits(__m512i x)
{
	__mmask8 kiszero = _mm512_cmpeq_epi64_mask(x, _mm512_set1_epi8(0));
	__mmask64 knegative = _mm512_cmplt_epi8_mask(x, _mm512_set1_epi8(0));
	__m512i notw = _mm512_xor_si512(x, _mm512_set1_epi8(-1));
	__m512i z = _mm512_mask_mov_epi8(x, knegative, notw);

	z = _mm512_or_si512(z, _mm512_slli_epi64(z, 32));
	z = _mm512_or_si512(z, _mm512_slli_epi64(z, 16));
	z = _mm512_or_si512(_mm512_or_si512(z, _mm512_set1_epi64(1ULL << 56)), _mm512_slli_epi64(z, 8));
	__m512i lz = _mm512_lzcnt_epi64(z);

	__m512i rembits = _mm512_sub_epi64(lz, _mm512_set1_epi64(1));

	return { rembits, kiszero };
};

template<int F, bool Delta>
static inline FORCEINLINE void VECTORCALL PackElement(uint8_t*& q, uint8_t*& r, __m512i w, __m512i t)
{
	static constexpr int MODEBITS = Delta ? 4 : 3;
	static constexpr uint32_t MODEPEXT32 = Delta ? 0x0f0f0f0f : 0x07070707;
	static constexpr uint64_t MODEPEXT64 = Delta ? 0x0f0f0f0f0f0f0f0fULL : 0x0707070707070707ULL;

	auto [rembitsw, kiszerow] = GetVRemBits<F>(w);
	__m512i rembits;
	__m512i rembitszeroed;
	__mmask8 ktemporal;
	if (!Delta)
	{
		rembits = rembitsw;
		rembitszeroed = _mm512_mask_mov_epi64(rembitsw, kiszerow, _mm512_set1_epi64(8));;
	}
	else
	{
		auto [rembitst, kiszerot] = GetVRemBits<F>(t);
		rembitsw = _mm512_mask_mov_epi64(rembitsw, kiszerow, _mm512_set1_epi64(8));
		rembitst = _mm512_mask_mov_epi64(rembitst, kiszerot, _mm512_set1_epi64(8));
		ktemporal = _mm512_cmpge_epi64_mask(rembitst, rembitsw);

		w = _mm512_mask_mov_epi64(w, ktemporal, t);
		rembits = _mm512_mask_mov_epi64(rembitsw, ktemporal, rembitst);
		rembitszeroed = rembits;
	}
#if 1
	// こう書き換えると下で言っているような無駄な kmov が発生しない分だけ命令が減るが、128bitでやっていた計算を一部512bitで行うことになるので、実行ユニットが混むかもしれない
	__m512i vmodes = _mm512_subs_epu8(_mm512_set1_epi64(7), rembitszeroed);
	if (Delta)
		vmodes = _mm512_mask_or_epi64(vmodes, ktemporal, vmodes, _mm512_set1_epi64(8));
	__m128i vmodes64 = _mm512_castsi512_si128(_mm512_permutexvar_epi8(_mm512_set8_epi8(56, 48, 40, 32, 24, 16, 8, 0), vmodes));
	uint32_t modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes64), MODEPEXT64);

	__m128i vmask = _mm_shuffle_epi8(_mm_set8_epi8((char)0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0), vmodes64);
#else
	__m128i rembits64 = _mm512_castsi512_si128(_mm512_permutexvar_epi8(_mm512_set8_epi8(56, 48, 40, 32, 24, 16, 8, 0), rembitszeroed));
	__m128i vmodes = _mm_subs_epu8(_mm_set1_epi8(7), rembits64);
	if (Delta)
	{
		/*
		 * コンパイラの最適化が甘いせいで、下で __mmask8 の ktemporal を _mm_maskz_mov_epi8 に渡すために __mmask16 にしようとして
		 * 無意味に汎用レジスタに kmovb した後 kmovw で戻す（本来はそんなことしなくても直接渡して同じ結果になる）が、
		 * それでも pdep で modes の方にビットを埋めるのと同じか速い…はず。
		 * modes はメモリに書くだけで他の処理に依存性がないので、スループットの方が重要かもしれない（どっちも1だが）。
		 */
		vmodes = _mm_or_si128(vmodes, _mm_maskz_mov_epi8(ktemporal, _mm_set1_epi8(8)));

		// 「pdep で modes の方にビットを埋める」というのはこういうコード。
		//uint32_t modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes), 0x0f0f0f0f0f0f0f0fULL) | _pdep_u32(ktemporal, 0x88888888);
	}
	uint32_t modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes), MODEPEXT64);

	__m128i vmask = _mm_shuffle_epi8(_mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, (char)0xff), rembits64);
#endif
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
	auto q = pPacked;
	auto r = pControl;

	// 分かりやすいけど恐らく遅いコード

	__m512i prevmask = _mm512_set_epi8(
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char)0xff);

	{
		__m512i prev = _mm512_set_epi8(
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char)0x80);

		auto t = pPrevBegin;
		for (auto p = pDstBegin; p != pDstBegin + cbStride; p += 64, t += 64)
		{
			auto [s0, ktemporalq] = UnpackForDelta<CODEFEATURE_AVX512_ICL>(q, r);

			auto kspatialb = ~_mm512_movepi8_mask(_mm512_movm_epi64(ktemporalq));

			__m512i t0 = _mm512_add_epi8(s0, _mm512_loadu_si512((const __m256i*)t));
			s0 = _mm512_mask_add_epi8(t0, kspatialb, s0, prev);

			kspatialb &= ~1ULL;
			__m512i stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				62, 61, 60, 59, 58, 57, 56, 55,
				54, 53, 52, 51, 50, 49, 48, 47,
				46, 45, 44, 43, 42, 41, 40, 39,
				38, 37, 36, 35, 34, 33, 32, 31,
				30, 29, 28, 27, 26, 25, 24, 23,
				22, 21, 20, 19, 18, 17, 16, 15,
				14, 13, 12, 11, 10,  9,  8,  7,
				 6,  5,  4,  3,  2,  1,  0, -1
			), s0); // 1バイトシフト
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 1;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				61, 60, 59, 58, 57, 56, 55, 54,
				53, 52, 51, 50, 49, 48, 47, 46,
				45, 44, 43, 42, 41, 40, 39, 38,
				37, 36, 35, 34, 33, 32, 31, 30,
				29, 28, 27, 26, 25, 24, 23, 22,
				21, 20, 19, 18, 17, 16, 15, 14,
				13, 12, 11, 10,  9,  8,  7,  6,
				 5,  4,  3,  2,  1,  0, -1, -1
			), s0); // 2バイトシフト
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 2;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				59, 58, 57, 56, 55, 54, 53, 52,
				51, 50, 49, 48, 47, 46, 45, 44,
				43, 42, 41, 40, 39, 38, 37, 36,
				35, 34, 33, 32, 31, 30, 29, 28,
				27, 26, 25, 24, 23, 22, 21, 20,
				19, 18, 17, 16, 15, 14, 13, 12,
				11, 10,  9,  8,  7,  6,  5,  4,
				 3,  2,  1,  0, -1, -1, -1, -1
			), s0); // 4バイトシフト 以下同様
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 4;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				55, 54, 53, 52, 51, 50, 49, 48,
				47, 46, 45, 44, 43, 42, 41, 40,
				39, 38, 37, 36, 35, 34, 33, 32,
				31, 30, 29, 28, 27, 26, 25, 24,
				23, 22, 21, 20, 19, 18, 17, 16,
				15, 14, 13, 12, 11, 10,  9,  8,
				 7,  6,  5,  4,  3,  2,  1,  0,
				-1, -1, -1, -1, -1, -1, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 8;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				47, 46, 45, 44, 43, 42, 41, 40,
				39, 38, 37, 36, 35, 34, 33, 32,
				31, 30, 29, 28, 27, 26, 25, 24,
				23, 22, 21, 20, 19, 18, 17, 16,
				15, 14, 13, 12, 11, 10,  9,  8,
				 7,  6,  5,  4,  3,  2,  1,  0,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 16;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				31, 30, 29, 28, 27, 26, 25, 24,
				23, 22, 21, 20, 19, 18, 17, 16,
				15, 14, 13, 12, 11, 10,  9,  8,
				 7,  6,  5,  4,  3,  2,  1,  0,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			_mm512_storeu_si512((__m512i*)p, s0);

			prev = _mm512_and_epi32(_mm512_permutexvar_epi8(_mm512_set_epi8(
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, 63
			), s0), prevmask);
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

			auto kspatialb = ~_mm512_movepi8_mask(_mm512_movm_epi64(ktemporalq));

			__m512i top = _mm512_loadu_si512((const __m512i*)(p - cbStride));
			__m512i t0 = _mm512_sub_epi8(_mm512_add_epi8(s0, _mm512_loadu_si512((const __m512i*)t)), top);
			s0 = _mm512_mask_add_epi8(t0, kspatialb, s0, prev);

			kspatialb &= ~1ULL;
			__m512i stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				62, 61, 60, 59, 58, 57, 56, 55,
				54, 53, 52, 51, 50, 49, 48, 47,
				46, 45, 44, 43, 42, 41, 40, 39,
				38, 37, 36, 35, 34, 33, 32, 31,
				30, 29, 28, 27, 26, 25, 24, 23,
				22, 21, 20, 19, 18, 17, 16, 15,
				14, 13, 12, 11, 10,  9,  8,  7,
				 6,  5,  4,  3,  2,  1,  0, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 1;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				61, 60, 59, 58, 57, 56, 55, 54,
				53, 52, 51, 50, 49, 48, 47, 46,
				45, 44, 43, 42, 41, 40, 39, 38,
				37, 36, 35, 34, 33, 32, 31, 30,
				29, 28, 27, 26, 25, 24, 23, 22,
				21, 20, 19, 18, 17, 16, 15, 14,
				13, 12, 11, 10,  9,  8,  7,  6,
				 5,  4,  3,  2,  1,  0, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 2;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				59, 58, 57, 56, 55, 54, 53, 52,
				51, 50, 49, 48, 47, 46, 45, 44,
				43, 42, 41, 40, 39, 38, 37, 36,
				35, 34, 33, 32, 31, 30, 29, 28,
				27, 26, 25, 24, 23, 22, 21, 20,
				19, 18, 17, 16, 15, 14, 13, 12,
				11, 10,  9,  8,  7,  6,  5,  4,
				 3,  2,  1,  0, -1, -1, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 4;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				55, 54, 53, 52, 51, 50, 49, 48,
				47, 46, 45, 44, 43, 42, 41, 40,
				39, 38, 37, 36, 35, 34, 33, 32,
				31, 30, 29, 28, 27, 26, 25, 24,
				23, 22, 21, 20, 19, 18, 17, 16,
				15, 14, 13, 12, 11, 10,  9,  8,
				 7,  6,  5,  4,  3,  2,  1,  0,
				-1, -1, -1, -1, -1, -1, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 8;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				47, 46, 45, 44, 43, 42, 41, 40,
				39, 38, 37, 36, 35, 34, 33, 32,
				31, 30, 29, 28, 27, 26, 25, 24,
				23, 22, 21, 20, 19, 18, 17, 16,
				15, 14, 13, 12, 11, 10,  9,  8,
				 7,  6,  5,  4,  3,  2,  1,  0,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			kspatialb &= kspatialb << 16;
			stmp = _mm512_permutexvar_epi8(_mm512_set_epi8(
				31, 30, 29, 28, 27, 26, 25, 24,
				23, 22, 21, 20, 19, 18, 17, 16,
				15, 14, 13, 12, 11, 10,  9,  8,
				 7,  6,  5,  4,  3,  2,  1,  0,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1
			), s0);
			s0 = _mm512_mask_add_epi8(s0, kspatialb, s0, stmp);

			_mm512_storeu_si512((__m512i*)p, _mm512_add_epi8(s0, top));

			prev = _mm512_and_epi32(_mm512_permutexvar_epi8(_mm512_set_epi8(
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, 63
			), s0), prevmask);
		}
	}
}
