/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2)
#error
#endif

template<int F, int NTS /* num_tables scale */, int TGI /* table group index */>
static inline FORCEINLINE void IncrementCounters8(__m128i xmm, uint32_t pCountTable[][256])
{
	static constexpr int TPC = NUM_COUNT_TABLES_PER_CHANNEL<8>;
	static constexpr int NT = TPC < NTS ? 1 : TPC / NTS;
	static constexpr int TO = NT * TGI % TPC;

#ifdef __SSE4_1__
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
#endif
}

template<int F, int NTS, int TGI>
static inline FORCEINLINE void IncrementCounters10(__m128i xmm, uint32_t pCountTable[][1024])
{
	static constexpr int TPC = NUM_COUNT_TABLES_PER_CHANNEL<10>;
	static constexpr int NT = TPC < NTS ? 1 : TPC / NTS;
	static constexpr int TO = NT * TGI % TPC;

#ifdef __SSE4_1__
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

#ifdef __SSSE3__
	for (; p <= pSrcEnd - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount8Element<F, true, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}
#endif
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

template<int F>
static inline FORCEINLINE VECTOR3<__m128i> /* value0, value1, nextprev */ tuned_RestoreLeft8Element(__m128i prev, __m128i s0, __m128i s1)
{
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 1));
	s1 = _mm_add_epi8(s1, _mm_slli_si128(s1, 1));
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 2));
	s1 = _mm_add_epi8(s1, _mm_slli_si128(s1, 2));
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 4));
	s1 = _mm_add_epi8(s1, _mm_slli_si128(s1, 4));
	s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 8));
	s1 = _mm_add_epi8(s1, _mm_slli_si128(s1, 8));
	s0 = _mm_add_epi8(s0, prev);
	s1 = _mm_add_epi8(s1, _mm_shuffle_epi8(s0, _mm_set1_epi8(15)));
	return { s0, s1, _mm_shuffle_epi8(s1, _mm_set1_epi8(15)) };
}

template<int F>
void tuned_RestoreCylindricalLeft8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

#ifdef __SSSE3__
	for (; p <= pSrcEnd - 32; p += 32, q += 32)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		__m128i s1 = _mm_loadu_si128((const __m128i *)(p+16));
		auto result = tuned_RestoreLeft8Element<F>(prev, s0, s1);
		_mm_storeu_si128((__m128i *)q, result.v0);
		_mm_storeu_si128((__m128i *)(q+16), result.v1);
		prev = result.v2;
	}
#endif
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *(q - 1) + *p;
	}
}

#ifdef GENERATE_SSSE3
template void tuned_RestoreCylindricalLeft8<CODEFEATURE_SSSE3>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
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

#ifdef __SSSE3__
	for (; p <= pSrcEnd - 8; p += 8, q += 8)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount10Element<F, true, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}
#endif
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


template<int F>
static inline FORCEINLINE VECTOR2<__m128i> /* value0, nextprev */ tuned_RestoreLeft10Element(__m128i prev, __m128i s0)
{
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 2));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 4));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 8));
	s0 = _mm_add_epi16(s0, prev);
	s0 = _mm_and_si128(s0, _mm_set1_epi16(CSymbolBits<10>::maskval));
	return { s0, _mm_shuffle_epi8(s0, _mm_set2_epi8(15, 14)) };
}

template<int F>
static inline FORCEINLINE VECTOR3<__m128i> /* value0, value1, nextprev */ tuned_RestoreLeft10Element(__m128i prev, __m128i s0, __m128i s1)
{
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 2));
	s1 = _mm_add_epi16(s1, _mm_slli_si128(s1, 2));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 4));
	s1 = _mm_add_epi16(s1, _mm_slli_si128(s1, 4));
	s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 8));
	s1 = _mm_add_epi16(s1, _mm_slli_si128(s1, 8));
	s0 = _mm_add_epi16(s0, prev);
	s1 = _mm_add_epi16(s1, _mm_shuffle_epi8(s0, _mm_set2_epi8(15, 14)));
	s0 = _mm_and_si128(s0, _mm_set1_epi16(CSymbolBits<10>::maskval));
	s1 = _mm_and_si128(s1, _mm_set1_epi16(CSymbolBits<10>::maskval));
	return { s0, s1, _mm_shuffle_epi8(s1, _mm_set2_epi8(15, 14)) };
}

template<int F>
void tuned_RestoreCylindricalLeft10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi16(CSymbolBits<10>::midval);

