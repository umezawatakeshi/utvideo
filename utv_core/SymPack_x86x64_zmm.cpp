/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "SymPack_x86x64.h"

#if !defined(GENERATE_AVX512_ICL)
#error
#endif

// 正しく動くことは SDE で確認してある。（実機がないので速度は不明）

template<int F>
static inline FORCEINLINE void PackForIntra(uint8_t*& q, uint8_t*& r, __m512i w)
{
	__mmask8 knotzero = _mm512_cmpneq_epi64_mask(w, _mm512_set1_epi8(0));
	__mmask64 knegative = _mm512_cmplt_epi8_mask(w, _mm512_set1_epi8(0));
	__m512i notw = _mm512_xor_si512(w, _mm512_set1_epi8(-1));
	__m512i z = _mm512_mask_mov_epi8(w, knegative, notw);

	z = _mm512_or_si512(z, _mm512_slli_epi64(z, 32));
	z = _mm512_or_si512(z, _mm512_slli_epi64(z, 16));
	z = _mm512_or_si512(_mm512_or_si512(z, _mm512_set1_epi64(1ULL << 56)), _mm512_slli_epi64(z, 8));
	__m512i lz = _mm512_lzcnt_epi64(z);

	__m512i rembits = _mm512_sub_epi64(lz, _mm512_set1_epi64(1));
	rembits = _mm512_mask_mov_epi64(_mm512_set1_epi64(8), knotzero, rembits);

	__m128i rembits64 = _mm512_castsi512_si128(_mm512_permutexvar_epi8(_mm512_set8_epi8(56, 48, 40, 32, 24, 16, 8, 0), rembits));
	__m128i vmodes = _mm_subs_epu8(_mm_set1_epi8(7), rembits64);
	uint32_t modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes), 0x0707070707070707ULL);

	__m128i vmask = _mm_shuffle_epi8(_mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, (char)0xff), rembits64);
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
	r += 3;
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


template<int F>
static inline FORCEINLINE __m512i UnpackForIntra(const uint8_t*& q, const uint8_t *& r)
{
	uint32_t modes = *(uint32_t*)r;
	r += 3;

	__m128i vmodes = _mm_cvtsi64_si128(_pdep_u64(modes, 0x0707070707070707ULL));
	__m128i vmask = _mm_shuffle_epi8(_mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, (char)0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x00), vmodes);
	__mmask64 kmask = _mm_cvtsi128_si64(vmask);
	__m512i w = _mm512_maskz_expandloadu_epi8(kmask, q);
	q += _mm_popcnt_u64(kmask);

	// VPMULTISHIFTQB を使う場合
	// AVX2 までと同様に VPSLL(V)Q を使った場合より速いかどうかは不明
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

	return w;
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


template<int F>
static inline FORCEINLINE void VECTORCALL PackForDelta(uint8_t*& q, uint8_t*& r, __m512i w, __m512i t)
{
	auto getvrembits = [](__m512i x)
	{
		__mmask8 knotzero = _mm512_cmpneq_epi64_mask(x, _mm512_set1_epi8(0));
		__mmask64 knegative = _mm512_cmplt_epi8_mask(x, _mm512_set1_epi8(0));
		__m512i notw = _mm512_xor_si512(x, _mm512_set1_epi8(-1));
		__m512i z = _mm512_mask_mov_epi8(x, knegative, notw);

		z = _mm512_or_si512(z, _mm512_slli_epi64(z, 32));
		z = _mm512_or_si512(z, _mm512_slli_epi64(z, 16));
		z = _mm512_or_si512(_mm512_or_si512(z, _mm512_set1_epi64(1ULL << 56)), _mm512_slli_epi64(z, 8));
		__m512i lz = _mm512_lzcnt_epi64(z);

		__m512i rembits = _mm512_sub_epi64(lz, _mm512_set1_epi64(1));
		rembits = _mm512_mask_mov_epi64(_mm512_set1_epi64(8), knotzero, rembits);

		return rembits;
	};

	__m512i rembitsw = getvrembits(w);
	__m512i rembitst = getvrembits(t);
	__mmask8 ktemporal = _mm512_cmpge_epi64_mask(rembitst, rembitsw);

	__m512i rembits = _mm512_mask_mov_epi64(rembitsw, ktemporal, rembitst);
	w = _mm512_mask_mov_epi64(w, ktemporal, t);
	__m128i rembits64 = _mm512_castsi512_si128(_mm512_permutexvar_epi8(_mm512_set8_epi8(56, 48, 40, 32, 24, 16, 8, 0), rembits));
	/*
	 * コンパイラの最適化が甘いせいで、下で __mmask8 を _mm_maskz_mov_epi8 に渡すために __mmask16 にしようとして
	 * 無意味に汎用レジスタに kmovb した後 kmovw で戻す（本来はそんなことしなくても直接渡して同じ結果になる）が、
	 * それでも pdep で modes の方にビットを埋めるのと同じか速い…はず。
	 * modes はメモリに書くだけで他の処理に依存性がないので、スループットの方が重要かもしれない（どっちも1だが）。
	 */
	// こっちだと追加部分のレイテンシは kmovb, kmovw, vmovdqu8, por で 2+2+1+1 = 6 で、ちゃんと最適化してくれれば 1+1 = 2 だが、
	__m128i vmodes = _mm_or_si128(_mm_subs_epu8(_mm_set1_epi8(7), rembits64), _mm_maskz_mov_epi8(ktemporal, _mm_set1_epi8(8)));
	uint32_t modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes), 0x0f0f0f0f0f0f0f0fULL);
	// こっちだと kmovb, pdep, or で 2+3+1 = 6
	//__m128i vmodes = _mm_subs_epu8(_mm_set1_epi8(7), rembits64);
	//uint32_t modes = (uint32_t)_pext_u64(_mm_cvtsi128_si64(vmodes), 0x0f0f0f0f0f0f0f0fULL) | _pdep_u32(ktemporal, 0x88888888);

	__m128i vmask = _mm_shuffle_epi8(_mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, (char)0xff), rembits64);
	uint64_t mask = _mm_cvtsi128_si64(vmask);
	__mmask64 kmask = mask;

	w = _mm512_add_epi8(w, _mm512_srlv_epi64(_mm512_set1_epi8((char)0x80), rembits));
	w = _mm512_or_si512(_mm512_and_si512(w, _mm512_set1_epi16(0x00ff)), _mm512_srlv_epi64(_mm512_andnot_si512(_mm512_set1_epi16(0x00ff), w), rembits));
	rembits = _mm512_slli_epi64(rembits, 1);
	w = _mm512_or_si512(_mm512_and_si512(w, _mm512_set1_epi32(0x0000ffff)), _mm512_srlv_epi64(_mm512_andnot_si512(_mm512_set1_epi32(0x0000ffff), w), rembits));
	rembits = _mm512_slli_epi64(rembits, 1);
	w = _mm512_or_si512(_mm512_and_si512(w, _mm512_set1_epi64(0x00000000ffffffffULL)), _mm512_srlv_epi64(_mm512_andnot_si512(_mm512_set1_epi64(0x00000000ffffffffULL), w), rembits));

	_mm512_mask_compressstoreu_epi8(q, kmask, w);
	q += _mm_popcnt_u64(mask);
	*(uint32_t*)r = modes;
	r += 4;
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
