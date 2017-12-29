/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2)
#error
#endif

template<int F>
void tuned_Pack8Sym8(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 3);

	for (auto p = pSrcBegin; p != pSrcEnd; p += 16)
	{
		__m128i w = _mm_loadu_si128((const __m128i *)p);
		__m128i visnotzero = _mm_cmpeq_epi64(_mm_cmpeq_epi64(w, _mm_setzero_si128()), _mm_setzero_si128());

		int mode0, mode1;
		int bits0, bits1;

		__m128i z;
		__m128i signs = _mm_cmpgt_epi8(_mm_setzero_si128(), w);
		int isnotzero0 = _mm_cvtsi128_si32(visnotzero);
		int isnotzero1 = _mm_extract_epi32(visnotzero, 2);
		z = _mm_xor_si128(w, signs);
		z = _mm_or_si128(z, _mm_srli_epi64(z, 32));
		z = _mm_or_si128(z, _mm_srli_epi64(z, 16));
		z = _mm_or_si128(z, _mm_srli_epi64(z, 8));
		z = _mm_and_si128(z, _mm_set1_epi64x(0xff));
		z = _mm_or_si128(z, _mm_set1_epi64x(1));
#if defined(_MSC_VER)
		_BitScanReverse((unsigned long *)&mode0, _mm_cvtsi128_si32(z));
		_BitScanReverse((unsigned long *)&mode1, _mm_extract_epi32(z, 2));
#elif defined(__GNUC__)
		mode0 = 31 - __builtin_clz(_mm_cvtsi128_si32(z));
		mode1 = 31 - __builtin_clz(_mm_extract_epi32(z, 2));
#else
#error
#endif
		bits0 = mode0 + 2;
		int rembits0 = 6 - mode0;
		mode0++;
		bits1 = mode1 + 2;
		int rembits1 = 6 - mode1;
		mode1++;

		bits0 &= isnotzero0;
		mode0 &= isnotzero0;
		bits1 &= isnotzero1;
		mode1 &= isnotzero1;
		__m128i vrembits0 = _mm_cvtsi32_si128(rembits0);
		__m128i vrembits1 = _mm_cvtsi32_si128(rembits1);
		__m128i w0 = _mm_add_epi8(w, _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits0));
		__m128i w1 = _mm_add_epi8(_mm_srli_si128(w, 8), _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits1));
		w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w0), vrembits0));
		w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w1), vrembits1));
		vrembits0 = _mm_slli_epi64(vrembits0, 1);
		vrembits1 = _mm_slli_epi64(vrembits1, 1);
		w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w0), vrembits0));
		w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w1), vrembits1));
		vrembits0 = _mm_slli_epi64(vrembits0, 1);
		vrembits1 = _mm_slli_epi64(vrembits1, 1);
		w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w0), vrembits0));
		w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w1), vrembits1));
		_mm_storel_epi64((__m128i*)q, w0);
		_mm_storel_epi64((__m128i*)(q + bits0), w1);

		q += bits0 + bits1;
		*(uint32_t *)r |= (((mode1 << 3) | mode0) << shift);
		shift += 6;
		if (shift == 24)
		{
			r += 3;
			shift = 0;
		}
	}

	*cbPacked = q - pPacked;
}

#ifdef GENERATE_SSE41
template void tuned_Pack8Sym8<CODEFEATURE_SSE41>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
#endif

#ifdef GENERATE_AVX1
template void tuned_Pack8Sym8<CODEFEATURE_AVX1>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
#endif

