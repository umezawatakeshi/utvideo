/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "POD.h"

extern void* enabler;

template<int F, int NTS /* num_tables scale */, int TGI /* table group index */>
static inline FORCEINLINE void IncrementCounters8(__m128i xmm, uint32_t pCountTable[][256])
{
	static constexpr int TPC = NUM_COUNT_TABLES_PER_CHANNEL<8>;
	static constexpr int NT = TPC < NTS ? 1 : TPC / NTS;
	static constexpr int TO = NT * TGI % TPC;

#if defined(__i386__)
	uint32_t x;

	x = _mm_cvtsi128_si32(xmm);
	++pCountTable[ 0 % NT + TO][x & 0xff];
	++pCountTable[ 1 % NT + TO][(x >> 8) & 0xff];
	++pCountTable[ 2 % NT + TO][(x >> 16) & 0xff];
	++pCountTable[ 3 % NT + TO][(x >> 24) & 0xff];

	x = _mm_extract_epi32(xmm, 1);
	++pCountTable[ 4 % NT + TO][x & 0xff];
	++pCountTable[ 5 % NT + TO][(x >> 8) & 0xff];
	++pCountTable[ 6 % NT + TO][(x >> 16) & 0xff];
	++pCountTable[ 7 % NT + TO][(x >> 24) & 0xff];

	x = _mm_extract_epi32(xmm, 2);
	++pCountTable[ 8 % NT + TO][x & 0xff];
	++pCountTable[ 9 % NT + TO][(x >> 8) & 0xff];
	++pCountTable[10 % NT + TO][(x >> 16) & 0xff];
	++pCountTable[11 % NT + TO][(x >> 24) & 0xff];

	x = _mm_extract_epi32(xmm, 3);
	++pCountTable[12 % NT + TO][x & 0xff];
	++pCountTable[13 % NT + TO][(x >> 8) & 0xff];
	++pCountTable[14 % NT + TO][(x >> 16) & 0xff];
	++pCountTable[15 % NT + TO][(x >> 24) & 0xff];
#elif defined(__x86_64__)
	uint64_t x;

	x = _mm_cvtsi128_si64(xmm);
	++pCountTable[ 0 % NT + TO][x & 0xff];
	++pCountTable[ 1 % NT + TO][(x >> 8) & 0xff];
	++pCountTable[ 2 % NT + TO][(x >> 16) & 0xff];
	++pCountTable[ 3 % NT + TO][(x >> 24) & 0xff];
	++pCountTable[ 4 % NT + TO][(x >> 32) & 0xff];
	++pCountTable[ 5 % NT + TO][(x >> 40) & 0xff];
	++pCountTable[ 6 % NT + TO][(x >> 48) & 0xff];
	++pCountTable[ 7 % NT + TO][(x >> 56) & 0xff];

	x = _mm_extract_epi64(xmm, 1);
	++pCountTable[ 8 % NT + TO][x & 0xff];
	++pCountTable[ 9 % NT + TO][(x >> 8) & 0xff];
	++pCountTable[10 % NT + TO][(x >> 16) & 0xff];
	++pCountTable[11 % NT + TO][(x >> 24) & 0xff];
	++pCountTable[12 % NT + TO][(x >> 32) & 0xff];
	++pCountTable[13 % NT + TO][(x >> 40) & 0xff];
	++pCountTable[14 % NT + TO][(x >> 48) & 0xff];
	++pCountTable[15 % NT + TO][(x >> 56) & 0xff];
#endif
}

