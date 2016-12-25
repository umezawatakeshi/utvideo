/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include <myintrin_x86x64.h>

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_AVX1)
#error
#endif

template<int F>
void tuned_RestoreLeft(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	auto p = pSrcBegin;
	auto q = pDst;

	__m128i prev = _mm_set1_epi8((char)0x80);

#ifdef __SSSE3__
	for (; p <= pSrcEnd - 32; p += 32, q += 32)
	{
		__m128i s0 = _mm_loadu_si128((const __m128i *)p);
		__m128i s1 = _mm_loadu_si128((const __m128i *)(p+16));

		s0 = _mm_add_epi8(s0, _mm_shuffle_epi8(s0, _mm_set_epi8(14, -1, 12, -1, 10, -1, 8, -1, 6, -1, 4, -1, 2, -1, 0, -1)));
		s1 = _mm_add_epi8(s1, _mm_shuffle_epi8(s1, _mm_set_epi8(14, -1, 12, -1, 10, -1, 8, -1, 6, -1, 4, -1, 2, -1, 0, -1)));
		s0 = _mm_add_epi8(s0, _mm_shuffle_epi8(s0, _mm_set_epi8(13, 13, -1, -1, 9, 9, -1, -1, 5, 5, -1, -1, 1, 1, -1, -1)));
		s1 = _mm_add_epi8(s1, _mm_shuffle_epi8(s1, _mm_set_epi8(13, 13, -1, -1, 9, 9, -1, -1, 5, 5, -1, -1, 1, 1, -1, -1)));
		s0 = _mm_add_epi8(s0, _mm_shuffle_epi8(s0, _mm_set_epi8(11, 11, 11, 11, -1, -1, -1, -1, 3, 3, 3, 3, -1, -1, -1, -1)));
		s1 = _mm_add_epi8(s1, _mm_shuffle_epi8(s1, _mm_set_epi8(11, 11, 11, 11, -1, -1, -1, -1, 3, 3, 3, 3, -1, -1, -1, -1)));
		s0 = _mm_add_epi8(s0, _mm_shuffle_epi8(s0, _mm_set_epi8(7, 7, 7, 7, 7, 7, 7, 7, -1, -1, -1, -1, -1, -1, -1, -1)));
		s1 = _mm_add_epi8(s1, _mm_shuffle_epi8(s1, _mm_set_epi8(7, 7, 7, 7, 7, 7, 7, 7, -1, -1, -1, -1, -1, -1, -1, -1)));
		s0 = _mm_add_epi8(s0, prev);
		s1 = _mm_add_epi8(s1, _mm_shuffle_epi8(s0, _mm_set1_epi8(15)));
		_mm_storeu_si128((__m128i *)q, s0);
		_mm_storeu_si128((__m128i *)(q+16), s1);
		prev = _mm_shuffle_epi8(s1, _mm_set1_epi8(15));
	}
#endif
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *(q - 1) + *p;
	}
}

#ifdef GENERATE_SSSE3
template void tuned_RestoreLeft<CODEFEATURE_SSSE3>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
#endif

#ifdef GENERATE_AVX1
template void tuned_RestoreLeft<CODEFEATURE_AVX1>(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
#endif