#ifdef GENERATE_AVX2
template<>
void tuned_Pack8Sym8<CODEFEATURE_AVX2>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 3);

	for (auto p = pSrcBegin; p != pSrcEnd; p += 32)
	{
		__m256i w = _mm256_loadu_si256((const __m256i *)p);
		__m256i visnotzero = _mm256_cmpeq_epi64(_mm256_cmpeq_epi64(w, _mm256_setzero_si256()), _mm256_setzero_si256());

		int modes;
		int bits0, bits1, bits2, bits3;

		__m256i z;
		__m256i signs = _mm256_cmpgt_epi8(_mm256_setzero_si256(), w);
		z = _mm256_xor_si256(w, signs);
		z = _mm256_or_si256(z, _mm256_srli_epi64(z, 32));
		z = _mm256_or_si256(z, _mm256_srli_epi64(z, 16));
		z = _mm256_or_si256(z, _mm256_srli_epi64(z, 8));
		z = _mm256_and_si256(z, _mm256_set1_epi64x(0xff));

		// Ç±Ç±Ç≈ BSR/LZCNT
		z = _mm256_or_si256(z, _mm256_set1_epi64x(1));
		z = _mm256_castps_si256(_mm256_cvtepi32_ps(z));
		z = _mm256_srli_epi32(z, 23);
		z = _mm256_sub_epi32(z, _mm256_set1_epi64x(0x7d));
		__m256i vbits = z;
		__m256i vrembits = _mm256_sub_epi64(_mm256_set1_epi64x(8), vbits);
		__m256i vmode = _mm256_sub_epi64(vbits, _mm256_set1_epi64x(1));
		vbits = _mm256_and_si256(vbits, visnotzero);
		vmode = _mm256_and_si256(vmode, visnotzero);

		bits0 = _mm_cvtsi128_si32(_mm256_castsi256_si128(vbits));
		bits1 = _mm_extract_epi32(_mm256_castsi256_si128(vbits), 2);
		__m128i vbitshigh = _mm256_extracti128_si256(vbits, 1);
		bits2 = _mm_cvtsi128_si32(vbitshigh);
		bits3 = _mm_extract_epi32(vbitshigh, 2);

		vmode = _mm256_permutevar8x32_epi32(vmode, _mm256_castsi128_si256(_mm_set_epi32(6, 4, 2, 0))); // VPERMD
		__m128i vmode128 = _mm_shuffle_epi8(_mm256_castsi256_si128(vmode), _mm_set1_epi32(0x0c080400));
		modes = _pext_u32(_mm_cvtsi128_si32(vmode128), 0x07070707);

		w = _mm256_add_epi8(w, _mm256_srlv_epi64(_mm256_set1_epi8((char)0x80), vrembits));
		w = _mm256_or_si256(_mm256_and_si256(w, _mm256_set1_epi16(0x00ff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi16(0x00ff), w), vrembits));
		vrembits = _mm256_slli_epi64(vrembits, 1);
		w = _mm256_or_si256(_mm256_and_si256(w, _mm256_set1_epi32(0x0000ffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi32(0x0000ffff), w), vrembits));
		vrembits = _mm256_slli_epi64(vrembits, 1);
		w = _mm256_or_si256(_mm256_and_si256(w, _mm256_set1_epi64x(0x00000000ffffffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi64x(0x00000000ffffffff), w), vrembits));

		_mm_storel_epi64((__m128i*)q, _mm256_castsi256_si128(w));
		q += bits0;
		_mm_storel_epi64((__m128i*)q, _mm_srli_si128(_mm256_castsi256_si128(w), 8));
		q += bits1;
		__m128i whigh = _mm256_extracti128_si256(w, 1);
		_mm_storel_epi64((__m128i*)q, whigh);
		q += bits2;
		_mm_storel_epi64((__m128i*)q, _mm_srli_si128(whigh, 8));
		q += bits3;

		*(uint32_t *)r |= modes << shift;
		shift += 12;
		if (shift == 24)
		{
			r += 3;
			shift = 0;
		}
	}

	*cbPacked = q - pPacked;
}
#endif