template<int F, int NTS, int TGI>
static inline FORCEINLINE void IncrementCounters10(__m128i xmm, uint32_t pCountTable[][1024])
{
	static constexpr int TPC = NUM_COUNT_TABLES_PER_CHANNEL<10>;
	static constexpr int NT = TPC < NTS ? 1 : TPC / NTS;
	static constexpr int TO = NT * TGI % TPC;

#if defined(__i386__)
	uint32_t x;

	x = _mm_cvtsi128_si32(xmm);
	++pCountTable[0 % NT + TO][x & 0xffff];
	++pCountTable[1 % NT + TO][(x >> 16) & 0xffff];

	x = _mm_extract_epi32(xmm, 1);
	++pCountTable[2 % NT + TO][x & 0xffff];
	++pCountTable[3 % NT + TO][(x >> 16) & 0xffff];

	x = _mm_extract_epi32(xmm, 2);
	++pCountTable[4 % NT + TO][x & 0xffff];
	++pCountTable[5 % NT + TO][(x >> 16) & 0xffff];

	x = _mm_extract_epi32(xmm, 3);
	++pCountTable[6 % NT + TO][x & 0xffff];
	++pCountTable[7 % NT + TO][(x >> 16) & 0xffff];
#elif defined(__x86_64__)
	uint64_t x;

	x = _mm_cvtsi128_si64(xmm);
	++pCountTable[0 % NT + TO][x & 0xffff];
	++pCountTable[1 % NT + TO][(x >> 16) & 0xffff];
	++pCountTable[2 % NT + TO][(x >> 32) & 0xffff];
	++pCountTable[3 % NT + TO][(x >> 48) & 0xffff];

	x = _mm_extract_epi64(xmm, 1);
	++pCountTable[4 % NT + TO][x & 0xffff];
	++pCountTable[5 % NT + TO][(x >> 16) & 0xffff];
	++pCountTable[6 % NT + TO][(x >> 32) & 0xffff];
	++pCountTable[7 % NT + TO][(x >> 48) & 0xffff];
#endif
}

template<int F>
static inline FORCEINLINE __m128i tuned_PredictLeft8Element(__m128i prev, __m128i value)
{
	__m128i left = _mm_alignr_epi8(value, prev, 15);
	__m128i residual = _mm_sub_epi8(value, left);
	return residual;
}

template<int F>
static inline FORCEINLINE __m256i tuned_PredictLeft8Element(__m256i prev, __m256i value)
{
	__m256i left = _mm256_alignr_epi8(value, _mm256_permute2x128_si256(value, prev, 0x03), 15);
	__m256i residual = _mm256_sub_epi8(value, left);
	return residual;
}

template<int F>
static inline FORCEINLINE __m512i tuned_PredictLeft8Element(__m512i prev, __m512i value)
{
	__m512i left = _mm512_permutex2var_epi8(prev, _mm512_set_epi8(
		126, 125, 124, 123, 122, 121, 120, 119,
		118, 117, 116, 115, 114, 113, 112, 111,
		110, 109, 108, 107, 106, 105, 104, 103,
		102, 101, 100,  99,  98,  97,  96,  95,
		 94,  93,  92,  91,  90,  89,  88,  87,
		 86,  85,  84,  83,  82,  81,  80,  79,
		 78,  77,  76,  75,  74,  73,  72,  71,
		 70,  69,  68,  67,  66,  65,  64,  63
	), value); // prev はこの後は使われないので、VPERMT2B で prev の方が dst になって上書きされることを期待している。
	__m512i residual = _mm512_sub_epi8(value, left);
	return residual;
}

template<int F, bool DoCount, int NTS, int TGI>
static inline FORCEINLINE __m128i tuned_PredictLeftAndCount8Element(__m128i prev, __m128i value, uint32_t pCountTable[][256])
{
	__m128i residual = tuned_PredictLeft8Element<F>(prev, value);
	if (DoCount)
		IncrementCounters8<F, NTS, TGI>(residual, pCountTable);
	return residual;
}

template<int F>
void tuned_PredictCylindricalLeftAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t pCountTable[][256])
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

	for (; p <= pSrcEnd - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount8Element<F, true, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - *(p - 1);
		++pCountTable[0][*q];
	}
}

#ifdef GENERATE_SSE41
template void tuned_PredictCylindricalLeftAndCount8<CODEFEATURE_SSE41>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t pCountTable[][256]);
#endif

#ifdef GENERATE_AVX1
template void tuned_PredictCylindricalLeftAndCount8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t pCountTable[][256]);
#endif


template<int F>
static inline FORCEINLINE VECTOR2<__m128i> /* value0, nextprev */ tuned_RestoreLeft8Element(__m128i prev, __m128i s0)
{
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 1));
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 2));
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 4));
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 8));
	s0 = _mm_add_epi8(s0, prev);
	return { s0, _mm_shuffle_epi8(s0, _mm_set1_epi8(15)) };
}