#ifdef __SSSE3__
	for (; p <= pSrcEnd - 16; p += 16, q += 16)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		__m128i s1 = _mm_loadu_si128((const __m128i *)(p + 8));
		auto result = tuned_RestoreLeft10Element<F>(prev, s0, s1);
		_mm_storeu_si128((__m128i *)q, result.v0);
		_mm_storeu_si128((__m128i *)(q + 8), result.v1);
		prev = result.v2;
	}
#endif
	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*(q - 1) + *p) & CSymbolBits<10>::maskval;
	}
}

#ifdef GENERATE_SSSE3
template void tuned_RestoreCylindricalLeft10<CODEFEATURE_SSSE3>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
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

#ifdef __SSSE3__
	for (; p <= pSrcBegin + cbStride - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount8Element<F, true, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}
#endif
	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *p - *(p - 1);
		++pCountTable[0][*q];
	}

	prev = _mm_set1_epi8((char)0x80);
	topprev = _mm_set1_epi8((char)0x80);

#ifdef __SSSE3__
	for (; p <= pSrcEnd - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i top = _mm_loadu_si128((const __m128i *)(p - cbStride));
		__m128i residual = tuned_PredictWrongMedianAndCount8Element<F, true, 1, 0>(topprev, top, prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
		topprev = top;
	}
#endif
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

#ifdef __SSSE3__
	for (; p <= pSrcBegin + cbStride - 32; p += 32, q += 32)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		__m128i s1 = _mm_loadu_si128((const __m128i *)(p + 16));
		auto result = tuned_RestoreLeft8Element<F>(prev, s0, s1);
		_mm_storeu_si128((__m128i *)q, result.v0);
		_mm_storeu_si128((__m128i *)(q + 16), result.v1);
		prev = result.v2;
	}
#endif
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

#ifdef GENERATE_SSSE3
template void tuned_RestoreCylindricalWrongMedian8<CODEFEATURE_SSSE3>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
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

#ifdef __SSSE3__
	for (; p <= pSrcBegin + cbStride - 16; p += 16, q += 16)
	{
		__m128i value = _mm_loadu_si128((const __m128i *)p);
		__m128i residual = tuned_PredictLeftAndCount8Element<F, DoCount, 1, 0>(prev, value, pCountTable);
		_mm_storeu_si128((__m128i *)q, residual);
		prev = value;
	}
#endif
	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *p - *(p - 1);
		if (DoCount)
			++pCountTable[0][*q];
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		prev = _mm_setzero_si128();

#ifdef __SSSE3__
		for (; p <= pp + cbStride - 16; p += 16, q += 16)
		{
			__m128i value = _mm_sub_epi8(_mm_loadu_si128((const __m128i *)p), _mm_loadu_si128((const __m128i *)(p - cbStride)));
			__m128i residual = tuned_PredictLeftAndCount8Element<F, DoCount, 1, 0>(prev, value, pCountTable);
			_mm_storeu_si128((__m128i *)q, residual);
			prev = value;
		}
#endif
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

#ifdef __SSSE3__
	for (; p <= pSrcBegin + cbStride - 32; p += 32, q += 32)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		__m128i s1 = _mm_loadu_si128((const __m128i *)(p + 16));
		auto result = tuned_RestoreLeft8Element<F>(prev, s0, s1);
		_mm_storeu_si128((__m128i *)q, result.v0);
		_mm_storeu_si128((__m128i *)(q + 16), result.v1);
		prev = result.v2;
	}
#endif
	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *(q - 1) + *p;
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		prev = _mm_set1_epi8((char)0);

#ifdef __SSSE3__
		for (; p <= pp + cbStride - 32; p += 32, q += 32)
		{
			__m128i s0 = _mm_loadu_si128((const __m128i *)p);
			__m128i s1 = _mm_loadu_si128((const __m128i *)(p + 16));
			auto result = tuned_RestoreLeft8Element<F>(prev, s0, s1);
			_mm_storeu_si128((__m128i *)q, _mm_add_epi8(result.v0, _mm_loadu_si128((const __m128i *)(q - cbStride))));
			_mm_storeu_si128((__m128i *)(q + 16), _mm_add_epi8(result.v1, _mm_loadu_si128((const __m128i *)(q - cbStride + 16))));
			prev = result.v2;
		}
#endif
		for (; p < pp + cbStride; p++, q++)
		{
			*q = *p + (*(q - 1) + *(q - cbStride) - *(q - cbStride - 1));
		}
	}
}

#ifdef GENERATE_SSSE3
template void tuned_RestorePlanarGradient8<CODEFEATURE_SSSE3>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestorePlanarGradient8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif
