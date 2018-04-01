/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2)
#error
#endif

template<int F>
static inline FORCEINLINE void packer(uint8_t*& q, uint8_t*& r, int& shift, __m128i wa, __m128i wb)
{
	__m128i viszeroa = _mm_cmpeq_epi64(wa, _mm_setzero_si128());
	__m128i viszerob = _mm_cmpeq_epi64(wb, _mm_setzero_si128());
	__m128i viszero = _mm_blend_epi16(viszeroa, viszerob, 0xcc);

	int bits0, bits1, bits2, bits3;

	__m128i za, zb;
	__m128i signsa = _mm_cmpgt_epi8(_mm_setzero_si128(), wa);
	__m128i signsb = _mm_cmpgt_epi8(_mm_setzero_si128(), wb);
	za = _mm_xor_si128(wa, signsa);
	zb = _mm_xor_si128(wb, signsb);
	za = _mm_or_si128(za, _mm_srli_epi64(za, 32));
	zb = _mm_or_si128(zb, _mm_slli_epi64(zb, 32));
	__m128i z = _mm_blend_epi16(za, zb, 0xcc);
	z = _mm_or_si128(z, _mm_srli_epi32(z, 16));
	z = _mm_or_si128(z, _mm_srli_epi32(z, 8));
	z = _mm_and_si128(z, _mm_set1_epi32(0xff));
	z = _mm_or_si128(z, _mm_set1_epi32(1));


	z = _mm_castps_si128(_mm_cvtepi32_ps(z));
	z = _mm_srli_epi32(z, 23);
	z = _mm_sub_epi32(z, _mm_set1_epi32(0x7d));
	__m128i vbits = z;
	__m128i vrembits = _mm_sub_epi32(_mm_set1_epi32(8), vbits);
	vbits = _mm_andnot_si128(viszero, vbits);
	__m128i vmode = _mm_add_epi32(vbits, _mm_cmpgt_epi32(vbits, _mm_setzero_si128()));

	bits0 = _mm_cvtsi128_si32(vbits);
	bits1 = _mm_extract_epi32(vbits, 2);
	bits2 = _mm_extract_epi32(vbits, 1);
	bits3 = _mm_extract_epi32(vbits, 3);

	__m128i vrembitsa = _mm_and_si128(vrembits, _mm_set1_epi64x(0xffffffff));
	__m128i vrembits0 = vrembitsa;
	__m128i vrembits1 = _mm_srli_si128(vrembitsa, 8);
	__m128i vrembitsb = _mm_srli_epi64(vrembits, 32);
	__m128i vrembits2 = vrembitsb;
	__m128i vrembits3 = _mm_srli_si128(vrembitsb, 8);
	__m128i w0 = _mm_add_epi8(wa, _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits0));
	__m128i w1 = _mm_add_epi8(_mm_srli_si128(wa, 8), _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits1));
	__m128i w2 = _mm_add_epi8(wb, _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits2));
	__m128i w3 = _mm_add_epi8(_mm_srli_si128(wb, 8), _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits3));
	w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w0), vrembits0));
	w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w1), vrembits1));
	w2 = _mm_or_si128(_mm_and_si128(w2, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w2), vrembits2));
	w3 = _mm_or_si128(_mm_and_si128(w3, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w3), vrembits3));
	vrembits0 = _mm_slli_epi64(vrembits0, 1);
	vrembits1 = _mm_slli_epi64(vrembits1, 1);
	vrembits2 = _mm_slli_epi64(vrembits2, 1);
	vrembits3 = _mm_slli_epi64(vrembits3, 1);
	w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w0), vrembits0));
	w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w1), vrembits1));
	w2 = _mm_or_si128(_mm_and_si128(w2, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w2), vrembits2));
	w3 = _mm_or_si128(_mm_and_si128(w3, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w3), vrembits3));
	vrembits0 = _mm_slli_epi64(vrembits0, 1);
	vrembits1 = _mm_slli_epi64(vrembits1, 1);
	vrembits2 = _mm_slli_epi64(vrembits2, 1);
	vrembits3 = _mm_slli_epi64(vrembits3, 1);
	w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w0), vrembits0));
	w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w1), vrembits1));
	w2 = _mm_or_si128(_mm_and_si128(w2, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w2), vrembits2));
	w3 = _mm_or_si128(_mm_and_si128(w3, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w3), vrembits3));
	_mm_storel_epi64((__m128i*)q, w0);
	q += bits0;
	_mm_storel_epi64((__m128i*)q, w1);
	q += bits1;
	_mm_storel_epi64((__m128i*)q, w2);
	q += bits2;
	_mm_storel_epi64((__m128i*)q, w3);
	q += bits3;

	vmode = _mm_shuffle_epi8(vmode, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 13, 12, 9, 8, 5, 4, 1, 0));