template<int F>
static inline FORCEINLINE VECTOR2<__m256i> /* value0, nextprev */ tuned_RestoreLeft8Element(__m256i prev, __m256i s0)
{
	s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 1));
	s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 2));
	s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 4));
	s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 8));
	s0 = _mm256_add_epi8(s0, _mm256_shuffle_epi8(_mm256_broadcastsi128_si256(_mm256_castsi256_si128(s0)), _mm256_set_epi8(
		15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	)));
	s0 = _mm256_add_epi8(s0, prev);
	prev = _mm256_shuffle_epi8(_mm256_permute2x128_si256(s0, s0, 0x11), _mm256_set1_epi8(15));
	return { s0, prev };
}

template<int F, typename std::enable_if<F == CODEFEATURE_AVX512_ICL>::type*& = enabler>
static inline FORCEINLINE VECTOR2<__m512i> /* value0, nextprev */ tuned_RestoreLeft8Element(__m512i prev, __m512i s0)
{
	s0 = _mm512_add_epi8(s0, _mm512_bslli_epi128(s0, 1));
	s0 = _mm512_add_epi8(s0, _mm512_bslli_epi128(s0, 2));
	s0 = _mm512_add_epi8(s0, _mm512_bslli_epi128(s0, 4));
	s0 = _mm512_add_epi8(s0, _mm512_bslli_epi128(s0, 8));
	__m512i stmp;
	// VPERMB は PSHUFB と違って不要な要素に 0 を埋めることは命令単体ではできない（zeroing-mask を使えば一応できるが）ため、
	// permutexvar (VPERMB) して add した後にマスクで加算をキャンセルする処理を行っている。
	//
	// 代わりに permute2xvar (VPERMT2B) を使って 0 を埋めて add するのとどっちが速いだろうか？
	//
	// icl では VPERMB は T/L1/L2 = 1/3/3 なのに対して VPERMT2B は 2/4/5/4 で、マスクを考慮すると VPERMT2B に優位性はない（同等）。
	// マスクでキャンセルするとオペランド依存グラフ（＝命令スケジューリング）に自由度が生まれる（実際その自由度を生かして記述している）一方、
	// 命令数が増えるためデコーダや命令キャッシュに負荷がかかる。よってどちらが速いかは場合による（＝計測しないと分からない）。
	stmp = _mm512_add_epi8(s0, _mm512_permutexvar_epi8(_mm512_set_epi8(
		47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	), s0));
	s0 = _mm512_mask_mov_epi64(s0, 0xcc, stmp);
	stmp = _mm512_add_epi8(s0, _mm512_permutexvar_epi8(_mm512_set_epi8(
		31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
		31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	), stmp));
	s0 = _mm512_mask_mov_epi64(s0, 0xf0, stmp);
	s0 = _mm512_add_epi8(s0, prev);
	prev = _mm512_permutexvar_epi8(_mm512_set1_epi8(63), s0);
	return { s0, prev };
}

template<int F>
void tuned_RestoreCylindricalLeft8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

	for (; p <= pSrcEnd - 16; p += 16, q += 16)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		auto result = tuned_RestoreLeft8Element<F>(prev, s0);
		_mm_storeu_si128((__m128i *)q, result.v0);
		prev = result.v1;
	}

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *(q - 1) + *p;
	}
}

#ifdef GENERATE_SSE41
template void tuned_RestoreCylindricalLeft8<CODEFEATURE_SSE41>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestoreCylindricalLeft8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
#endif


template<int F>
static inline FORCEINLINE __m128i tuned_PredictLeft10Element(__m128i prev, __m128i value)
{
	__m128i left = _mm_alignr_epi8(value, prev, 14);
	__m128i residual = _mm_and_si128(_mm_sub_epi16(value, left), _mm_set1_epi16(CSymbolBits<10>::maskval));
	return residual;
}

template<int F, bool DoCount, int NTS, int TGI>
static inline FORCEINLINE __m128i tuned_PredictLeftAndCount10Element(__m128i prev, __m128i value, uint32_t pCountTable[][1024])
{
	__m128i residual = tuned_PredictLeft10Element<F>(prev, value);
	if (DoCount)
		IncrementCounters10<F, NTS, TGI>(residual, pCountTable);
	return residual;
}

template<int F>
void tuned_PredictCylindricalLeftAndCount10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, uint32_t pCountTable[][1024])
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi16(CSymbolBits<10>::midval);

	for (; p <= pSrcEnd - 8; p += 8, q += 8)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount10Element<F, true, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}

	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*p - *(p - 1)) & CSymbolBits<10>::maskval;
		++pCountTable[0][*q];
	}
}

