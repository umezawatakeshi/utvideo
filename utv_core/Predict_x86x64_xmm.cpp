/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_AVX1)
#error
#endif

template<int F>
void tuned_RestoreLeft8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

	for (; p <= pSrcEnd - 16; p += 16, q += 16)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);

		s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 1));
		s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 2));
		s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 4));
		s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 8));
		s0 = _mm_add_epi8(s0, prev);
		_mm_storeu_si128((__m128i *)q, s0);
#ifdef __SSSE3__
		prev = _mm_shuffle_epi8(s0, _mm_set1_epi8(15));
#else
		prev = _mm_and_si128(s0, _mm_set1_epi32(0xFF000000));
		prev = _mm_shuffle_epi32(_mm_shufflehi_epi16(prev, _MM_SHUFFLE(3, 3, 3, 3)), _MM_SHUFFLE(3, 3, 3, 3));
		prev = _mm_or_si128(prev, _mm_srli_epi16(prev, 8));
#endif
	}

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *(q - 1) + *p;
	}
}

template<int F>
void tuned_RestoreLeft10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi16(CSymbolBits<10>::midval);

	for (; p <= pSrcEnd - 8; p += 8, q += 8)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		__m128i s1 = _mm_loadu_si128((const __m128i *)(p + 8));

		s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 2));
		s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 4));
		s0 = _mm_add_epi16(s0, _mm_slli_si128(s0, 8));
		s0 = _mm_add_epi16(s0, prev);
		s0 = _mm_and_si128(s0, _mm_set1_epi16(CSymbolBits<10>::maskval));
		_mm_storeu_si128((__m128i *)q, s0);
		prev = _mm_shuffle_epi32(_mm_shufflehi_epi16(s0, _MM_SHUFFLE(3, 3, 3, 3)), _MM_SHUFFLE(3, 3, 3, 3));
	}

	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*(q - 1) + *p) & CSymbolBits<10>::maskval;
	}
}

#ifdef GENERATE_SSE2
template void tuned_RestoreLeft8<CODEFEATURE_SSE2>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
template void tuned_RestoreLeft10<CODEFEATURE_SSE2>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
#endif

#ifdef GENERATE_SSSE3
template void tuned_RestoreLeft8<CODEFEATURE_SSSE3>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
template void tuned_RestoreLeft10<CODEFEATURE_SSSE3>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestoreLeft8<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
template void tuned_RestoreLeft10<CODEFEATURE_AVX1>(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
#endif