#ifdef __x86_64__
	uint64_t modes = _mm_cvtsi128_si64(vmode);
	modes |= modes >> (32 - 3);
#else
	vmode = _mm_or_si128(vmode, _mm_srli_epi64(vmode, 32 - 3));
	uint32_t modes = _mm_cvtsi128_si32(vmode);
#endif
	modes |= modes >> (16 - 6);

	*(uint32_t *)r |= (((uint32_t)(uint16_t)modes) << shift);
	shift += 12;
	if (shift == 24)
	{
		r += 3;
		shift = 0;
	}
}

template<int F>
void tuned_Pack8SymAfterPredictPlanarGradient8(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 3);

	{
		__m128i prev = _mm_set1_epi8((char)0x80);

		for (auto p = pSrcBegin; p != pSrcBegin + cbStride; p += 32)
		{
			__m128i value0 = _mm_loadu_si128((const __m128i *)p);
			__m128i value1 = _mm_loadu_si128((const __m128i *)(p + 16));
			__m128i left0 = _mm_alignr_epi8(value0, prev, 15);
			__m128i left1 = _mm_alignr_epi8(value1, value0, 15);

			__m128i error0 = _mm_sub_epi8(value0, left0);
			__m128i error1 = _mm_sub_epi8(value1, left1);
			prev = value1;
			packer<F>(q, r, shift, error0, error1);
		}
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		__m128i prev = _mm_set1_epi8((char)0x80);
		__m128i topprev = _mm_set1_epi8((char)0x80);

		for (auto p = pp; p != pp + cbStride; p += 32)
		{
			__m128i value0 = _mm_loadu_si128((const __m128i *)p);
			__m128i value1 = _mm_loadu_si128((const __m128i *)(p + 16));
			__m128i top0 = _mm_loadu_si128((const __m128i *)(p - cbStride));
			__m128i top1 = _mm_loadu_si128((const __m128i *)(p + 16 - cbStride));
			__m128i left0 = _mm_alignr_epi8(value0, prev, 15);
			__m128i left1 = _mm_alignr_epi8(value1, value0, 15);
			__m128i topleft0 = _mm_alignr_epi8(top0, topprev, 15);
			__m128i topleft1 = _mm_alignr_epi8(top1, top0, 15);

			__m128i error0 = _mm_sub_epi8(_mm_add_epi8(value0, topleft0), _mm_add_epi8(left0, top0));
			__m128i error1 = _mm_sub_epi8(_mm_add_epi8(value1, topleft1), _mm_add_epi8(left1, top1));
			prev = value1;
			topprev = top1;
			packer<F>(q, r, shift, error0, error1);
		}
	}

	*cbPacked = q - pPacked;
}

#ifdef GENERATE_SSE41
template void tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
#endif

#ifdef GENERATE_AVX2
template<int F>
static inline FORCEINLINE void packer(uint8_t*& q, uint8_t*& r, __m256i wa, __m256i wb)
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
	za = _mm256_or_si256(za, _mm256_srli_epi64(za, 16));
	zb = _mm256_or_si256(zb, _mm256_srli_epi64(zb, 16));
	za = _mm256_or_si256(za, _mm256_srli_epi64(za, 8));
	zb = _mm256_or_si256(zb, _mm256_srli_epi64(zb, 8));
	za = _mm256_and_si256(za, _mm256_set1_epi64x(0xff));
	zb = _mm256_and_si256(zb, _mm256_set1_epi64x(0xff));

	// Ç±Ç±Ç≈ BSR/LZCNT
	za = _mm256_or_si256(za, _mm256_set1_epi64x(1));
	zb = _mm256_or_si256(zb, _mm256_set1_epi64x(1));
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
	__m256i vmodea = _mm256_add_epi64(vbitsa, _mm256_cmpgt_epi64(vbitsa, _mm256_setzero_si256()));
	__m256i vmodeb = _mm256_add_epi64(vbitsb, _mm256_cmpgt_epi64(vbitsb, _mm256_setzero_si256()));

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
	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 3);

	{
		__m256i prev = _mm256_set1_epi8((char)0x80);

		for (auto p = pSrcBegin; p != pSrcBegin + cbStride; p += 64)
		{
			__m256i value0 = _mm256_loadu_si256((const __m256i *)p);
			__m256i value1 = _mm256_loadu_si256((const __m256i *)(p + 32));
			__m256i tmp0 = _mm256_permute2x128_si256(value0, prev, 0x03);
			__m256i tmp1 = _mm256_permute2x128_si256(value1, value0, 0x03);
			__m256i left0 = _mm256_alignr_epi8(value0, tmp0, 15);
			__m256i left1 = _mm256_alignr_epi8(value1, tmp1, 15);

			__m256i error0 = _mm256_sub_epi8(value0, left0);
			__m256i error1 = _mm256_sub_epi8(value1, left1);
			prev = value1;

			packer<CODEFEATURE_AVX2>(q, r, error0, error1);
		}
	}

	for (auto pp = pSrcBegin + cbStride; pp != pSrcEnd; pp += cbStride)
	{
		__m256i prev = _mm256_set1_epi8((char)0x80);
		__m256i topprev = _mm256_set1_epi8((char)0x80);

		for (auto p = pp; p != pp + cbStride; p += 64)
		{
			__m256i value0 = _mm256_loadu_si256((const __m256i *)p);
			__m256i value1 = _mm256_loadu_si256((const __m256i *)(p + 32));
			__m256i top0 = _mm256_loadu_si256((const __m256i *)(p - cbStride));
			__m256i top1 = _mm256_loadu_si256((const __m256i *)(p + 32 - cbStride));
			__m256i tmp0 = _mm256_permute2x128_si256(value0, prev, 0x03);
			__m256i tmp1 = _mm256_permute2x128_si256(value1, value0, 0x03);
			__m256i toptmp0 = _mm256_permute2x128_si256(top0, topprev, 0x03);
			__m256i toptmp1 = _mm256_permute2x128_si256(top1, top0, 0x03);
			__m256i left0 = _mm256_alignr_epi8(value0, tmp0, 15);
			__m256i left1 = _mm256_alignr_epi8(value1, tmp1, 15);
			__m256i topleft0 = _mm256_alignr_epi8(top0, toptmp0, 15);
			__m256i topleft1 = _mm256_alignr_epi8(top1, toptmp1, 15);

			__m256i error0 = _mm256_sub_epi8(_mm256_add_epi8(value0, topleft0), _mm256_add_epi8(left0, top0));
			__m256i error1 = _mm256_sub_epi8(_mm256_add_epi8(value1, topleft1), _mm256_add_epi8(left1, top1));
			prev = value1;
			topprev = top1;

			packer<CODEFEATURE_AVX2>(q, r, error0, error1);
		}
	}

	*cbPacked = q - pPacked;

}
#endif


template<int F>
static inline FORCEINLINE typename std::enable_if<F < CODEFEATURE_AVX2, __m128i>::type unpacker(const uint8_t*& q, const uint8_t *& r, int& shift)
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

	shift += 3;
	if (shift == 24)
	{
		r += 3;
		shift = 0;
	}

	return w;
}

template<int F>
void tuned_Unpack8SymAndRestorePlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbStride)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;

	{
		__m128i prev = _mm_set1_epi8((char)0x80);

		for (auto p = pDstBegin; p != pDstBegin + cbStride; p += 16)
		{
			__m128i w0 = unpacker<F>(q, r, shift);
			__m128i w1 = unpacker<F>(q, r, shift);
			__m128i s0 = _mm_unpacklo_epi64(w0, w1);

			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 1));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 2));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 4));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 8));
			s0 = _mm_add_epi8(s0, prev);
			_mm_storeu_si128((__m128i *)p, s0);
			prev = _mm_shuffle_epi8(s0, _mm_set1_epi8(15));
		}
	}

	for (auto pp = pDstBegin + cbStride; pp != pDstEnd; pp += cbStride)
	{
		__m128i prev = _mm_set1_epi8((char)0);

		for (auto p = pp; p != pp + cbStride; p += 16)
		{
			__m128i w0 = unpacker<F>(q, r, shift);
			__m128i w1 = unpacker<F>(q, r, shift);
			__m128i s0 = _mm_unpacklo_epi64(w0, w1);

			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 1));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 2));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 4));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 8));
			s0 = _mm_add_epi8(s0, prev);
			_mm_storeu_si128((__m128i *)p, _mm_add_epi8(s0, _mm_loadu_si128((const __m128i *)(p - cbStride))));
			prev = _mm_shuffle_epi8(s0, _mm_set1_epi8(15));
		}
	}
}

#ifdef GENERATE_SSE41
template void tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_SSE41>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbStride);
#endif

#ifdef GENERATE_AVX2
template<int F>
static inline FORCEINLINE typename std::enable_if<F == CODEFEATURE_AVX2, __m256i>::type unpacker(const uint8_t*& q, const uint8_t *& r, int& shift)
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
			__m256i s0 = unpacker<CODEFEATURE_AVX2>(q, r, shift);

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
			__m256i s0 = unpacker<CODEFEATURE_AVX2>(q, r, shift);

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
#endif


template<int F>
static inline FORCEINLINE void VECTORCALL PackForDelta(uint8_t*& q, uint8_t*& r, int& shift, __m128i wa, __m128i wb, __m128i ta, __m128i tb)
{
	int bits0, bits1, bits2, bits3;

	auto getvbits = [](__m128i xa, __m128i xb)
	{
		__m128i za, zb;
		__m128i viszeroa = _mm_cmpeq_epi64(xa, _mm_setzero_si128());
		__m128i viszerob = _mm_cmpeq_epi64(xb, _mm_setzero_si128());
		__m128i viszero = _mm_blend_epi16(viszeroa, viszerob, 0xcc);
		__m128i signsa = _mm_cmpgt_epi8(_mm_setzero_si128(), xa);
		__m128i signsb = _mm_cmpgt_epi8(_mm_setzero_si128(), xb);
		za = _mm_xor_si128(xa, signsa);
		zb = _mm_xor_si128(xb, signsb);
		za = _mm_or_si128(za, _mm_srli_epi64(za, 32));
		zb = _mm_or_si128(zb, _mm_slli_epi64(zb, 32));
		__m128i z = _mm_blend_epi16(za, zb, 0xcc);
		z = _mm_or_si128(z, _mm_srli_epi32(z, 16));
		z = _mm_or_si128(z, _mm_srli_epi32(z, 8));
		z = _mm_and_si128(z, _mm_set1_epi32(0xff));
		z = _mm_or_si128(z, _mm_set1_epi32(1));

		z = _mm_castps_si128(_mm_cvtepi32_ps(z));
		z = _mm_srli_epi32(z, 23);
		z = _mm_sub_epi32(z, _mm_set1_epi32(0x7d));

		return _mm_andnot_si128(viszero, z);
	};

	__m128i vbitsw = getvbits(wa, wb);
	__m128i vbitst = getvbits(ta, tb);
	__m128i vspatial = _mm_cmpgt_epi32(vbitst, vbitsw);
	wa = _mm_blendv_epi8(ta, wa, _mm_shuffle_epi32(vspatial, _MM_SHUFFLE(2, 2, 0, 0)));
	wb = _mm_blendv_epi8(tb, wb, _mm_shuffle_epi32(vspatial, _MM_SHUFFLE(3, 3, 1, 1)));
	__m128i vbits = _mm_min_epi32(vbitsw, vbitst);
	__m128i vrembits = _mm_sub_epi32(_mm_set1_epi32(8), vbits);
	__m128i vmode = _mm_add_epi32(vbits, _mm_cmpgt_epi32(vbits, _mm_setzero_si128()));
	vmode = _mm_or_si128(vmode, _mm_andnot_si128(vspatial, _mm_set1_epi32(8)));

	bits0 = _mm_cvtsi128_si32(vbits);
	bits1 = _mm_extract_epi32(vbits, 2);
	bits2 = _mm_extract_epi32(vbits, 1);
	bits3 = _mm_extract_epi32(vbits, 3);

	__m128i vrembitsa = _mm_and_si128(vrembits, _mm_set1_epi64x(0xffffffff));
	__m128i vrembits0 = vrembitsa;
	__m128i vrembits1 = _mm_srli_si128(vrembitsa, 8);
	__m128i vrembitsb = _mm_srli_epi64(vrembits, 32);
	__m128i vrembits2 = vrembitsb;
	__m128i vrembits3 = _mm_srli_si128(vrembitsb, 8);
	__m128i w0 = _mm_add_epi8(wa, _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits0));
	__m128i w1 = _mm_add_epi8(_mm_srli_si128(wa, 8), _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits1));
	__m128i w2 = _mm_add_epi8(wb, _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits2));
	__m128i w3 = _mm_add_epi8(_mm_srli_si128(wb, 8), _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits3));
	w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w0), vrembits0));
	w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w1), vrembits1));
	w2 = _mm_or_si128(_mm_and_si128(w2, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w2), vrembits2));
	w3 = _mm_or_si128(_mm_and_si128(w3, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w3), vrembits3));
	vrembits0 = _mm_slli_epi64(vrembits0, 1);
	vrembits1 = _mm_slli_epi64(vrembits1, 1);
	vrembits2 = _mm_slli_epi64(vrembits2, 1);
	vrembits3 = _mm_slli_epi64(vrembits3, 1);
	w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w0), vrembits0));
	w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w1), vrembits1));
	w2 = _mm_or_si128(_mm_and_si128(w2, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w2), vrembits2));
	w3 = _mm_or_si128(_mm_and_si128(w3, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w3), vrembits3));
	vrembits0 = _mm_slli_epi64(vrembits0, 1);
	vrembits1 = _mm_slli_epi64(vrembits1, 1);
	vrembits2 = _mm_slli_epi64(vrembits2, 1);
	vrembits3 = _mm_slli_epi64(vrembits3, 1);
	w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w0), vrembits0));
	w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w1), vrembits1));
	w2 = _mm_or_si128(_mm_and_si128(w2, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w2), vrembits2));
	w3 = _mm_or_si128(_mm_and_si128(w3, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w3), vrembits3));
	_mm_storel_epi64((__m128i*)q, w0);
	q += bits0;
	_mm_storel_epi64((__m128i*)q, w1);
	q += bits1;
	_mm_storel_epi64((__m128i*)q, w2);
	q += bits2;
	_mm_storel_epi64((__m128i*)q, w3);
	q += bits3;

	vmode = _mm_shuffle_epi8(vmode, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 13, 12, 9, 8, 5, 4, 1, 0));
#ifdef __x86_64__
	uint64_t modes = _mm_cvtsi128_si64(vmode);
	modes |= modes >> (32 - 4);
#else
	vmode = _mm_or_si128(vmode, _mm_srli_epi64(vmode, 32 - 4));
	uint32_t modes = _mm_cvtsi128_si32(vmode);
#endif
	modes |= modes >> (16 - 8);

	*(uint32_t *)r |= (((uint32_t)(uint16_t)modes) << shift);
	shift += 16;
	if (shift == 32)
	{
		r += 4;
		shift = 0;
	}
}

template<int F>
void tuned_Pack8SymWithDiff8(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const uint8_t *pPrevBegin, size_t cbStride)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 4);

	{
		__m128i prev = _mm_set1_epi8((char)0x80);

		for (auto p = pSrcBegin, t = pPrevBegin; p != pSrcBegin + cbStride; p += 32, t += 32)
		{
			__m128i value0 = _mm_loadu_si128((const __m128i *)p);
			__m128i value1 = _mm_loadu_si128((const __m128i *)(p + 16));
			__m128i left0 = _mm_alignr_epi8(value0, prev, 15);
			__m128i left1 = _mm_alignr_epi8(value1, value0, 15);

			__m128i error0 = _mm_sub_epi8(value0, left0);
			__m128i error1 = _mm_sub_epi8(value1, left1);
			__m128i t0 = _mm_sub_epi8(value0, _mm_loadu_si128((const __m128i *)t));
			__m128i t1 = _mm_sub_epi8(value1, _mm_loadu_si128((const __m128i *)(t + 16)));
			prev = value1;
			PackForDelta<F>(q, r, shift, error0, error1, t0, t1);
		}
	}

	for (auto pp = pSrcBegin + cbStride, tt = pPrevBegin + cbStride; pp != pSrcEnd; pp += cbStride, tt += cbStride)
	{
		__m128i prev = _mm_set1_epi8((char)0x80);
		__m128i topprev = _mm_set1_epi8((char)0x80);

		for (auto p = pp, t = tt; p != pp + cbStride; p += 32, t += 32)
		{
			__m128i value0 = _mm_loadu_si128((const __m128i *)p);
			__m128i value1 = _mm_loadu_si128((const __m128i *)(p + 16));
			__m128i top0 = _mm_loadu_si128((const __m128i *)(p - cbStride));
			__m128i top1 = _mm_loadu_si128((const __m128i *)(p + 16 - cbStride));
			__m128i left0 = _mm_alignr_epi8(value0, prev, 15);
			__m128i left1 = _mm_alignr_epi8(value1, value0, 15);
			__m128i topleft0 = _mm_alignr_epi8(top0, topprev, 15);
			__m128i topleft1 = _mm_alignr_epi8(top1, top0, 15);

			__m128i error0 = _mm_sub_epi8(_mm_add_epi8(value0, topleft0), _mm_add_epi8(left0, top0));
			__m128i error1 = _mm_sub_epi8(_mm_add_epi8(value1, topleft1), _mm_add_epi8(left1, top1));
			__m128i t0 = _mm_sub_epi8(value0, _mm_loadu_si128((const __m128i *)t));
			__m128i t1 = _mm_sub_epi8(value1, _mm_loadu_si128((const __m128i *)(t + 16)));
			prev = value1;
			topprev = top1;
			PackForDelta<F>(q, r, shift, error0, error1, t0, t1);
		}
	}

	*cbPacked = q - pPacked;
}

#ifdef GENERATE_SSE41
template void tuned_Pack8SymWithDiff8<CODEFEATURE_SSE41>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const uint8_t *pPrevBegin, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_Pack8SymWithDiff8<CODEFEATURE_AVX1>(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const uint8_t *pPrevBegin, size_t cbStride);
#endif

#ifdef GENERATE_AVX2
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
		z = _mm256_or_si256(z, _mm256_srli_epi32(z, 16));
		z = _mm256_or_si256(z, _mm256_srli_epi32(z, 8));
		z = _mm256_and_si256(z, _mm256_set1_epi32(0xff));

		// Ç±Ç±Ç≈ BSR/LZCNT
		z = _mm256_or_si256(z, _mm256_set1_epi32(1));
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
	__m256i vmode = _mm256_add_epi32(vbits, _mm256_cmpgt_epi32(vbits, _mm256_setzero_si256()));
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
			__m256i tmp0 = _mm256_permute2x128_si256(value0, prev, 0x03);
			__m256i tmp1 = _mm256_permute2x128_si256(value1, value0, 0x03);
			__m256i left0 = _mm256_alignr_epi8(value0, tmp0, 15);
			__m256i left1 = _mm256_alignr_epi8(value1, tmp1, 15);

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
		__m256i prev = _mm256_set1_epi8((char)0x80);
		__m256i topprev = _mm256_set1_epi8((char)0x80);

		for (auto p = pp, t = tt; p != pp + cbStride; p += 64, t += 64)
		{
			__m256i value0 = _mm256_loadu_si256((const __m256i *)p);
			__m256i value1 = _mm256_loadu_si256((const __m256i *)(p + 32));
			__m256i top0 = _mm256_loadu_si256((const __m256i *)(p - cbStride));
			__m256i top1 = _mm256_loadu_si256((const __m256i *)(p + 32 - cbStride));
			__m256i tmp0 = _mm256_permute2x128_si256(value0, prev, 0x03);
			__m256i tmp1 = _mm256_permute2x128_si256(value1, value0, 0x03);
			__m256i toptmp0 = _mm256_permute2x128_si256(top0, topprev, 0x03);
			__m256i toptmp1 = _mm256_permute2x128_si256(top1, top0, 0x03);
			__m256i left0 = _mm256_alignr_epi8(value0, tmp0, 15);
			__m256i left1 = _mm256_alignr_epi8(value1, tmp1, 15);
			__m256i topleft0 = _mm256_alignr_epi8(top0, toptmp0, 15);
			__m256i topleft1 = _mm256_alignr_epi8(top1, toptmp1, 15);

			__m256i error0 = _mm256_sub_epi8(_mm256_add_epi8(value0, topleft0), _mm256_add_epi8(left0, top0));
			__m256i error1 = _mm256_sub_epi8(_mm256_add_epi8(value1, topleft1), _mm256_add_epi8(left1, top1));
			__m256i t0 = _mm256_sub_epi8(value0, _mm256_loadu_si256((const __m256i *)t));
			__m256i t1 = _mm256_sub_epi8(value1, _mm256_loadu_si256((const __m256i *)(t + 32)));
			prev = value1;
			topprev = top1;

			PackForDelta<CODEFEATURE_AVX2>(q, r, error0, error1, t0, t1);
		}
	}

	*cbPacked = q - pPacked;

}
#endif