#ifdef GENERATE_SSE41
template void tuned_PredictCylindricalLeftAndCount10<CODEFEATURE_SSE41>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, uint32_t pCountTable[][1024]);
#endif

#ifdef GENERATE_AVX1
template void tuned_PredictCylindricalLeftAndCount10<CODEFEATURE_AVX1>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, uint32_t pCountTable[][1024]);
#endif


template<int F, bool NeedMask = true>
static inline FORCEINLINE VECTOR2<__m128i> /* value0, nextprev */ tuned_RestoreLeft10Element(__m128i prev, __m128i s0)
{
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 2));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 4));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 8));
	s0 = _mm_add_epi16(s0, prev);
	if (NeedMask)
		s0 = _mm_and_si128(s0, _mm_set1_epi16(CSymbolBits<10>::maskval));
	return { s0, _mm_shuffle_epi8(s0, _mm_set2_epi8(15, 14)) };
}

template<int F>
void tuned_RestoreCylindricalLeft10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi16(CSymbolBits<10>::midval);

	for (; p <= pSrcEnd - 8; p += 8, q += 8)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		auto result = tuned_RestoreLeft10Element<F>(prev, s0);
		_mm_storeu_si128((__m128i *)q, result.v0);
		prev = result.v1;
	}

	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*(q - 1) + *p) & CSymbolBits<10>::maskval;
	}
}

#ifdef GENERATE_SSE41
template void tuned_RestoreCylindricalLeft10<CODEFEATURE_SSE41>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestoreCylindricalLeft10<CODEFEATURE_AVX1>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
#endif


template<int F>
static inline FORCEINLINE VECTOR2<__m128i> /* value0, nextprev */ tuned_RestoreLeft16Element(__m128i prev, __m128i s0)
{
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 2));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 4));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 8));
	s0 = _mm_add_epi16(s0, prev);
	return { s0, _mm_shuffle_epi8(s0, _mm_set2_epi8(15, 14)) };
}


template<int F>
static inline FORCEINLINE __m128i VECTORCALL tuned_PredictWrongMedian8Element(__m128i topprev, __m128i top, __m128i prev, __m128i value)
{
	__m128i left = _mm_alignr_epi8(value, prev, 15);
	__m128i topleft = _mm_alignr_epi8(top, topprev, 15);
	__m128i grad = _mm_sub_epi8(_mm_add_epi8(left, top), topleft);
	__m128i pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
	__m128i residual = _mm_sub_epi8(value, pred);

	return residual;
}

template<int F, bool DoCount, int NTS, int TGI>
static inline FORCEINLINE __m128i VECTORCALL tuned_PredictWrongMedianAndCount8Element(__m128i topprev, __m128i top, __m128i prev, __m128i value, uint32_t pCountTable[][256])
{
	__m128i residual = tuned_PredictWrongMedian8Element<F>(topprev, top, prev, value);
	if (DoCount)
		IncrementCounters8<F, NTS, TGI>(residual, pCountTable);
	return residual;
}

template<int F>
void tuned_PredictCylindricalWrongMedianAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256])
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);
	__m128i topprev;

	for (; p <= pSrcBegin + cbStride - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount8Element<F, true, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}

	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *p - *(p - 1);
		++pCountTable[0][*q];
	}

	prev = _mm_set1_epi8((char)0x80);
	topprev = _mm_set1_epi8((char)0x80);

	for (; p <= pSrcEnd - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i top = _mm_loadu_si128((const __m128i *)(p - cbStride));
		__m128i residual = tuned_PredictWrongMedianAndCount8Element<F, true, 1, 0>(topprev, top, prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
		topprev = top;
	}

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - median<uint8_t>(*(p - 1), *(p - cbStride), *(p - 1) + *(p - cbStride) - *(p - cbStride - 1));
		++pCountTable[0][*q];
	}
}

#ifdef GENERATE_SSE41
template void tuned_PredictCylindricalWrongMedianAndCount8<CODEFEATURE_SSE41>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
#endif

#ifdef GENERATE_AVX1
template void tuned_PredictCylindricalWrongMedianAndCount8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
#endif