template<int F>
void tuned_Unpack8Sym8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;

	for (auto p = pDstBegin; p != pDstEnd; p += 8)
	{
		__m128i w;
		int mode = ((*(const uint32_t *)r) >> shift) & 7;
		int bits;
		if (mode == 0)
		{
			w = _mm_setzero_si128();
			bits = 0;
		}
		else
		{
			__m128i z, mask;

			w = _mm_loadl_epi64((const __m128i *)q);
			bits = mode + 1;
			int rembits = 8 - bits;
			__m128i vrembits = _mm_cvtsi32_si128(rembits);
			__m128i vbits = _mm_cvtsi32_si128(bits);

			__m128i vrembitsn = _mm_slli_epi64(vrembits, 2);
			z = _mm_sll_epi64(w, vrembitsn);
			w = _mm_blend_epi16(w, z, 0xcc);
			vrembitsn = _mm_srli_epi64(vrembitsn, 1);
			z = _mm_sll_epi64(w, vrembitsn);
			w = _mm_blend_epi16(w, z, 0xaa);
			z = _mm_sll_epi64(w, vrembits);
			w = _mm_blendv_epi8(w, z, _mm_set1_epi16((short)0xff00));
			mask = _mm_sub_epi64(_mm_sll_epi64(_mm_set1_epi8(1), vbits), _mm_set1_epi8(1));
			w = _mm_and_si128(w, mask);
			__m128i offset = _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits);
			w = _mm_sub_epi8(w, offset);
		}

		q += bits;
		_mm_storel_epi64((__m128i*)p, w);

		shift += 3;
		if (shift == 24)
		{
			r += 3;
			shift = 0;
		}
	}
}

#ifdef GENERATE_SSE41
template void tuned_Unpack8Sym8<CODEFEATURE_SSE41>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl);
#endif

#ifdef GENERATE_AVX1
template void tuned_Unpack8Sym8<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl);
#endif

#ifdef GENERATE_AVX2
template<>
void tuned_Unpack8Sym8<CODEFEATURE_AVX2>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;

	for (auto p = pDstBegin; p != pDstEnd; p += 32)
	{
		__m256i w;
		int modes = ((*(uint32_t *)r) >> shift);
		int bits0 = modes & 7;
		if (bits0 != 0)
			bits0++;
		int bits1 = (modes >> 3) & 7;
		if (bits1 != 0)
			bits1++;
		int bits2 = (modes >> 6) & 7;
		if (bits2 != 0)
			bits2++;
		int bits3 = (modes >> 9) & 7;
		if (bits3 != 0)
			bits3++;

		__m128i w0, w1, vbits0, vbits1;
		__m256i z, mask;
		__m256i vbits;

		w0 = _mm_loadl_epi64((__m128i *)q);
		q += bits0;
		w0 = _mm_unpacklo_epi64(w0, _mm_loadl_epi64((__m128i *)q));
		q += bits1;
		w1 = _mm_loadl_epi64((__m128i *)q);
		q += bits2;
		w1 = _mm_unpacklo_epi64(w1, _mm_loadl_epi64((__m128i *)q));
		q += bits3;
		w = _mm256_inserti128_si256(_mm256_castsi128_si256(w0), w1, 1);

		vbits0 = _mm_cvtsi32_si128(bits0);
		vbits0 = _mm_insert_epi32(vbits0, bits1, 2);
		vbits1 = _mm_cvtsi32_si128(bits2);
		vbits1 = _mm_insert_epi32(vbits1, bits3, 2);
		vbits = _mm256_inserti128_si256(_mm256_castsi128_si256(vbits0), vbits1, 1);

		__m256i vrembits = _mm256_sub_epi64(_mm256_set1_epi64x(8), vbits);

		__m256i vrembitsn = _mm256_slli_epi64(vrembits, 2);
		z = _mm256_sllv_epi64(w, vrembitsn);
		w = _mm256_blend_epi16(w, z, 0xcc);
		vrembitsn = _mm256_srli_epi64(vrembitsn, 1);
		z = _mm256_sllv_epi64(w, vrembitsn);
		w = _mm256_blend_epi16(w, z, 0xaa);
		z = _mm256_sllv_epi64(w, vrembits);
		w = _mm256_blendv_epi8(w, z, _mm256_set1_epi16((short)0xff00));
		mask = _mm256_sub_epi64(_mm256_sllv_epi64(_mm256_set1_epi8(1), vbits), _mm256_set1_epi8(1));
		w = _mm256_and_si256(w, mask);
		__m256i offset = _mm256_and_si256(_mm256_srlv_epi64(_mm256_set1_epi8((char)0x80), vrembits), mask);
		w = _mm256_sub_epi8(w, offset);

		_mm256_storeu_si256((__m256i*)p, w);

		shift += 12;
		if (shift == 24)
		{
			r += 3;
			shift = 0;
		}
	}
}
#endif
