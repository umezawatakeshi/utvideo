/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "SymPack_x86x64.h"

#if !defined(GENERATE_AVX2)
#error
#endif

template<int F>
static inline FORCEINLINE void PackForIntra(uint8_t*& q, uint8_t*& r, __m256i wa, __m256i wb)
{
	__m256i viszeroa = _mm256_cmpeq_epi64(wa, _mm256_setzero_si256());
	__m256i viszerob = _mm256_cmpeq_epi64(wb, _mm256_setzero_si256());

	int modesa, modesb;
	int bits0, bits1, bits2, bits3, bits4, bits5, bits6, bits7;

	__m256i za;
	__m256i zb;
	__m256i signsa = _mm256_cmpgt_epi8(_mm256_setzero_si256(), wa);
	__m256i signsb = _mm256_cmpgt_epi8(_mm256_setzero_si256(), wb);
	za = _mm256_xor_si256(wa, signsa);
	zb = _mm256_xor_si256(wb, signsb);
	za = _mm256_or_si256(za, _mm256_srli_epi64(za, 32));
	zb = _mm256_or_si256(zb, _mm256_srli_epi64(zb, 32));
	za = _mm256_or_si256(_mm256_or_si256(za, _mm256_set1_epi64x(1)), _mm256_srli_epi64(za, 16));
	zb = _mm256_or_si256(_mm256_or_si256(zb, _mm256_set1_epi64x(1)), _mm256_srli_epi64(zb, 16));
	za = _mm256_or_si256(za, _mm256_srli_epi64(za, 8));
	zb = _mm256_or_si256(zb, _mm256_srli_epi64(zb, 8));
	za = _mm256_and_si256(za, _mm256_set1_epi64x(0xff));
	zb = _mm256_and_si256(zb, _mm256_set1_epi64x(0xff));

	// Ç±Ç±Ç≈ BSR/LZCNT
	za = _mm256_castps_si256(_mm256_cvtepi32_ps(za));
	zb = _mm256_castps_si256(_mm256_cvtepi32_ps(zb));
	za = _mm256_srli_epi32(za, 23);
	zb = _mm256_srli_epi32(zb, 23);
	za = _mm256_sub_epi32(za, _mm256_set1_epi64x(0x7d));
	zb = _mm256_sub_epi32(zb, _mm256_set1_epi64x(0x7d));
	__m256i vbitsa = za;
	__m256i vbitsb = zb;
	__m256i vrembitsa = _mm256_sub_epi64(_mm256_set1_epi64x(8), vbitsa);
	__m256i vrembitsb = _mm256_sub_epi64(_mm256_set1_epi64x(8), vbitsb);
	vbitsa = _mm256_andnot_si256(viszeroa, vbitsa);
	vbitsb = _mm256_andnot_si256(viszerob, vbitsb);
	__m256i vmodea = _mm256_subs_epu8(vbitsa, _mm256_set1_epi64x(1));
	__m256i vmodeb = _mm256_subs_epu8(vbitsb, _mm256_set1_epi64x(1));

	bits0 = _mm_cvtsi128_si32(_mm256_castsi256_si128(vbitsa));
	bits1 = _mm_extract_epi32(_mm256_castsi256_si128(vbitsa), 2);
	__m128i vbitshigha = _mm256_extracti128_si256(vbitsa, 1);
	bits2 = _mm_cvtsi128_si32(vbitshigha);
	bits3 = _mm_extract_epi32(vbitshigha, 2);
	bits4 = _mm_cvtsi128_si32(_mm256_castsi256_si128(vbitsb));
	bits5 = _mm_extract_epi32(_mm256_castsi256_si128(vbitsb), 2);
	__m128i vbitshighb = _mm256_extracti128_si256(vbitsb, 1);
	bits6 = _mm_cvtsi128_si32(vbitshighb);
	bits7 = _mm_extract_epi32(vbitshighb, 2);

	vmodea = _mm256_permutevar8x32_epi32(vmodea, _mm256_castsi128_si256(_mm_set_epi32(6, 4, 2, 0))); // VPERMD
	vmodeb = _mm256_permutevar8x32_epi32(vmodeb, _mm256_castsi128_si256(_mm_set_epi32(6, 4, 2, 0))); // VPERMD
	__m128i vmode128a = _mm_shuffle_epi8(_mm256_castsi256_si128(vmodea), _mm_set1_epi32(0x0c080400));
	__m128i vmode128b = _mm_shuffle_epi8(_mm256_castsi256_si128(vmodeb), _mm_set1_epi32(0x0c080400));
	modesa = _pext_u32(_mm_cvtsi128_si32(vmode128a), 0x07070707);
	modesb = _pext_u32(_mm_cvtsi128_si32(vmode128b), 0x07070707);

	wa = _mm256_add_epi8(wa, _mm256_srlv_epi64(_mm256_set1_epi8((char)0x80), vrembitsa));
	wb = _mm256_add_epi8(wb, _mm256_srlv_epi64(_mm256_set1_epi8((char)0x80), vrembitsb));
	wa = _mm256_or_si256(_mm256_and_si256(wa, _mm256_set1_epi16(0x00ff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi16(0x00ff), wa), vrembitsa));
	wb = _mm256_or_si256(_mm256_and_si256(wb, _mm256_set1_epi16(0x00ff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi16(0x00ff), wb), vrembitsb));
	vrembitsa = _mm256_slli_epi64(vrembitsa, 1);
	vrembitsb = _mm256_slli_epi64(vrembitsb, 1);
	wa = _mm256_or_si256(_mm256_and_si256(wa, _mm256_set1_epi32(0x0000ffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi32(0x0000ffff), wa), vrembitsa));
	wb = _mm256_or_si256(_mm256_and_si256(wb, _mm256_set1_epi32(0x0000ffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi32(0x0000ffff), wb), vrembitsb));
	vrembitsa = _mm256_slli_epi64(vrembitsa, 1);
	vrembitsb = _mm256_slli_epi64(vrembitsb, 1);
	wa = _mm256_or_si256(_mm256_and_si256(wa, _mm256_set1_epi64x(0x00000000ffffffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi64x(0x00000000ffffffff), wa), vrembitsa));
	wb = _mm256_or_si256(_mm256_and_si256(wb, _mm256_set1_epi64x(0x00000000ffffffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi64x(0x00000000ffffffff), wb), vrembitsb));

	_mm_storel_epi64((__m128i*)q, _mm256_castsi256_si128(wa));
	q += bits0;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(_mm256_castsi256_si128(wa), 8));
	q += bits1;
	__m128i whigha = _mm256_extracti128_si256(wa, 1);
	_mm_storel_epi64((__m128i*)q, whigha);
	q += bits2;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(whigha, 8));
	q += bits3;

	_mm_storel_epi64((__m128i*)q, _mm256_castsi256_si128(wb));
	q += bits4;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(_mm256_castsi256_si128(wb), 8));
	q += bits5;
	__m128i whighb = _mm256_extracti128_si256(wb, 1);
	_mm_storel_epi64((__m128i*)q, whighb);
	q += bits6;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(whighb, 8));
	q += bits7;

	*(uint32_t *)r = (modesb << 12) | modesa;
	r += 3;
}

template<>
void tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride)
{
	constexpr int F = CODEFEATURE_AVX2;

	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 3);

	{
		__m256i prev = _mm256_set1_epi8((char)0x80);

		for (auto p = pSrcBegin; p != pSrcBegin + cbStride; p += 64)
		{
			__m256i value0 = _mm256_loadu_si256((const __m256i *)p);
			__m256i value1 = _mm256_loadu_si256((const __m256i *)(p + 32));
			__m256i residual0 = tuned_PredictLeft8Element<F>(prev, value0);
			__m256i residual1 = tuned_PredictLeft8Element<F>(value0, value1);
			prev = value1;

			PackForIntra<CODEFEATURE_AVX2>(q, r, residual0, residual1);
		}
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		__m256i prev = _mm256_setzero_si256();

		for (auto p = pp; p != pp + cbStride; p += 64)
		{
			__m256i value0 = _mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)p), _mm256_loadu_si256((const __m256i *)(p - cbStride)));
			__m256i value1 = _mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(p + 32)), _mm256_loadu_si256((const __m256i *)(p + 32 - cbStride)));
			__m256i residual0 = tuned_PredictLeft8Element<F>(prev, value0);
			__m256i residual1 = tuned_PredictLeft8Element<F>(value0, value1);
			prev = value1;

			PackForIntra<CODEFEATURE_AVX2>(q, r, residual0, residual1);
		}
	}

	*cbPacked = q - pPacked;

}


template<int F>
static inline FORCEINLINE __m256i UnpackForIntra(const uint8_t*& q, const uint8_t *& r, int& shift)
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

	shift += 12;
	if (shift == 24)
	{
		r += 3;
		shift = 0;
	}

	return w;
}

template<>
void tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbStride)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;

	{
		__m256i prev = _mm256_set1_epi8((char)0x80);

		for (auto p = pDstBegin; p != pDstBegin + cbStride; p += 32)
		{
			__m256i s0 = UnpackForIntra<CODEFEATURE_AVX2>(q, r, shift);

			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 1));
			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 2));
			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 4));
			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 8));
			s0 = _mm256_add_epi8(s0, _mm256_shuffle_epi8(_mm256_broadcastsi128_si256(_mm256_castsi256_si128(s0)), _mm256_set_epi8(
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			)));
			s0 = _mm256_add_epi8(s0, prev);
			_mm256_storeu_si256((__m256i *)p, s0);
			prev = _mm256_shuffle_epi8(_mm256_permute2x128_si256(s0, s0, 0x11), _mm256_set1_epi8(15));
		}
	}

	for (auto pp = pDstBegin + cbStride; pp != pDstEnd; pp += cbStride)
	{
		__m256i prev = _mm256_set1_epi8((char)0);

		for (auto p = pp; p != pp + cbStride; p += 32)
		{
			__m256i s0 = UnpackForIntra<CODEFEATURE_AVX2>(q, r, shift);

			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 1));
			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 2));
			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 4));
			s0 = _mm256_add_epi8(s0, _mm256_slli_si256(s0, 8));
			s0 = _mm256_add_epi8(s0, _mm256_shuffle_epi8(_mm256_broadcastsi128_si256(_mm256_castsi256_si128(s0)), _mm256_set_epi8(
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			)));
			s0 = _mm256_add_epi8(s0, prev);
			_mm256_storeu_si256((__m256i *)p, _mm256_add_epi8(s0, _mm256_loadu_si256((const __m256i *)(p - cbStride))));
			prev = _mm256_shuffle_epi8(_mm256_permute2x128_si256(s0, s0, 0x11), _mm256_set1_epi8(15));
		}
	}

}


template<int F>
static inline FORCEINLINE void VECTORCALL PackForDelta(uint8_t*& q, uint8_t*& r, __m256i wa, __m256i wb, __m256i ta, __m256i tb)
{
	int modesa, modesb;
	int bits0, bits1, bits2, bits3, bits4, bits5, bits6, bits7;

	auto getvbits = [](__m256i xa, __m256i xb)
	{
		__m256i za;
		__m256i zb;
		__m256i viszeroa = _mm256_cmpeq_epi64(xa, _mm256_setzero_si256());
		__m256i viszerob = _mm256_cmpeq_epi64(xb, _mm256_setzero_si256());
		__m256i viszero = _mm256_blend_epi16(viszeroa, viszerob, 0xcc);
		__m256i signsa = _mm256_cmpgt_epi8(_mm256_setzero_si256(), xa);
		__m256i signsb = _mm256_cmpgt_epi8(_mm256_setzero_si256(), xb);
		za = _mm256_xor_si256(xa, signsa);
		zb = _mm256_xor_si256(xb, signsb);
		za = _mm256_or_si256(za, _mm256_srli_epi64(za, 32));
		zb = _mm256_or_si256(zb, _mm256_slli_epi64(zb, 32));
		__m256i z = _mm256_blend_epi16(za, zb, 0xcc);
		z = _mm256_or_si256(_mm256_or_si256(z, _mm256_set1_epi32(1)), _mm256_srli_epi32(z, 16));
		z = _mm256_or_si256(z, _mm256_srli_epi32(z, 8));
		z = _mm256_and_si256(z, _mm256_set1_epi32(0xff));

		// Ç±Ç±Ç≈ BSR/LZCNT
		z = _mm256_castps_si256(_mm256_cvtepi32_ps(z));
		z = _mm256_srli_epi32(z, 23);
		z = _mm256_sub_epi32(z, _mm256_set1_epi32(0x7d));
		return _mm256_andnot_si256(viszero, z);
	};

	auto vbitsw = getvbits(wa, wb);
	auto vbitst = getvbits(ta, tb);
	__m256i vspatial = _mm256_cmpgt_epi32(vbitst, vbitsw);
	__m256i vspatiala = _mm256_shuffle_epi32(vspatial, _MM_SHUFFLE(2, 2, 0, 0));
	__m256i vspatialb = _mm256_shuffle_epi32(vspatial, _MM_SHUFFLE(3, 3, 1, 1));
	wa = _mm256_blendv_epi8(ta, wa, vspatiala);
	wb = _mm256_blendv_epi8(tb, wb, vspatialb);
	__m256i vbits = _mm256_min_epi32(vbitsw, vbitst);
	__m256i vrembits = _mm256_sub_epi32(_mm256_set1_epi32(8), vbits);
	__m256i vrembitsa = _mm256_and_si256(vrembits, _mm256_set1_epi64x(0x00000000ffffffff));
	__m256i vrembitsb = _mm256_srli_epi64(vrembits, 32);
	__m256i vmode = _mm256_subs_epu8(vbits, _mm256_set1_epi32(1));
	vmode = _mm256_or_si256(vmode, _mm256_andnot_si256(vspatial, _mm256_set1_epi32(8)));

	bits0 = _mm_cvtsi128_si32(_mm256_castsi256_si128(vbits));
	bits1 = _mm_extract_epi32(_mm256_castsi256_si128(vbits), 2);
	bits4 = _mm_extract_epi32(_mm256_castsi256_si128(vbits), 1);
	bits5 = _mm_extract_epi32(_mm256_castsi256_si128(vbits), 3);
	__m128i vbitshigh = _mm256_extracti128_si256(vbits, 1);
	bits2 = _mm_cvtsi128_si32(vbitshigh);
	bits3 = _mm_extract_epi32(vbitshigh, 2);
	bits6 = _mm_extract_epi32(vbitshigh, 1);
	bits7 = _mm_extract_epi32(vbitshigh, 3);

	__m256i vmodea = _mm256_permutevar8x32_epi32(vmode, _mm256_castsi128_si256(_mm_set_epi32(6, 4, 2, 0))); // VPERMD
	__m256i vmodeb = _mm256_permutevar8x32_epi32(vmode, _mm256_castsi128_si256(_mm_set_epi32(7, 5, 3, 1))); // VPERMD
	__m128i vmode128a = _mm_shuffle_epi8(_mm256_castsi256_si128(vmodea), _mm_set1_epi32(0x0c080400));
	__m128i vmode128b = _mm_shuffle_epi8(_mm256_castsi256_si128(vmodeb), _mm_set1_epi32(0x0c080400));
	modesa = _pext_u32(_mm_cvtsi128_si32(vmode128a), 0x0f0f0f0f);
	modesb = _pext_u32(_mm_cvtsi128_si32(vmode128b), 0x0f0f0f0f);

	wa = _mm256_add_epi8(wa, _mm256_srlv_epi64(_mm256_set1_epi8((char)0x80), vrembitsa));
	wb = _mm256_add_epi8(wb, _mm256_srlv_epi64(_mm256_set1_epi8((char)0x80), vrembitsb));
	wa = _mm256_or_si256(_mm256_and_si256(wa, _mm256_set1_epi16(0x00ff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi16(0x00ff), wa), vrembitsa));
	wb = _mm256_or_si256(_mm256_and_si256(wb, _mm256_set1_epi16(0x00ff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi16(0x00ff), wb), vrembitsb));
	vrembitsa = _mm256_slli_epi64(vrembitsa, 1);
	vrembitsb = _mm256_slli_epi64(vrembitsb, 1);
	wa = _mm256_or_si256(_mm256_and_si256(wa, _mm256_set1_epi32(0x0000ffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi32(0x0000ffff), wa), vrembitsa));
	wb = _mm256_or_si256(_mm256_and_si256(wb, _mm256_set1_epi32(0x0000ffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi32(0x0000ffff), wb), vrembitsb));
	vrembitsa = _mm256_slli_epi64(vrembitsa, 1);
	vrembitsb = _mm256_slli_epi64(vrembitsb, 1);
	wa = _mm256_or_si256(_mm256_and_si256(wa, _mm256_set1_epi64x(0x00000000ffffffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi64x(0x00000000ffffffff), wa), vrembitsa));
	wb = _mm256_or_si256(_mm256_and_si256(wb, _mm256_set1_epi64x(0x00000000ffffffff)), _mm256_srlv_epi64(_mm256_andnot_si256(_mm256_set1_epi64x(0x00000000ffffffff), wb), vrembitsb));

	_mm_storel_epi64((__m128i*)q, _mm256_castsi256_si128(wa));
	q += bits0;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(_mm256_castsi256_si128(wa), 8));
	q += bits1;
	__m128i whigha = _mm256_extracti128_si256(wa, 1);
	_mm_storel_epi64((__m128i*)q, whigha);
	q += bits2;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(whigha, 8));
	q += bits3;

	_mm_storel_epi64((__m128i*)q, _mm256_castsi256_si128(wb));
	q += bits4;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(_mm256_castsi256_si128(wb), 8));
	q += bits5;
	__m128i whighb = _mm256_extracti128_si256(wb, 1);
	_mm_storel_epi64((__m128i*)q, whighb);
	q += bits6;
	_mm_storel_epi64((__m128i*)q, _mm_srli_si128(whighb, 8));
	q += bits7;

	*(uint32_t *)r = (modesb << 16) | modesa;
	r += 4;
}

template<>
void tuned_Pack8SymWithDiff8<CODEFEATURE_AVX2>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const uint8_t *pPrevBegin, size_t cbStride)
{
	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 4);

	{
		__m256i prev = _mm256_set1_epi8((char)0x80);

		for (auto p = pSrcBegin, t = pPrevBegin; p != pSrcBegin + cbStride; p += 64, t += 64)
		{
			__m256i value0 = _mm256_loadu_si256((const __m256i *)p);
			__m256i value1 = _mm256_loadu_si256((const __m256i *)(p + 32));
			__m256i left0 = _mm256_alignr_epi8(value0, _mm256_permute2x128_si256(value0, prev, 0x03), 15);
			__m256i left1 = _mm256_alignr_epi8(value1, _mm256_permute2x128_si256(value1, value0, 0x03), 15);

			__m256i error0 = _mm256_sub_epi8(value0, left0);
			__m256i error1 = _mm256_sub_epi8(value1, left1);
			__m256i t0 = _mm256_sub_epi8(value0, _mm256_loadu_si256((const __m256i *)t));
			__m256i t1 = _mm256_sub_epi8(value1, _mm256_loadu_si256((const __m256i *)(t + 32)));
			prev = value1;

			PackForDelta<CODEFEATURE_AVX2>(q, r, error0, error1, t0, t1);
		}
	}

	for (auto pp = pSrcBegin + cbStride, tt = pPrevBegin + cbStride; pp != pSrcEnd; pp += cbStride, tt += cbStride)
	{
		__m256i prev = _mm256_setzero_si256();

		for (auto p = pp, t = tt; p != pp + cbStride; p += 64, t += 64)
		{
			__m256i value0 = _mm256_loadu_si256((const __m256i *)p);
			__m256i value1 = _mm256_loadu_si256((const __m256i *)(p + 32));
			__m256i top0 = _mm256_loadu_si256((const __m256i *)(p - cbStride));
			__m256i top1 = _mm256_loadu_si256((const __m256i *)(p + 32 - cbStride));
			__m256i tmp0 = _mm256_sub_epi8(value0, top0);
			__m256i tmp1 = _mm256_sub_epi8(value1, top1);
			__m256i left0 = _mm256_alignr_epi8(tmp0, _mm256_permute2x128_si256(tmp0, prev, 0x03), 15);
			__m256i left1 = _mm256_alignr_epi8(tmp1, _mm256_permute2x128_si256(tmp1, tmp0, 0x03), 15);
			__m256i error0 = _mm256_sub_epi8(tmp0, left0);
			__m256i error1 = _mm256_sub_epi8(tmp1, left1);
			__m256i t0 = _mm256_sub_epi8(value0, _mm256_loadu_si256((const __m256i *)t));
			__m256i t1 = _mm256_sub_epi8(value1, _mm256_loadu_si256((const __m256i *)(t + 32)));
			prev = tmp1;

			PackForDelta<CODEFEATURE_AVX2>(q, r, error0, error1, t0, t1);
		}
	}

	*cbPacked = q - pPacked;

}