template<int F>
void tuned_RestoreCylindricalWrongMedian8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

	for (; p <= pSrcBegin + cbStride - 16; p += 16, q += 16)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		auto result = tuned_RestoreLeft8Element<F>(prev, s0);
		_mm_storeu_si128((__m128i *)q, result.v0);
		prev = result.v1;
	}

	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *(q - 1) + *p;
	}

	__m128i left = _mm_setzero_si128();
	__m128i topleft = _mm_setzero_si128();

	for (; p < pSrcEnd; p++, q++)
	{
		__m128i top = _mm_cvtsi32_si128(*(const uint32_t *)(q - cbStride));
		__m128i grad = _mm_add_epi8(left, _mm_sub_epi8(top, topleft));
		__m128i pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
		__m128i value = _mm_add_epi8(pred, _mm_cvtsi32_si128(*(const uint32_t *)p));
		*q = _mm_cvtsi128_si32(value);
		left = value;
		topleft = top;
	}
}

#ifdef GENERATE_SSE41
template void tuned_RestoreCylindricalWrongMedian8<CODEFEATURE_SSE41>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestoreCylindricalWrongMedian8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif


template<int F, bool DoCount>
static inline void tuned_PredictPlanarGradientAndMayCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256])
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

	for (; p <= pSrcBegin + cbStride - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount8Element<F, DoCount, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}

	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *p - *(p - 1);
		if (DoCount)
			++pCountTable[0][*q];
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		prev = _mm_setzero_si128();

		for (; p <= pp + cbStride - 16; p += 16, q += 16)
		{
			__m128i value = _mm_sub_epi8(_mm_loadu_si128((const __m128i *)p), _mm_loadu_si128((const __m128i *)(p - cbStride)));
			__m128i residual = tuned_PredictLeftAndCount8Element<F, DoCount, 1, 0>(prev, value, pCountTable);
			_mm_storeu_si128((__m128i *)q, residual);
			prev = value;
		}

		for (; p < pp + cbStride; p++, q++)
		{
			*q = *p - (*(p - 1) + *(p - cbStride) - *(p - cbStride - 1));
			if (DoCount)
				++pCountTable[0][*q];
		}
	}
}

template<int F>
void tuned_PredictPlanarGradientAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256])
{
	tuned_PredictPlanarGradientAndMayCount8<F, true>(pDst, pSrcBegin, pSrcEnd, cbStride, pCountTable);
}

template<int F>
void tuned_PredictPlanarGradient8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride)
{
	tuned_PredictPlanarGradientAndMayCount8<F, false>(pDst, pSrcBegin, pSrcEnd, cbStride, NULL);
}

#ifdef GENERATE_SSE41
template void tuned_PredictPlanarGradientAndCount8<CODEFEATURE_SSE41>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
template void tuned_PredictPlanarGradient8<CODEFEATURE_SSE41>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_PredictPlanarGradientAndCount8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
template void tuned_PredictPlanarGradient8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif


template<int F>
void tuned_RestorePlanarGradient8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

	for (; p <= pSrcBegin + cbStride - 16; p += 16, q += 16)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		auto result = tuned_RestoreLeft8Element<F>(prev, s0);
		_mm_storeu_si128((__m128i *)q, result.v0);
		prev = result.v1;
	}

	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *(q - 1) + *p;
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		prev = _mm_set1_epi8((char)0);

		for (; p <= pp + cbStride - 16; p += 16, q += 16)
		{
			__m128i s0 = _mm_loadu_si128((const __m128i *)p);
			auto result = tuned_RestoreLeft8Element<F>(prev, s0);
			_mm_storeu_si128((__m128i *)q, _mm_add_epi8(result.v0, _mm_loadu_si128((const __m128i *)(q - cbStride))));
			prev = result.v1;
		}

		for (; p < pp + cbStride; p++, q++)
		{
			*q = *p + (*(q - 1) + *(q - cbStride) - *(q - cbStride - 1));
		}
	}
}

#ifdef GENERATE_SSE41
template void tuned_RestorePlanarGradient8<CODEFEATURE_SSE41>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestorePlanarGradient8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif


template<int F, bool DoCount>
static inline void tuned_PredictPlanarGradientAndMayCount10(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride, uint32_t pCountTable[][1024])
{
	auto p = pSrcBegin;
	auto q = pDst;
	auto nStride = cbStride / 2;

	__m128i prev = _mm_set1_epi16(CSymbolBits<10>::midval);

	for (; p <= pSrcBegin + nStride - 8; p += 8, q += 8)
	{
		__m128i value = _mm_loadu_si128((const __m128i*)p);
		__m128i residual = tuned_PredictLeftAndCount10Element<F, DoCount, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i*)q, residual);
		prev = value;
	}

	for (; p < pSrcBegin + nStride; p++, q++)
	{
		*q = (*p - *(p - 1)) & CSymbolBits<10>::maskval;
		if (DoCount)
			++pCountTable[0][*q];
	}

	for (auto pp = pSrcBegin + nStride; pp != pSrcEnd; pp += nStride)
	{
		prev = _mm_setzero_si128();

		for (; p <= pp + nStride - 8; p += 8, q += 8)
		{
			__m128i value = _mm_sub_epi16(_mm_loadu_si128((const __m128i*)p), _mm_loadu_si128((const __m128i*)(p - nStride)));
			__m128i residual = tuned_PredictLeftAndCount10Element<F, DoCount, 1, 0>(prev, value, pCountTable);
			_mm_storeu_si128((__m128i*)q, residual);
			prev = value;
		}

		for (; p < pp + nStride; p++, q++)
		{
			*q = (*p - (*(p - 1) + *(p - nStride) - *(p - nStride - 1))) & CSymbolBits<10>::maskval;
			if (DoCount)
				++pCountTable[0][*q];
		}
	}
}

template<int F>
void tuned_PredictPlanarGradientAndCount10(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride, uint32_t pCountTable[][1024])
{
	tuned_PredictPlanarGradientAndMayCount10<F, true>(pDst, pSrcBegin, pSrcEnd, cbStride, pCountTable);
}

template<int F>
void tuned_PredictPlanarGradient10(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride)
{
	tuned_PredictPlanarGradientAndMayCount10<F, false>(pDst, pSrcBegin, pSrcEnd, cbStride, NULL);
}

#ifdef GENERATE_SSE41
template void tuned_PredictPlanarGradientAndCount10<CODEFEATURE_SSE41>(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride, uint32_t pCountTable[][1024]);
template void tuned_PredictPlanarGradient10<CODEFEATURE_SSE41>(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_PredictPlanarGradientAndCount10<CODEFEATURE_AVX1>(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride, uint32_t pCountTable[][1024]);
template void tuned_PredictPlanarGradient10<CODEFEATURE_AVX1>(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride);
#endif


template<int F>
void tuned_RestorePlanarGradient10(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride)
{
	auto p = pSrcBegin;
	auto q = pDst;
	auto nStride = cbStride / 2;

	__m128i prev = _mm_set1_epi16(CSymbolBits<10>::midval);

	for (; p <= pSrcBegin + nStride - 8; p += 8, q += 8)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i*)p);
		auto result = tuned_RestoreLeft10Element<F>(prev, s0);
		_mm_storeu_si128((__m128i*)q, result.v0);
		prev = result.v1;
	}

	for (; p < pSrcBegin + nStride; p++, q++)
	{
		*q = (*(q - 1) + *p) & CSymbolBits<10>::maskval;
	}

	for (auto pp = pSrcBegin + nStride; pp != pSrcEnd; pp += nStride)
	{
		prev = _mm_set1_epi16((char)0);

		for (; p <= pp + nStride - 8; p += 8, q += 8)
		{
			__m128i s0 = _mm_loadu_si128((const __m128i*)p);
			auto result = tuned_RestoreLeft10Element<F, false>(prev, s0);
			_mm_storeu_si128((__m128i*)q, _mm_and_si128(_mm_add_epi16(result.v0, _mm_loadu_si128((const __m128i*)(q - nStride))), _mm_set1_epi16(CSymbolBits<10>::maskval)));
			prev = result.v1;
		}

		for (; p < pp + nStride; p++, q++)
		{
			*q = (*p + (*(q - 1) + *(q - nStride) - *(q - nStride - 1))) & CSymbolBits<10>::maskval;
		}
	}
}

#ifdef GENERATE_SSE41
template void tuned_RestorePlanarGradient10<CODEFEATURE_SSE41>(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestorePlanarGradient10<CODEFEATURE_AVX1>(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride);
#endif