template<int F>
static inline FORCEINLINE typename std::enable_if<F < CODEFEATURE_AVX2, std::pair<__m128i, __m128i>>::type UnpackForDelta(const uint8_t*& q, const uint8_t *& r, int& shift)
{
	__m128i w;
	int mode = ((*(const uint32_t *)r) >> shift) & 7;
	__m128i rmask = _mm_cmpeq_epi64(_mm_and_si128(_mm_cvtsi32_si128(*(const uint32_t *)r >> shift), _mm_set1_epi64x(8)), _mm_set1_epi64x(8));
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

	shift += 4;
	if (shift == 32)
	{
		r += 4;
		shift = 0;
	}

	return { w, rmask };
}

template<int F>
void tuned_Unpack8SymWithDiff8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, const uint8_t *pPrevBegin, size_t cbStride)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;

	{
		__m128i prev = _mm_set1_epi8((char)0x80);

		auto t = pPrevBegin;
		for (auto p = pDstBegin; p != pDstBegin + cbStride; p += 16, t += 16)
		{
			auto w0 = UnpackForDelta<F>(q, r, shift);
			auto w1 = UnpackForDelta<F>(q, r, shift);
			__m128i s0 = _mm_unpacklo_epi64(w0.first, w1.first);
			__m128i m0 = _mm_unpacklo_epi64(w0.second, w1.second);

			auto t0 = _mm_add_epi8(s0, _mm_loadu_si128((const __m128i*)t));
			auto a = _mm_alignr_epi8(_mm_and_si128(t0, m0), prev, 15);
			s0 = _mm_andnot_si128(m0, _mm_add_epi8(s0, a));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 1));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 2));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 4));
			s0 = _mm_add_epi8(s0, _mm_andnot_si128(m0, _mm_slli_si128(s0, 8)));
			s0 = _mm_blendv_epi8(s0, t0, m0);
			_mm_storeu_si128((__m128i *)p, s0);
			prev = s0;
		}
	}

	auto tt = pPrevBegin + cbStride;
	for (auto pp = pDstBegin + cbStride; pp != pDstEnd; pp += cbStride, tt += cbStride)
	{
		__m128i prev = _mm_set1_epi8((char)0);

		auto t = tt;
		for (auto p = pp; p != pp + cbStride; p += 16, t += 16)
		{
			auto w0 = UnpackForDelta<F>(q, r, shift);
			auto w1 = UnpackForDelta<F>(q, r, shift);
			__m128i s0 = _mm_unpacklo_epi64(w0.first, w1.first);
			__m128i m0 = _mm_unpacklo_epi64(w0.second, w1.second);

			__m128i top = _mm_loadu_si128((const __m128i*)(p - cbStride));
			auto t0 = _mm_add_epi8(s0, _mm_loadu_si128((const __m128i*)t));
			auto a = _mm_alignr_epi8(_mm_and_si128(_mm_sub_epi8(t0, top), m0), prev, 15);
			s0 = _mm_andnot_si128(m0, _mm_add_epi8(s0, a));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 1));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 2));
			s0 = _mm_add_epi8(s0, _mm_slli_si128(s0, 4));
			s0 = _mm_add_epi8(s0, _mm_andnot_si128(m0, _mm_slli_si128(s0, 8)));
			s0 = _mm_add_epi8(s0, top);
			s0 = _mm_blendv_epi8(s0, t0, m0);
			_mm_storeu_si128((__m128i *)p, s0);
			prev = _mm_sub_epi8(s0, top);
		}
	}
}

#ifdef GENERATE_SSE41
template void tuned_Unpack8SymWithDiff8<CODEFEATURE_SSE41>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, const uint8_t *pPrevBegin, size_t cbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_Unpack8SymWithDiff8<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, const uint8_t *pPrevBegin, size_t cbStride);
#endif

#ifdef GENERATE_AVX2
template<int F>
static inline FORCEINLINE typename std::enable_if<F == CODEFEATURE_AVX2, std::pair<__m256i, __m256i>>::type UnpackForDelta(const uint8_t*& q, const uint8_t *& r, int& shift)
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
#endif