template<int F>
static inline FORCEINLINE typename std::pair<__m256i, __m256i> UnpackForDelta(const uint8_t*& q, const uint8_t *& r, int& shift)
{
	__m256i w;
	int modes = ((*(uint32_t *)r) >> shift);
	__m256i rmask = _mm256_cmpeq_epi64(_mm256_and_si256(_mm256_broadcastq_epi64(_mm_cvtsi32_si128(*(const uint32_t *)r >> shift)), _mm256_set_epi64x(0x8000, 0x0800, 0x0080, 0x0008)), _mm256_set_epi64x(0x8000, 0x0800, 0x0080, 0x0008));
	int bits0 = modes & 7;
	if (bits0 != 0)
		bits0++;
	int bits1 = (modes >> 4) & 7;
	if (bits1 != 0)
		bits1++;
	int bits2 = (modes >> 8) & 7;
	if (bits2 != 0)
		bits2++;
	int bits3 = (modes >> 12) & 7;
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

	shift += 16;
	if (shift == 32)
	{
		r += 4;
		shift = 0;
	}

	return { w, rmask };
}

template<>
void tuned_Unpack8SymWithDiff8<CODEFEATURE_AVX2>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, const uint8_t *pPrevBegin, size_t cbStride)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;

	{
		__m256i prev = _mm256_set1_epi8((char)0x80);

		auto t = pPrevBegin;
		for (auto p = pDstBegin; p != pDstBegin + cbStride; p += 32, t += 32)
		{
			auto w0 = UnpackForDelta<CODEFEATURE_AVX2>(q, r, shift);
			__m256i s0 = w0.first;
			__m256i m0 = w0.second;

			__m256i t0 = _mm256_add_epi8(s0, _mm256_loadu_si256((const __m256i*)t));
			__m256i t0masked = _mm256_and_si256(t0, m0);
			__m256i atmp = _mm256_permute2x128_si256(t0masked, prev, 0x03);
			__m256i a0 = _mm256_alignr_epi8(t0masked, atmp, 15);
			s0 = _mm256_andnot_si256(m0, _mm256_add_epi8(s0, a0));
			s0 = _mm256_add_epi8(s0, _mm256_slli_epi64(s0, 8));
			s0 = _mm256_add_epi8(s0, _mm256_slli_epi64(s0, 16));
			s0 = _mm256_add_epi8(s0, _mm256_slli_epi64(s0, 32));
			s0 = _mm256_add_epi8(s0, _mm256_shuffle_epi8(_mm256_andnot_si256(m0, s0), _mm256_set16_epi8(7, 7, 7, 7, 7, 7, 7, 7, -1, -1, -1, -1, -1, -1, -1, -1)));
			__m256i mask16 = _mm256_or_si256(m0, _mm256_permute4x64_epi64(m0, _MM_SHUFFLE(3, 3, 2, 1)));
			s0 = _mm256_add_epi8(s0, _mm256_shuffle_epi8(_mm256_broadcastsi128_si256(_mm256_castsi256_si128(_mm256_andnot_si256(mask16, s0))), _mm256_set_epi8(
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			)));
			s0 = _mm256_blendv_epi8(s0, t0, m0);
			_mm256_storeu_si256((__m256i *)p, s0);
			prev = s0;
		}
	}

	auto tt = pPrevBegin + cbStride;
	for (auto pp = pDstBegin + cbStride; pp != pDstEnd; pp += cbStride, tt += cbStride)
	{
		__m256i prev = _mm256_set1_epi8((char)0);

		auto t = tt;
		for (auto p = pp; p != pp + cbStride; p += 32, t += 32)
		{
			auto w0 = UnpackForDelta<CODEFEATURE_AVX2>(q, r, shift);
			__m256i s0 = w0.first;
			__m256i m0 = w0.second;

			__m256i top = _mm256_loadu_si256((const __m256i*)(p - cbStride));
			__m256i t0 = _mm256_add_epi8(s0, _mm256_loadu_si256((const __m256i*)t));
			__m256i t0masked = _mm256_and_si256(_mm256_sub_epi8(t0, top), m0);
			__m256i atmp = _mm256_permute2x128_si256(t0masked, prev, 0x03);
			__m256i a0 = _mm256_alignr_epi8(t0masked, atmp, 15);
			s0 = _mm256_andnot_si256(m0, _mm256_add_epi8(s0, a0));
			s0 = _mm256_add_epi8(s0, _mm256_slli_epi64(s0, 8));
			s0 = _mm256_add_epi8(s0, _mm256_slli_epi64(s0, 16));
			s0 = _mm256_add_epi8(s0, _mm256_slli_epi64(s0, 32));
			s0 = _mm256_add_epi8(s0, _mm256_shuffle_epi8(_mm256_andnot_si256(m0, s0), _mm256_set16_epi8(7, 7, 7, 7, 7, 7, 7, 7, -1, -1, -1, -1, -1, -1, -1, -1)));
			__m256i mask16 = _mm256_or_si256(m0, _mm256_permute4x64_epi64(m0, _MM_SHUFFLE(3, 3, 2, 1)));
			s0 = _mm256_add_epi8(s0, _mm256_shuffle_epi8(_mm256_broadcastsi128_si256(_mm256_castsi256_si128(_mm256_andnot_si256(mask16, s0))), _mm256_set_epi8(
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			)));
			s0 = _mm256_add_epi8(s0, top);
			s0 = _mm256_blendv_epi8(s0, t0, m0);
			_mm256_storeu_si256((__m256i *)p, s0);
			prev = _mm256_sub_epi8(s0, top);
		}
	}

}
