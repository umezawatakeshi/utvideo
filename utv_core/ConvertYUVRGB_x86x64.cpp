/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2)
#error
#endif

extern void* enabler;

#include "POD.h"

template<int F, class C, class T>
void tuned_ConvertULY4ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	const int shift = 13;

	__m128i xy2rgb = _mm_set2_epi16_shift((-16 * C::Y2RGB + 0.5) / 0xff, C::Y2RGB, shift);
	__m128i vu2r = _mm_set2_epi16_shift(C::V2R, 0, shift);
	__m128i vu2g = _mm_set2_epi16_shift(C::V2G, C::U2G, shift);
	__m128i vu2b = _mm_set2_epi16_shift(0, C::U2B, shift);

#if defined(__AVX2__)
	__m256i xy2rgb256 = _mm256_set2_epi16_shift((-16 * C::Y2RGB + 0.5) / 0xff, C::Y2RGB, shift);
	__m256i vu2r256 = _mm256_set2_epi16_shift(C::V2R, 0, shift);
	__m256i vu2g256 = _mm256_set2_epi16_shift(C::V2G, C::U2G, shift);
	__m256i vu2b256 = _mm256_set2_epi16_shift(0, C::U2B, shift);
#endif

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride, pYBegin += cbPlaneWidth, pUBegin += cbPlaneWidth, pVBegin += cbPlaneWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - 32; pp += T::BYPP * 8)
		{
			__m128i yy = _mm_loadl_epi64((const __m128i *)y);
			__m128i uu = _mm_loadl_epi64((const __m128i *)u);
			__m128i vv = _mm_loadl_epi64((const __m128i *)v);

			__m256i xy = _mm256_cvtepu8_epi16(_mm_unpacklo_epi8(yy, _mm_setone_si128())); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
			__m256i vu = _mm256_cvtepu8_epi16(_mm_unpacklo_epi8(uu, vv)); // 00 V3 00 U3 00 V2 00 U2 00 V1 00 U1 00 V0 00 U0
			vu = _mm256_sub_epi16(vu, _mm256_set1_epi16(128));

			__m256i rgbtmp = _mm256_madd_epi16(xy, xy2rgb256);

			auto xyuv2rgb = [rgbtmp, vu, shift](__m256i vu2rgb) -> __m256i {
				__m256i rgb = _mm256_add_epi32(rgbtmp, _mm256_madd_epi16(vu, vu2rgb));
				rgb = _mm256_srai_epi32(rgb, shift);
				rgb = _mm256_packs_epi32(rgb, rgb);
				rgb = _mm256_packus_epi16(rgb, rgb);
				return rgb;
			};
			__m256i rr = xyuv2rgb(vu2r256);
			__m256i gg = xyuv2rgb(vu2g256);
			__m256i bb = xyuv2rgb(vu2b256);

			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				__m256i bgrx = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(bb, gg), _mm256_unpacklo_epi8(rr, _mm256_setone_si256()));
				_mm256_storeu_si256((__m256i *)pp, bgrx);
			}
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				__m256i bgrx = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(bb, gg), _mm256_unpacklo_epi8(rr, rr));
				__m256i bgr = _mm256_shuffle_epi8(bgrx, _mm256_set16_epi8(-1, -1, -1, -1, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0));
				bgr = _mm256_permutevar8x32_epi32(bgr, _mm256_set_epi32(-1, -1, 6, 5, 4, 2, 1, 0));
				_mm256_storeu_si256((__m256i *)pp, bgr);
			}
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				__m256i xrgb = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(_mm256_setone_si256(), rr), _mm256_unpacklo_epi8(gg, bb));
				_mm256_storeu_si256((__m256i *)pp, xrgb);
			}
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				__m256i xrgb = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(rr, rr), _mm256_unpacklo_epi8(gg, bb));
				__m256i rgb = _mm256_shuffle_epi8(xrgb, _mm256_set16_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
				rgb = _mm256_permutevar8x32_epi32(rgb, _mm256_set_epi32(-1, -1, 6, 5, 4, 2, 1, 0));
				_mm256_storeu_si256((__m256i *)pp, rgb);
			}

			y += 8;
			u += 8;
			v += 8;
		}
#else
		for (; pp <= p + cbWidth - 16; pp += T::BYPP * 4)
		{
			__m128i yy = _mm_cvtsi32_si128(*(const int *)y);
			__m128i uu = _mm_cvtsi32_si128(*(const int *)u);
			__m128i vv = _mm_cvtsi32_si128(*(const int *)v);

			__m128i xy = _mm_unpacklo_epi8(_mm_unpacklo_epi8(yy, _mm_setone_si128()), _mm_setzero_si128()); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
			__m128i vu = _mm_unpacklo_epi8(_mm_unpacklo_epi8(uu, vv), _mm_setzero_si128()); // 00 V3 00 U3 00 V2 00 U2 00 V1 00 U1 00 V0 00 U0
			vu = _mm_sub_epi16(vu, _mm_set1_epi16(128));

			__m128i rgbtmp = _mm_madd_epi16(xy, xy2rgb);

			auto xyuv2rgb = [rgbtmp, vu, shift](__m128i vu2rgb) -> __m128i {
				__m128i rgb = _mm_add_epi32(rgbtmp, _mm_madd_epi16(vu, vu2rgb));
				rgb = _mm_srai_epi32(rgb, shift);
				rgb = _mm_packs_epi32(rgb, rgb);
				rgb = _mm_packus_epi16(rgb, rgb);
				return rgb;
			};
			__m128i rr = xyuv2rgb(vu2r);
			__m128i gg = xyuv2rgb(vu2g);
			__m128i bb = xyuv2rgb(vu2b);

			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				__m128i bgrx = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, _mm_setone_si128()));
				_mm_storeu_si128((__m128i *)pp, bgrx);
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				__m128i bgrx = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, rr));
				__m128i bgr = _mm_shuffle_epi8(bgrx, _mm_set_epi8(-1, -1, -1, -1, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0));
				_mm_storeu_si128((__m128i *)pp, bgr);
			}
#endif
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rr), _mm_unpacklo_epi8(gg, bb));
				_mm_storeu_si128((__m128i *)pp, xrgb);
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rr, rr), _mm_unpacklo_epi8(gg, bb));
				__m128i rgb = _mm_shuffle_epi8(xrgb, _mm_set_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
				_mm_storeu_si128((__m128i *)pp, rgb);
			}
#endif

			y += 4;
			u += 4;
			v += 4;
		}
#endif

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			__m128i xy = _mm_cvtsi32_si128(*y | 0x00ff0000);
			__m128i uu = _mm_cvtsi32_si128(*u);
			__m128i vv = _mm_cvtsi32_si128(*v);

			__m128i vu = _mm_unpacklo_epi8(_mm_unpacklo_epi8(uu, vv), _mm_setzero_si128()); // 00 V3 00 U3 00 V2 00 U2 00 V1 00 U1 00 V0 00 U0
			vu = _mm_sub_epi16(vu, _mm_set1_epi16(128));

			__m128i rgbtmp = _mm_madd_epi16(xy, xy2rgb);

			auto xyuv2rgb = [rgbtmp, vu, shift](__m128i vu2rgb) -> __m128i {
				__m128i rgb = _mm_add_epi32(rgbtmp, _mm_madd_epi16(vu, vu2rgb));
				rgb = _mm_srai_epi32(rgb, shift);
				rgb = _mm_packs_epi32(rgb, rgb);
				rgb = _mm_packus_epi16(rgb, rgb);
				return rgb;
			};
			__m128i rr = xyuv2rgb(vu2r);
			__m128i gg = xyuv2rgb(vu2g);
			__m128i bb = xyuv2rgb(vu2b);

			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				__m128i bgrx = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, _mm_setone_si128()));
				*(uint32_t *)pp = _mm_cvtsi128_si32(bgrx);
			}
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rr), _mm_unpacklo_epi8(gg, bb));
				*(uint32_t *)pp = _mm_cvtsi128_si32(xrgb);
			}
			else if (std::is_same<T, CBGRColorOrder>::value || std::is_same<T, CRGBColorOrder>::value)
			{
				*(pp + T::B) = (uint8_t)_mm_cvtsi128_si32(bb);
				*(pp + T::G) = (uint8_t)_mm_cvtsi128_si32(gg);
				*(pp + T::R) = (uint8_t)_mm_cvtsi128_si32(rr);
			}

			y += 1;
			u += 1;
			v += 1;
		}
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

//

template<int F, class C, class T>
void tuned_ConvertRGBToULY4(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	const int shift = 14;

	__m128i rb2y, xg2y, rb2u, xg2u, rb2v, xg2v;
#if defined(__AVX2__)
	__m256i rb2y256, xg2y256, rb2u256, xg2u256, rb2v256, xg2v256;
#endif

	if (std::is_same<T, CBGRAColorOrder>::value || std::is_same<T, CBGRColorOrder>::value)
	{
		rb2y = _mm_set2_epi16_shift(C::R2Y, C::B2Y, shift);
		xg2y = _mm_set2_epi16_shift(16.5 / 0xff, C::G2Y, shift);
		rb2u = _mm_set2_epi16_shift(C::R2U, C::B2U, shift);
		xg2u = _mm_set2_epi16_shift(128.5 / 0xff, C::G2U, shift);
		rb2v = _mm_set2_epi16_shift(C::R2V, C::B2V, shift);
		xg2v = _mm_set2_epi16_shift(128.5 / 0xff, C::G2V, shift);
#if defined(__AVX2__)
		rb2y256 = _mm256_set2_epi16_shift(C::R2Y, C::B2Y, shift);
		xg2y256 = _mm256_set2_epi16_shift(16.5 / 0xff, C::G2Y, shift);
		rb2u256 = _mm256_set2_epi16_shift(C::R2U, C::B2U, shift);
		xg2u256 = _mm256_set2_epi16_shift(128.5 / 0xff, C::G2U, shift);
		rb2v256 = _mm256_set2_epi16_shift(C::R2V, C::B2V, shift);
		xg2v256 = _mm256_set2_epi16_shift(128.5 / 0xff, C::G2V, shift);
#endif
	}
	else
	{
		rb2y = _mm_set2_epi16_shift(C::B2Y, C::R2Y, shift);
		xg2y = _mm_set2_epi16_shift(C::G2Y, 16.5 / 0xff, shift);
		rb2u = _mm_set2_epi16_shift(C::B2U, C::R2U, shift);
		xg2u = _mm_set2_epi16_shift(C::G2U, 128.5 / 0xff, shift);
		rb2v = _mm_set2_epi16_shift(C::B2V, C::R2V, shift);
		xg2v = _mm_set2_epi16_shift(C::G2V, 128.5 / 0xff, shift);
#if defined(__AVX2__)
		rb2y256 = _mm256_set2_epi16_shift(C::B2Y, C::R2Y, shift);
		xg2y256 = _mm256_set2_epi16_shift(C::G2Y, 16.5 / 0xff, shift);
		rb2u256 = _mm256_set2_epi16_shift(C::B2U, C::R2U, shift);
		xg2u256 = _mm256_set2_epi16_shift(C::G2U, 128.5 / 0xff, shift);
		rb2v256 = _mm256_set2_epi16_shift(C::B2V, C::R2V, shift);
		xg2v256 = _mm256_set2_epi16_shift(C::G2V, 128.5 / 0xff, shift);
#endif
	}

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride, pYBegin += cbPlaneWidth, pUBegin += cbPlaneWidth, pVBegin += cbPlaneWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - 32; pp += T::BYPP * 8)
		{
			__m256i m = _mm256_loadu_si256((const __m256i *)pp);
			__m256i rb, xg;
			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				// m = XX R3 G3 B3 XX R2 G2 B2 XX R1 G1 B1 XX R0 G0 B0
				rb = _mm256_and_si256(m, _mm256_set1_epi16(0x00ff)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm256_or_si256(_mm256_srli_epi16(m, 8), _mm256_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				m = _mm256_permutevar8x32_epi32(m, _mm256_set_epi32(-1, 5, 4, 3, -1, 2, 1, 0));
				// m = XX XX XX XX R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
				rb = _mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm256_or_si256(_mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, -1, -1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1)), _mm256_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				// m = B3 G3 R3 XX B2 G2 R2 XX B1 G1 R1 XX B0 G0 R0 XX
				rb = _mm256_srli_epi16(m, 8); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm256_or_si256(_mm256_and_si256(m, _mm256_set1_epi32(0x00ff0000)), _mm256_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				m = _mm256_permutevar8x32_epi32(m, _mm256_set_epi32(-1, 5, 4, 3, -1, 2, 1, 0));
				// m = XX XX XX XX B3 G3 R3 B2 G2 R2 B1 G1 R1 B0 G0 R0
				rb = _mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm256_or_si256(_mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1, -1, -1)), _mm256_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}

			auto xrgb2yuv = [rb, xg, shift](__m256i rb2yuv, __m256i xg2yuv) -> __m128i {
				__m256i yuv = _mm256_add_epi32(_mm256_madd_epi16(rb, rb2yuv), _mm256_madd_epi16(xg, xg2yuv));
				yuv = _mm256_srli_epi32(yuv, shift);
				yuv = _mm256_shuffle_epi8(yuv, _mm256_set16_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0));
				yuv = _mm256_permutevar8x32_epi32(yuv, _mm256_set_epi32(-1, -1, -1, -1, -1, -1, 4, 0));
				return _mm256_castsi256_si128(yuv);
			};
			_mm_storel_epi64((__m128i *)y, xrgb2yuv(rb2y256, xg2y256));
			_mm_storel_epi64((__m128i *)u, xrgb2yuv(rb2u256, xg2u256));
			_mm_storel_epi64((__m128i *)v, xrgb2yuv(rb2v256, xg2v256));

			y += 8;
			u += 8;
			v += 8;
		}
#else
		for (; pp <= p + cbWidth - 16; pp += T::BYPP*4)
		{
			__m128i m = _mm_loadu_si128((const __m128i *)pp);
			__m128i rb, xg;
			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				// m = XX R3 G3 B3 XX R2 G2 B2 XX R1 G1 B1 XX R0 G0 B0
				rb = _mm_and_si128(m, _mm_set1_epi16(0x00ff)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm_or_si128(_mm_srli_epi16(m, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				// m = XX XX XX XX R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
				rb = _mm_shuffle_epi8(m, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm_or_si128(_mm_shuffle_epi8(m, _mm_set_epi8(-1, -1, -1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1)), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
#endif
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				// m = B3 G3 R3 XX B2 G2 R2 XX B1 G1 R1 XX B0 G0 R0 XX
				rb = _mm_srli_epi16(m, 8); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm_or_si128(_mm_and_si128(m, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				// m = XX XX XX XX B3 G3 R3 B2 G2 R2 B1 G1 R1 B0 G0 R0
				rb = _mm_shuffle_epi8(m, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm_or_si128(_mm_shuffle_epi8(m, _mm_set_epi8(-1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1, -1, -1)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}
#endif

			auto xrgb2yuv = [rb, xg, shift](__m128i rb2yuv, __m128i xg2yuv) -> uint32_t {
				__m128i yuv = _mm_add_epi32(_mm_madd_epi16(rb, rb2yuv), _mm_madd_epi16(xg, xg2yuv));
				yuv = _mm_srli_epi32(yuv, shift);
#ifdef __SSSE3__
				if (F >= CODEFEATURE_SSSE3)
				{
					yuv = _mm_shuffle_epi8(yuv, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0));
				}
				else
#endif
				{
					yuv = _mm_packs_epi32(yuv, yuv);
					yuv = _mm_packus_epi16(yuv, yuv);
				}
				return _mm_cvtsi128_si32(yuv);
			};
			*(uint32_t *)y = xrgb2yuv(rb2y, xg2y);
			*(uint32_t *)u = xrgb2yuv(rb2u, xg2u);
			*(uint32_t *)v = xrgb2yuv(rb2v, xg2v);

			y += 4;
			u += 4;
			v += 4;
		}
#endif

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			__m128i m;
			__m128i rb, xg;
			if (std::is_same<T, CBGRAColorOrder>::value || std::is_same<T, CBGRColorOrder>::value)
			{
				if (std::is_same<T, CBGRAColorOrder>::value)
				{
					m = _mm_cvtsi32_si128(*(const uint32_t *)pp); // m = XX XX XX XX XX XX XX XX XX XX XX XX XX R0 G0 B0
				}
				else
				{
					m = _mm_cvtsi32_si128(*(const uint32_t *)(pp - 1)); // m = XX XX XX XX XX XX XX XX XX XX XX XX R0 G0 B0 XX
					m = _mm_srli_epi32(m, 8);
				}
				rb = _mm_and_si128(m, _mm_set1_epi16(0x00ff)); // 00 XX 00 XX 00 XX 00 XX 00 XX 00 XX 00 R0 00 B0
				xg = _mm_or_si128(_mm_srli_epi16(m, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 XX 00 ff 00 XX 00 ff 00 XX 00 ff 00 G0
			}
			else if (std::is_same<T, CARGBColorOrder>::value || std::is_same<T, CRGBColorOrder>::value)
			{
				if (std::is_same<T, CARGBColorOrder>::value)
				{
					m = _mm_cvtsi32_si128(*(const uint32_t *)pp); // m = XX XX XX XX XX XX XX XX XX XX XX XX B0 G0 R0 XX
				}
				else
				{
					m = _mm_cvtsi32_si128(*(const uint32_t *)(pp - 1)); // m = XX XX XX XX XX XX XX XX XX XX XX XX B0 G0 R0 XX
				}
				rb = _mm_srli_epi16(m, 8); // 00 XX 00 XX 00 XX 00 XX 00 XX 00 XX 00 B0 00 R0
				xg = _mm_or_si128(_mm_and_si128(m, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 XX 00 ff 00 XX 00 ff 00 XX 00 ff 00 G0 00 ff
			}

			auto xrgb2yuv = [rb, xg, shift](__m128i rb2yuv, __m128i xg2yuv) -> uint8_t {
				__m128i yuv = _mm_add_epi32(_mm_madd_epi16(rb, rb2yuv), _mm_madd_epi16(xg, xg2yuv));
				yuv = _mm_srli_epi32(yuv, shift);
				return (uint8_t)_mm_cvtsi128_si32(yuv);
			};
			*y = xrgb2yuv(rb2y, xg2y);
			*u = xrgb2yuv(rb2u, xg2u);
			*v = xrgb2yuv(rb2v, xg2v);

			y++;
			u++;
			v++;
		}

		std::fill(y, pYBegin + cbPlaneWidth, y[-1]);
		std::fill(u, pUBegin + cbPlaneWidth, u[-1]);
		std::fill(v, pVBegin + cbPlaneWidth, v[-1]);
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

//

// èàóùÇ™ ULY4 Ç∆ÇŸÇ⁄ìØÇ∂Ç»ÇÃÇÇ»ÇÒÇ∆Ç©ÇµÇΩÇ¢
template<int F, class C, class T>
void tuned_ConvertULY2ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	const int shift = 13;

	__m128i xy2rgb = _mm_set2_epi16_shift((-16 * C::Y2RGB + 0.5) / 0xff, C::Y2RGB, shift);
	__m128i vu2r = _mm_set2_epi16_shift(C::V2R, 0, shift);
	__m128i vu2g = _mm_set2_epi16_shift(C::V2G, C::U2G, shift);
	__m128i vu2b = _mm_set2_epi16_shift(0, C::U2B, shift);

#if defined(__AVX2__)
	__m256i xy2rgb256 = _mm256_set2_epi16_shift((-16 * C::Y2RGB + 0.5) / 0xff, C::Y2RGB, shift);
	__m256i vu2r256 = _mm256_set2_epi16_shift(C::V2R, 0, shift);
	__m256i vu2g256 = _mm256_set2_epi16_shift(C::V2G, C::U2G, shift);
	__m256i vu2b256 = _mm256_set2_epi16_shift(0, C::U2B, shift);
#endif

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - 32; pp += T::BYPP * 8)
		{
			__m128i yy = _mm_loadl_epi64((const __m128i *)y);
			__m128i uu = _mm_cvtsi32_si128(*(const int *)u);
			__m128i vv = _mm_cvtsi32_si128(*(const int *)v);

			__m256i xy = _mm256_cvtepu8_epi16(_mm_unpacklo_epi8(yy, _mm_setone_si128())); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
			__m128i vutmp = _mm_unpacklo_epi8(uu, vv); // XX XX XX XX XX XX XX XX V6 U6 V4 U4 V2 U2 V0 U0
			__m256i vu = _mm256_cvtepu8_epi16(_mm_unpacklo_epi16(vutmp, vutmp)); // 00 V2 00 U2 00 V2 00 U2 00 V0 00 U0 00 V0 00 U0
			vu = _mm256_sub_epi16(vu, _mm256_set1_epi16(128));

			__m256i rgbtmp = _mm256_madd_epi16(xy, xy2rgb256);

			auto xyuv2rgb = [rgbtmp, vu, shift](__m256i vu2rgb) -> __m256i {
				__m256i rgb = _mm256_add_epi32(rgbtmp, _mm256_madd_epi16(vu, vu2rgb));
				rgb = _mm256_srai_epi32(rgb, shift);
				rgb = _mm256_packs_epi32(rgb, rgb);
				rgb = _mm256_packus_epi16(rgb, rgb);
				return rgb;
			};
			__m256i rr = xyuv2rgb(vu2r256);
			__m256i gg = xyuv2rgb(vu2g256);
			__m256i bb = xyuv2rgb(vu2b256);

			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				__m256i bgrx = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(bb, gg), _mm256_unpacklo_epi8(rr, _mm256_setone_si256()));
				_mm256_storeu_si256((__m256i *)pp, bgrx);
			}
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				__m256i bgrx = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(bb, gg), _mm256_unpacklo_epi8(rr, rr));
				__m256i bgr = _mm256_shuffle_epi8(bgrx, _mm256_set16_epi8(-1, -1, -1, -1, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0));
				bgr = _mm256_permutevar8x32_epi32(bgr, _mm256_set_epi32(-1, -1, 6, 5, 4, 2, 1, 0));
				_mm256_storeu_si256((__m256i *)pp, bgr);
			}
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				__m256i xrgb = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(_mm256_setone_si256(), rr), _mm256_unpacklo_epi8(gg, bb));
				_mm256_storeu_si256((__m256i *)pp, xrgb);
			}
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				__m256i xrgb = _mm256_unpacklo_epi16(_mm256_unpacklo_epi8(rr, rr), _mm256_unpacklo_epi8(gg, bb));
				__m256i rgb = _mm256_shuffle_epi8(xrgb, _mm256_set16_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
				rgb = _mm256_permutevar8x32_epi32(rgb, _mm256_set_epi32(-1, -1, 6, 5, 4, 2, 1, 0));
				_mm256_storeu_si256((__m256i *)pp, rgb);
			}

			y += 8;
			u += 4;
			v += 4;
		}
#else
		for (; pp <= p + cbWidth - 16; pp += T::BYPP * 4)
		{
			__m128i yy = _mm_cvtsi32_si128(*(const int *)y);
			__m128i uu = _mm_cvtsi32_si128(*(const int *)u);
			__m128i vv = _mm_cvtsi32_si128(*(const int *)v);

			__m128i xy = _mm_unpacklo_epi8(_mm_unpacklo_epi8(yy, _mm_setone_si128()), _mm_setzero_si128()); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
			__m128i vutmp = _mm_unpacklo_epi8(uu, vv); // XX XX XX XX XX XX XX XX XX XX XX XX V2 U2 V0 U0
			__m128i vu = _mm_unpacklo_epi8(_mm_unpacklo_epi16(vutmp, vutmp), _mm_setzero_si128()); // 00 V2 00 U2 00 V2 00 U2 00 V0 00 U0 00 V0 00 U0
			vu = _mm_sub_epi16(vu, _mm_set1_epi16(128));

			__m128i rgbtmp = _mm_madd_epi16(xy, xy2rgb);

			auto xyuv2rgb = [rgbtmp, vu, shift](__m128i vu2rgb) -> __m128i {
				__m128i rgb = _mm_add_epi32(rgbtmp, _mm_madd_epi16(vu, vu2rgb));
				rgb = _mm_srai_epi32(rgb, shift);
				rgb = _mm_packs_epi32(rgb, rgb);
				rgb = _mm_packus_epi16(rgb, rgb);
				return rgb;
			};
			__m128i rr = xyuv2rgb(vu2r);
			__m128i gg = xyuv2rgb(vu2g);
			__m128i bb = xyuv2rgb(vu2b);

			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				__m128i bgrx = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, _mm_setone_si128()));
				_mm_storeu_si128((__m128i *)pp, bgrx);
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				__m128i bgrx = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, rr));
				__m128i bgr = _mm_shuffle_epi8(bgrx, _mm_set_epi8(-1, -1, -1, -1, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0));
				_mm_storeu_si128((__m128i *)pp, bgr);
			}
#endif
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rr), _mm_unpacklo_epi8(gg, bb));
				_mm_storeu_si128((__m128i *)pp, xrgb);
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rr, rr), _mm_unpacklo_epi8(gg, bb));
				__m128i rgb = _mm_shuffle_epi8(xrgb, _mm_set_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
				_mm_storeu_si128((__m128i *)pp, rgb);
			}
#endif

			y += 4;
			u += 2;
			v += 2;
		}
#endif

		for (; pp < p + cbWidth; pp += T::BYPP * 2)
		{
			__m128i yy = _mm_cvtsi32_si128(*(const int *)y);
			__m128i uu = _mm_cvtsi32_si128(*u);
			__m128i vv = _mm_cvtsi32_si128(*v);

			__m128i xy = _mm_unpacklo_epi8(_mm_unpacklo_epi8(yy, _mm_setone_si128()), _mm_setzero_si128()); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
			__m128i vutmp = _mm_unpacklo_epi8(uu, vv); // XX XX XX XX XX XX XX XX XX XX XX XX V2 U2 V0 U0
			__m128i vu = _mm_unpacklo_epi8(_mm_unpacklo_epi16(vutmp, vutmp), _mm_setzero_si128()); // 00 V2 00 U2 00 V2 00 U2 00 V0 00 U0 00 V0 00 U0
			vu = _mm_sub_epi16(vu, _mm_set1_epi16(128));

			__m128i rgbtmp = _mm_madd_epi16(xy, xy2rgb);

			auto xyuv2rgb = [rgbtmp, vu, shift](__m128i vu2rgb) -> __m128i {
				__m128i rgb = _mm_add_epi32(rgbtmp, _mm_madd_epi16(vu, vu2rgb));
				rgb = _mm_srai_epi32(rgb, shift);
				rgb = _mm_packs_epi32(rgb, rgb);
				rgb = _mm_packus_epi16(rgb, rgb);
				return rgb;
			};
			__m128i rr = xyuv2rgb(vu2r);
			__m128i gg = xyuv2rgb(vu2g);
			__m128i bb = xyuv2rgb(vu2b);

			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				__m128i bgrx = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, _mm_setone_si128()));
				_mm_storel_epi64((__m128i *)pp, bgrx);
			}
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rr), _mm_unpacklo_epi8(gg, bb));
				_mm_storel_epi64((__m128i *)pp, xrgb);
			}
			else if (std::is_same<T, CBGRColorOrder>::value || std::is_same<T, CRGBColorOrder>::value)
			{
				uint32_t b = _mm_cvtsi128_si32(bb);
				uint32_t g = _mm_cvtsi128_si32(gg);
				uint32_t r = _mm_cvtsi128_si32(rr);
				*(pp + T::B) = (uint8_t)b;
				*(pp + T::G) = (uint8_t)g;
				*(pp + T::R) = (uint8_t)r;
				*(pp + T::BYPP + T::B) = (uint8_t)(b >> 8);
				*(pp + T::BYPP + T::G) = (uint8_t)(g >> 8);
				*(pp + T::BYPP + T::R) = (uint8_t)(r >> 8);
			}

			y += 2;
			u += 1;
			v += 1;
		}
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

//

// ULY2ToRGB ÇŸÇ«Ç≈ÇÕÇ»Ç¢Ç™ ULY4 Ç∆Ç©Ç»ÇËãﬂÇ¢ÇÃÇà»â∫ó™
template<int F, class C, class T>
void tuned_ConvertRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	const int shift = 14;

	__m128i rb2y, xg2y, rb2u, xg2u, rb2v, xg2v;
#if defined(__AVX2__)
	__m256i rb2y256, xg2y256, rb2u256, xg2u256, rb2v256, xg2v256;
#endif

	if (std::is_same<T, CBGRAColorOrder>::value || std::is_same<T, CBGRColorOrder>::value)
	{
		rb2y = _mm_set2_epi16_shift(C::R2Y, C::B2Y, shift);
		xg2y = _mm_set2_epi16_shift(16.5 / 0xff, C::G2Y, shift);
		rb2u = _mm_set4_epi16_shift(0, 0, C::R2U, C::B2U, shift);
		xg2u = _mm_set4_epi16_shift(0, 0, 128.5 / 0xff, C::G2U, shift);
		rb2v = _mm_set4_epi16_shift(0, 0, C::R2V, C::B2V, shift);
		xg2v = _mm_set4_epi16_shift(0, 0, 128.5 / 0xff, C::G2V, shift);
#if defined(__AVX2__)
		rb2y256 = _mm256_set2_epi16_shift(C::R2Y, C::B2Y, shift);
		xg2y256 = _mm256_set2_epi16_shift(16.5 / 0xff, C::G2Y, shift);
		rb2u256 = _mm256_set4_epi16_shift(0, 0, C::R2U, C::B2U, shift);
		xg2u256 = _mm256_set4_epi16_shift(0, 0, 128.5 / 0xff, C::G2U, shift);
		rb2v256 = _mm256_set4_epi16_shift(0, 0, C::R2V, C::B2V, shift);
		xg2v256 = _mm256_set4_epi16_shift(0, 0, 128.5 / 0xff, C::G2V, shift);
#endif
	}
	else
	{
		rb2y = _mm_set2_epi16_shift(C::B2Y, C::R2Y, shift);
		xg2y = _mm_set2_epi16_shift(C::G2Y, 16.5 / 0xff, shift);
		rb2u = _mm_set4_epi16_shift(0, 0, C::B2U, C::R2U, shift);
		xg2u = _mm_set4_epi16_shift(0, 0, C::G2U, 128.5 / 0xff, shift);
		rb2v = _mm_set4_epi16_shift(0, 0, C::B2V, C::R2V, shift);
		xg2v = _mm_set4_epi16_shift(0, 0, C::G2V, 128.5 / 0xff, shift);
#if defined(__AVX2__)
		rb2y256 = _mm256_set2_epi16_shift(C::B2Y, C::R2Y, shift);
		xg2y256 = _mm256_set2_epi16_shift(C::G2Y, 16.5 / 0xff, shift);
		rb2u256 = _mm256_set4_epi16_shift(0, 0, C::B2U, C::R2U, shift);
		xg2u256 = _mm256_set4_epi16_shift(0, 0, C::G2U, 128.5 / 0xff, shift);
		rb2v256 = _mm256_set4_epi16_shift(0, 0, C::B2V, C::R2V, shift);
		xg2v256 = _mm256_set4_epi16_shift(0, 0, C::G2V, 128.5 / 0xff, shift);
#endif
	}

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - 32; pp += T::BYPP * 8)
		{
			__m256i m = _mm256_loadu_si256((const __m256i *)pp);
			__m256i rb, xg;
			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				// m = XX R3 G3 B3 XX R2 G2 B2 XX R1 G1 B1 XX R0 G0 B0
				rb = _mm256_and_si256(m, _mm256_set1_epi16(0x00ff)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm256_or_si256(_mm256_srli_epi16(m, 8), _mm256_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				m = _mm256_permutevar8x32_epi32(m, _mm256_set_epi32(-1, 5, 4, 3, -1, 2, 1, 0));
				// m = XX XX XX XX R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
				rb = _mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm256_or_si256(_mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, -1, -1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1)), _mm256_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				// m = B3 G3 R3 XX B2 G2 R2 XX B1 G1 R1 XX B0 G0 R0 XX
				rb = _mm256_srli_epi16(m, 8); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm256_or_si256(_mm256_and_si256(m, _mm256_set1_epi32(0x00ff0000)), _mm256_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				m = _mm256_permutevar8x32_epi32(m, _mm256_set_epi32(-1, 5, 4, 3, -1, 2, 1, 0));
				// m = XX XX XX XX B3 G3 R3 B2 G2 R2 B1 G1 R1 B0 G0 R0
				rb = _mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm256_or_si256(_mm256_shuffle_epi8(m, _mm256_set16_epi8(-1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1, -1, -1)), _mm256_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}

			__m256i yy = _mm256_add_epi32(_mm256_madd_epi16(rb, rb2y256), _mm256_madd_epi16(xg, xg2y256));
			yy = _mm256_srli_epi32(yy, shift);
			yy = _mm256_shuffle_epi8(yy, _mm256_set16_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0));
			yy = _mm256_permutevar8x32_epi32(yy, _mm256_set_epi32(-1, -1, -1, -1, -1, -1, 4, 0));
			_mm_storel_epi64((__m128i *)y, _mm256_castsi256_si128(yy));

			rb = _mm256_add_epi16(rb, _mm256_srli_epi64(rb, 32));
			xg = _mm256_add_epi16(xg, _mm256_srli_epi64(xg, 32));
			auto xrgb2uv = [rb, xg, shift](__m256i rb2uv, __m256i xg2uv) -> uint32_t {
				__m256i uv = _mm256_add_epi32(_mm256_madd_epi16(rb, rb2uv), _mm256_madd_epi16(xg, xg2uv));
				uv = _mm256_srli_epi32(uv, shift + 1);
				uv = _mm256_permutevar8x32_epi32(uv, _mm256_set_epi32(-1, -1, -1, -1, 6, 4, 2, 0));
				__m128i uv128 = _mm_shuffle_epi8(_mm256_castsi256_si128(uv), _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0));
				return _mm_cvtsi128_si32(uv128);
			};
			*(uint32_t *)u = xrgb2uv(rb2u256, xg2u256);
			*(uint32_t *)v = xrgb2uv(rb2v256, xg2v256);

			y += 8;
			u += 4;
			v += 4;
		}
#else
		for (; pp <= p + cbWidth - 16; pp += T::BYPP * 4)
		{
			__m128i m = _mm_loadu_si128((const __m128i *)pp);
			__m128i rb, xg;
			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				// m = XX R3 G3 B3 XX R2 G2 B2 XX R1 G1 B1 XX R0 G0 B0
				rb = _mm_and_si128(m, _mm_set1_epi16(0x00ff)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm_or_si128(_mm_srli_epi16(m, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				// m = XX XX XX XX R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
				rb = _mm_shuffle_epi8(m, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
				xg = _mm_or_si128(_mm_shuffle_epi8(m, _mm_set_epi8(-1, -1, -1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1)), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
			}
#endif
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				// m = B3 G3 R3 XX B2 G2 R2 XX B1 G1 R1 XX B0 G0 R0 XX
				rb = _mm_srli_epi16(m, 8); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm_or_si128(_mm_and_si128(m, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				// m = XX XX XX XX B3 G3 R3 B2 G2 R2 B1 G1 R1 B0 G0 R0
				rb = _mm_shuffle_epi8(m, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
				xg = _mm_or_si128(_mm_shuffle_epi8(m, _mm_set_epi8(-1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1, -1, -1)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
			}
#endif

			__m128i yy = _mm_add_epi32(_mm_madd_epi16(rb, rb2y), _mm_madd_epi16(xg, xg2y));
			yy = _mm_srli_epi32(yy, shift);
#ifdef __SSSE3__
			if (F >= CODEFEATURE_SSSE3)
			{
				yy = _mm_shuffle_epi8(yy, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0));
			}
			else
#endif
			{
				yy = _mm_packs_epi32(yy, yy);
				yy = _mm_packus_epi16(yy, yy);
			}
			*(uint32_t *)y = _mm_cvtsi128_si32(yy);

			rb = _mm_add_epi16(rb, _mm_srli_epi64(rb, 32));
			xg = _mm_add_epi16(xg, _mm_srli_epi64(xg, 32));
			auto xrgb2uv = [rb, xg, shift](__m128i rb2uv, __m128i xg2uv) -> uint32_t {
				__m128i uv = _mm_add_epi32(_mm_madd_epi16(rb, rb2uv), _mm_madd_epi16(xg, xg2uv));
				uv = _mm_srli_epi32(uv, shift + 1);
#ifdef __SSSE3__
				if (F >= CODEFEATURE_SSSE3)
				{
					uv = _mm_shuffle_epi8(uv, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 0));
				}
				else
#endif
				{
					uv = _mm_shuffle_epi32(uv, _MM_SHUFFLE(0, 0, 2, 0));
					uv = _mm_packs_epi32(uv, uv);
					uv = _mm_packus_epi16(uv, uv);
				}
				return _mm_cvtsi128_si32(uv);
			};
			*(uint16_t *)u = xrgb2uv(rb2u, xg2u);
			*(uint16_t *)v = xrgb2uv(rb2v, xg2v);

			y += 4;
			u += 2;
			v += 2;
		}
#endif

		for (; pp < p + cbWidth; pp += T::BYPP * 2)
		{
			__m128i m;
			__m128i rb, xg;
			if (std::is_same<T, CBGRAColorOrder>::value || std::is_same<T, CBGRColorOrder>::value)
			{
				if (std::is_same<T, CBGRAColorOrder>::value)
				{
					m = _mm_loadl_epi64((const __m128i *)pp); // m = XX XX XX XX XX XX XX XX XX R1 G1 B1 XX R0 G0 B0
				}
				else
				{
#ifdef __SSSE3__
					m = _mm_loadl_epi64((const __m128i *)(pp - 2)); // m = XX XX XX XX XX XX XX XX R1 G1 B1 R0 G0 B0 XX XX
					m = _mm_shuffle_epi8(m, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, -1, 4, 3, 2));
#endif
				}
				rb = _mm_and_si128(m, _mm_set1_epi16(0x00ff)); // 00 XX 00 XX 00 XX 00 XX 00 R1 00 B1 00 R0 00 B0
				xg = _mm_or_si128(_mm_srli_epi16(m, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 XX 00 ff 00 XX 00 ff 00 G1 00 ff 00 G0
			}
			else if (std::is_same<T, CARGBColorOrder>::value || std::is_same<T, CRGBColorOrder>::value)
			{
				if (std::is_same<T, CARGBColorOrder>::value)
				{
					m = _mm_loadl_epi64((const __m128i *)pp); // m = XX XX XX XX XX XX XX XX B1 G1 R1 XX B0 G0 R0 XX
				}
				else
				{
#ifdef __SSSE3__
					m = _mm_loadl_epi64((const __m128i *)(pp - 2)); // m = XX XX XX XX XX XX XX XX B1 G1 R1 B0 G0 R0 XX XX
					m = _mm_shuffle_epi8(m, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, -1, 4, 3, 2, -1));
#endif
				}
				rb = _mm_srli_epi16(m, 8); // 00 XX 00 XX 00 XX 00 XX 00 XX 00 XX 00 B0 00 R0
				xg = _mm_or_si128(_mm_and_si128(m, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 XX 00 ff 00 XX 00 ff 00 XX 00 ff 00 G0 00 ff
			}

			__m128i yy = _mm_add_epi32(_mm_madd_epi16(rb, rb2y), _mm_madd_epi16(xg, xg2y));
			yy = _mm_srli_epi32(yy, shift);
#ifdef __SSSE3__
			if (F >= CODEFEATURE_SSSE3)
			{
				yy = _mm_shuffle_epi8(yy, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, 0));
			}
			else
#endif
			{
				yy = _mm_packs_epi32(yy, yy);
				yy = _mm_packus_epi16(yy, yy);
			}
			*(uint16_t *)y = _mm_cvtsi128_si32(yy);

			rb = _mm_add_epi16(rb, _mm_srli_epi64(rb, 32));
			xg = _mm_add_epi16(xg, _mm_srli_epi64(xg, 32));
			auto xrgb2uv = [rb, xg, shift](__m128i rb2uv, __m128i xg2uv) -> uint8_t {
				__m128i uv = _mm_add_epi32(_mm_madd_epi16(rb, rb2uv), _mm_madd_epi16(xg, xg2uv));
				uv = _mm_srli_epi32(uv, shift + 1);
				return (uint8_t)_mm_cvtsi128_si32(uv);
			};
			*u = xrgb2uv(rb2u, xg2u);
			*v = xrgb2uv(rb2v, xg2v);

			y += 2;
			u++;
			v++;
		}

		std::fill(y, pYBegin + cbYWidth, y[-1]);
		std::fill(u, pUBegin + cbCWidth, u[-1]);
		std::fill(v, pVBegin + cbCWidth, v[-1]);
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

//

template<int F, class C, class T>
void tuned_ConvertULY0ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	const int shift = 13;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	__m128i xy2rgb = _mm_set2_epi16_shift((-16 * C::Y2RGB + 0.5) / 0xff, C::Y2RGB, shift);
	__m128i vu2r = _mm_set2_epi16_shift(C::V2R, 0, shift);
	__m128i vu2g = _mm_set2_epi16_shift(C::V2G, C::U2G, shift);
	__m128i vu2b = _mm_set2_epi16_shift(0, C::U2B, shift);

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto *ppp = pDstBegin; ppp != pDstEnd; ppp += scbRawStripeSize)
	{
		for (auto p = ppp; p != ppp + scbPredictStride; p += scbStride)
		{
			auto pp = p;

			for (; pp <= p + cbWidth - 16; pp += T::BYPP * 4)
			{
				__m128i yyt = _mm_cvtsi32_si128(*(const int *)y);
				__m128i yyb = _mm_cvtsi32_si128(*(const int *)(y + dwYPlaneGrossWidth));
				__m128i uu = _mm_cvtsi32_si128(*(const int *)u);
				__m128i vv = _mm_cvtsi32_si128(*(const int *)v);

				__m128i xyt = _mm_unpacklo_epi8(_mm_unpacklo_epi8(yyt, _mm_setone_si128()), _mm_setzero_si128()); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
				__m128i xyb = _mm_unpacklo_epi8(_mm_unpacklo_epi8(yyb, _mm_setone_si128()), _mm_setzero_si128()); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
				__m128i vutmp = _mm_unpacklo_epi8(uu, vv); // XX XX XX XX XX XX XX XX XX XX XX XX V2 U2 V0 U0
				__m128i vu = _mm_unpacklo_epi8(_mm_unpacklo_epi16(vutmp, vutmp), _mm_setzero_si128()); // 00 V2 00 U2 00 V2 00 U2 00 V0 00 U0 00 V0 00 U0
				vu = _mm_sub_epi16(vu, _mm_set1_epi16(128));

				__m128i rgbtmpt = _mm_madd_epi16(xyt, xy2rgb);
				__m128i rgbtmpb = _mm_madd_epi16(xyb, xy2rgb);

				auto xyuv2rgb = [vu, shift](__m128i rgbtmp, __m128i vu2rgb) -> __m128i {
					__m128i rgb = _mm_add_epi32(rgbtmp, _mm_madd_epi16(vu, vu2rgb));
					rgb = _mm_srai_epi32(rgb, shift);
					rgb = _mm_packs_epi32(rgb, rgb);
					rgb = _mm_packus_epi16(rgb, rgb);
					return rgb;
				};
				__m128i rrt = xyuv2rgb(rgbtmpt, vu2r);
				__m128i ggt = xyuv2rgb(rgbtmpt, vu2g);
				__m128i bbt = xyuv2rgb(rgbtmpt, vu2b);
				__m128i rrb = xyuv2rgb(rgbtmpb, vu2r);
				__m128i ggb = xyuv2rgb(rgbtmpb, vu2g);
				__m128i bbb = xyuv2rgb(rgbtmpb, vu2b);

				if (std::is_same<T, CBGRAColorOrder>::value)
				{
					__m128i bgrxt = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bbt, ggt), _mm_unpacklo_epi8(rrt, _mm_setone_si128()));
					__m128i bgrxb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bbb, ggb), _mm_unpacklo_epi8(rrb, _mm_setone_si128()));
					_mm_storeu_si128((__m128i *)pp, bgrxt);
					_mm_storeu_si128((__m128i *)(pp + scbPredictStride), bgrxb);
				}
#ifdef __SSSE3__
				else if (std::is_same<T, CBGRColorOrder>::value)
				{
					__m128i bgrxt = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bbt, ggt), _mm_unpacklo_epi8(rrt, rrt));
					__m128i bgrxb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bbb, ggb), _mm_unpacklo_epi8(rrb, rrb));
					__m128i bgrt = _mm_shuffle_epi8(bgrxt, _mm_set_epi8(-1, -1, -1, -1, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0));
					__m128i bgrb = _mm_shuffle_epi8(bgrxb, _mm_set_epi8(-1, -1, -1, -1, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0));
					_mm_storeu_si128((__m128i *)pp, bgrt);
					_mm_storeu_si128((__m128i *)(pp + scbPredictStride), bgrb);
				}
#endif
				else if (std::is_same<T, CARGBColorOrder>::value)
				{
					__m128i xrgbt = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rrt), _mm_unpacklo_epi8(ggt, bbt));
					__m128i xrgbb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rrb), _mm_unpacklo_epi8(ggb, bbb));
					_mm_storeu_si128((__m128i *)pp, xrgbt);
					_mm_storeu_si128((__m128i *)(pp + scbPredictStride), xrgbb);
				}
#ifdef __SSSE3__
				else if (std::is_same<T, CRGBColorOrder>::value)
				{
					__m128i xrgbt = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rrt, rrt), _mm_unpacklo_epi8(ggt, bbt));
					__m128i xrgbb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rrb, rrb), _mm_unpacklo_epi8(ggb, bbb));
					__m128i rgbt = _mm_shuffle_epi8(xrgbt, _mm_set_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
					__m128i rgbb = _mm_shuffle_epi8(xrgbb, _mm_set_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
					_mm_storeu_si128((__m128i *)pp, rgbt);
					_mm_storeu_si128((__m128i *)(pp + scbPredictStride), rgbb);
				}
#endif

				y += 4;
				u += 2;
				v += 2;
			}

			for (; pp < p + cbWidth; pp += T::BYPP * 2)
			{
				__m128i yyt = _mm_cvtsi32_si128(*(const int *)y);
				__m128i yyb = _mm_cvtsi32_si128(*(const int *)(y + dwYPlaneGrossWidth));
				__m128i uu = _mm_cvtsi32_si128(*u);
				__m128i vv = _mm_cvtsi32_si128(*v);

				__m128i xyt = _mm_unpacklo_epi8(_mm_unpacklo_epi8(yyt, _mm_setone_si128()), _mm_setzero_si128()); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
				__m128i xyb = _mm_unpacklo_epi8(_mm_unpacklo_epi8(yyb, _mm_setone_si128()), _mm_setzero_si128()); // 00 ff 00 Y3 00 ff 00 Y2 00 ff 00 Y1 00 ff 00 Y0
				__m128i vutmp = _mm_unpacklo_epi8(uu, vv); // XX XX XX XX XX XX XX XX XX XX XX XX V2 U2 V0 U0
				__m128i vu = _mm_unpacklo_epi8(_mm_unpacklo_epi16(vutmp, vutmp), _mm_setzero_si128()); // 00 V2 00 U2 00 V2 00 U2 00 V0 00 U0 00 V0 00 U0
				vu = _mm_sub_epi16(vu, _mm_set1_epi16(128));

				__m128i rgbtmpt = _mm_madd_epi16(xyt, xy2rgb);
				__m128i rgbtmpb = _mm_madd_epi16(xyb, xy2rgb);

				auto xyuv2rgb = [vu, shift](__m128i rgbtmp, __m128i vu2rgb) -> __m128i {
					__m128i rgb = _mm_add_epi32(rgbtmp, _mm_madd_epi16(vu, vu2rgb));
					rgb = _mm_srai_epi32(rgb, shift);
					rgb = _mm_packs_epi32(rgb, rgb);
					rgb = _mm_packus_epi16(rgb, rgb);
					return rgb;
				};
				__m128i rrt = xyuv2rgb(rgbtmpt, vu2r);
				__m128i ggt = xyuv2rgb(rgbtmpt, vu2g);
				__m128i bbt = xyuv2rgb(rgbtmpt, vu2b);
				__m128i rrb = xyuv2rgb(rgbtmpb, vu2r);
				__m128i ggb = xyuv2rgb(rgbtmpb, vu2g);
				__m128i bbb = xyuv2rgb(rgbtmpb, vu2b);

				if (std::is_same<T, CBGRAColorOrder>::value)
				{
					__m128i bgrxt = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bbt, ggt), _mm_unpacklo_epi8(rrt, _mm_setone_si128()));
					__m128i bgrxb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bbb, ggb), _mm_unpacklo_epi8(rrb, _mm_setone_si128()));
					_mm_storel_epi64((__m128i *)pp, bgrxt);
					_mm_storel_epi64((__m128i *)(pp + scbPredictStride), bgrxb);
				}
				else if (std::is_same<T, CARGBColorOrder>::value)
				{
					__m128i xrgbt = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rrt), _mm_unpacklo_epi8(ggt, bbt));
					__m128i xrgbb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rrb), _mm_unpacklo_epi8(ggb, bbb));
					_mm_storel_epi64((__m128i *)pp, xrgbt);
					_mm_storel_epi64((__m128i *)(pp + scbPredictStride), xrgbb);
				}
				else if (std::is_same<T, CBGRColorOrder>::value || std::is_same<T, CRGBColorOrder>::value)
				{
					uint32_t b0 = _mm_cvtsi128_si32(bbt);
					uint32_t g0 = _mm_cvtsi128_si32(ggt);
					uint32_t r0 = _mm_cvtsi128_si32(rrt);
					uint32_t b1 = _mm_cvtsi128_si32(bbb); // Ç±Ç±Ç‹Ç≈óàÇƒèâÇﬂÇƒ t/b ÉTÉtÉBÉbÉNÉXÇ∆Ç¢Ç§ïœêîñΩñºÇ™îjí]ÇµÇƒÇ¢ÇÈÇ±Ç∆Ç…ãCÇ√Ç≠
					uint32_t g1 = _mm_cvtsi128_si32(ggb);
					uint32_t r1 = _mm_cvtsi128_si32(rrb);
					*(pp + T::B) = (uint8_t)b0;
					*(pp + T::G) = (uint8_t)g0;
					*(pp + T::R) = (uint8_t)r0;
					*(pp + T::BYPP + T::B) = (uint8_t)(b0 >> 8);
					*(pp + T::BYPP + T::G) = (uint8_t)(g0 >> 8);
					*(pp + T::BYPP + T::R) = (uint8_t)(r0 >> 8);
					*(pp + scbPredictStride + T::B) = (uint8_t)b1;
					*(pp + scbPredictStride + T::G) = (uint8_t)g1;
					*(pp + scbPredictStride + T::R) = (uint8_t)r1;
					*(pp + scbPredictStride + T::BYPP + T::B) = (uint8_t)(b1 >> 8);
					*(pp + scbPredictStride + T::BYPP + T::G) = (uint8_t)(g1 >> 8);
					*(pp + scbPredictStride + T::BYPP + T::R) = (uint8_t)(r1 >> 8);
				}

				y += 2;
				u += 1;
				v += 1;
			}
		}

		y += dwYPlaneGrossWidth;
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
#endif

//

template<int F, class C, class T>
void tuned_ConvertRGBToULY0(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	const int shift = 14;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	__m128i rb2y, xg2y, rb2u, xg2u, rb2v, xg2v;

	if (std::is_same<T, CBGRAColorOrder>::value || std::is_same<T, CBGRColorOrder>::value)
	{
		rb2y = _mm_set2_epi16_shift(C::R2Y, C::B2Y, shift);
		xg2y = _mm_set2_epi16_shift(16.5 / 0xff, C::G2Y, shift);
		rb2u = _mm_set4_epi16_shift(0, 0, C::R2U, C::B2U, shift);
		xg2u = _mm_set4_epi16_shift(0, 0, 128.5 / 0xff, C::G2U, shift);
		rb2v = _mm_set4_epi16_shift(0, 0, C::R2V, C::B2V, shift);
		xg2v = _mm_set4_epi16_shift(0, 0, 128.5 / 0xff, C::G2V, shift);
	}
	else
	{
		rb2y = _mm_set2_epi16_shift(C::B2Y, C::R2Y, shift);
		xg2y = _mm_set2_epi16_shift(C::G2Y, 16.5 / 0xff, shift);
		rb2u = _mm_set4_epi16_shift(0, 0, C::B2U, C::R2U, shift);
		xg2u = _mm_set4_epi16_shift(0, 0, C::G2U, 128.5 / 0xff, shift);
		rb2v = _mm_set4_epi16_shift(0, 0, C::B2V, C::R2V, shift);
		xg2v = _mm_set4_epi16_shift(0, 0, C::G2V, 128.5 / 0xff, shift);
	}

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto ppp = pSrcBegin; ppp != pSrcEnd; ppp += scbRawStripeSize)
	{
		for (auto p = ppp; p != ppp + scbPredictStride; p += scbStride)
		{
			auto pp = p;

			for (; pp <= p + cbWidth - 16; pp += T::BYPP * 4)
			{
				__m128i mt = _mm_loadu_si128((const __m128i *)pp);
				__m128i mb = _mm_loadu_si128((const __m128i *)(pp + scbPredictStride));
				__m128i rbt, rbb, xgt, xgb;
				if (std::is_same<T, CBGRAColorOrder>::value)
				{
					// m = XX R3 G3 B3 XX R2 G2 B2 XX R1 G1 B1 XX R0 G0 B0
					rbt = _mm_and_si128(mt, _mm_set1_epi16(0x00ff)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
					rbb = _mm_and_si128(mb, _mm_set1_epi16(0x00ff)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
					xgt = _mm_or_si128(_mm_srli_epi16(mt, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
					xgb = _mm_or_si128(_mm_srli_epi16(mb, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
				}
#ifdef __SSSE3__
				else if (std::is_same<T, CBGRColorOrder>::value)
				{
					// m = XX XX XX XX R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
					rbt = _mm_shuffle_epi8(mt, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
					rbb = _mm_shuffle_epi8(mb, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 R3 00 B3 00 R2 00 B2 00 R1 00 B1 00 R0 00 B0
					xgt = _mm_or_si128(_mm_shuffle_epi8(mt, _mm_set_epi8(-1, -1, -1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1)), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
					xgb = _mm_or_si128(_mm_shuffle_epi8(mb, _mm_set_epi8(-1, -1, -1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1)), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0
				}
#endif
				else if (std::is_same<T, CARGBColorOrder>::value)
				{
					// m = B3 G3 R3 XX B2 G2 R2 XX B1 G1 R1 XX B0 G0 R0 XX
					rbt = _mm_srli_epi16(mt, 8); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
					rbb = _mm_srli_epi16(mb, 8); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
					xgt = _mm_or_si128(_mm_and_si128(mt, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
					xgb = _mm_or_si128(_mm_and_si128(mb, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
				}
#ifdef __SSSE3__
				else if (std::is_same<T, CRGBColorOrder>::value)
				{
					// m = XX XX XX XX B3 G3 R3 B2 G2 R2 B1 G1 R1 B0 G0 R0
					rbt = _mm_shuffle_epi8(mt, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
					rbb = _mm_shuffle_epi8(mb, _mm_set_epi8(-1, 11, -1, 9, -1, 8, -1, 6, -1, 5, -1, 3, -1, 2, -1, 0)); // 00 B3 00 R3 00 B2 00 R2 00 B1 00 R1 00 B0 00 R0
					xgt = _mm_or_si128(_mm_shuffle_epi8(mt, _mm_set_epi8(-1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1, -1, -1)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
					xgb = _mm_or_si128(_mm_shuffle_epi8(mb, _mm_set_epi8(-1, 10, -1, -1, -1, 7, -1, -1, -1, 4, -1, -1, -1, 1, -1, -1)), _mm_set1_epi32(0x000000ff)); // 00 G3 00 ff 00 G2 00 ff 00 G1 00 ff 00 G0 00 ff
				}
#endif

				__m128i yyt = _mm_add_epi32(_mm_madd_epi16(rbt, rb2y), _mm_madd_epi16(xgt, xg2y));
				__m128i yyb = _mm_add_epi32(_mm_madd_epi16(rbb, rb2y), _mm_madd_epi16(xgb, xg2y));
				yyt = _mm_srli_epi32(yyt, shift);
				yyb = _mm_srli_epi32(yyb, shift);
#ifdef __SSSE3__
				if (F >= CODEFEATURE_SSSE3)
				{
					yyt = _mm_shuffle_epi8(yyt, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0));
					yyb = _mm_shuffle_epi8(yyb, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0));
				}
				else
#endif
				{
					yyt = _mm_packs_epi32(yyt, yyt);
					yyb = _mm_packs_epi32(yyb, yyb);
					yyt = _mm_packus_epi16(yyt, yyt);
					yyb = _mm_packus_epi16(yyb, yyb);
				}
				*(uint32_t *)y = _mm_cvtsi128_si32(yyt);
				*(uint32_t *)(y + dwYPlaneGrossWidth) = _mm_cvtsi128_si32(yyb);

				__m128i rb = _mm_add_epi16(_mm_add_epi16(rbt, _mm_srli_epi64(rbt, 32)), _mm_add_epi16(rbb, _mm_srli_epi64(rbb, 32)));
				__m128i xg = _mm_add_epi16(_mm_add_epi16(xgt, _mm_srli_epi64(xgt, 32)), _mm_add_epi16(xgb, _mm_srli_epi64(xgb, 32)));
				auto xrgb2uv = [rb, xg, shift](__m128i rb2uv, __m128i xg2uv) -> uint32_t {
					__m128i uv = _mm_add_epi32(_mm_madd_epi16(rb, rb2uv), _mm_madd_epi16(xg, xg2uv));
					uv = _mm_srli_epi32(uv, shift + 2);
#ifdef __SSSE3__
					if (F >= CODEFEATURE_SSSE3)
					{
						uv = _mm_shuffle_epi8(uv, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 0));
					}
					else
#endif
					{
						uv = _mm_shuffle_epi32(uv, _MM_SHUFFLE(0, 0, 2, 0));
						uv = _mm_packs_epi32(uv, uv);
						uv = _mm_packus_epi16(uv, uv);
					}
					return _mm_cvtsi128_si32(uv);
				};
				*(uint16_t *)u = xrgb2uv(rb2u, xg2u);
				*(uint16_t *)v = xrgb2uv(rb2v, xg2v);

				y += 4;
				u += 2;
				v += 2;
			}
			for (; pp < p + cbWidth; pp += T::BYPP * 2)
			{
				__m128i mt, mb;
				__m128i rbt, rbb, xgt, xgb;
				if (std::is_same<T, CBGRAColorOrder>::value || std::is_same<T, CBGRColorOrder>::value)
				{
					if (std::is_same<T, CBGRAColorOrder>::value)
					{
						mt = _mm_loadl_epi64((const __m128i *)pp); // m = XX XX XX XX XX XX XX XX XX R1 G1 B1 XX R0 G0 B0
						mb = _mm_loadl_epi64((const __m128i *)(pp + scbPredictStride)); // m = XX XX XX XX XX XX XX XX XX R1 G1 B1 XX R0 G0 B0
					}
					else
					{
#ifdef __SSSE3__
						mt = _mm_loadl_epi64((const __m128i *)(pp - 2)); // m = XX XX XX XX XX XX XX XX R1 G1 B1 R0 G0 B0 XX XX
						mb = _mm_loadl_epi64((const __m128i *)(pp + scbPredictStride - 2)); // m = XX XX XX XX XX XX XX XX R1 G1 B1 R0 G0 B0 XX XX
						mt = _mm_shuffle_epi8(mt, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, -1, 4, 3, 2));
						mb = _mm_shuffle_epi8(mb, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, -1, 4, 3, 2));
#endif
					}
					rbt = _mm_and_si128(mt, _mm_set1_epi16(0x00ff)); // 00 XX 00 XX 00 XX 00 XX 00 R1 00 B1 00 R0 00 B0
					rbb = _mm_and_si128(mb, _mm_set1_epi16(0x00ff)); // 00 XX 00 XX 00 XX 00 XX 00 R1 00 B1 00 R0 00 B0
					xgt = _mm_or_si128(_mm_srli_epi16(mt, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 XX 00 ff 00 XX 00 ff 00 G1 00 ff 00 G0
					xgb = _mm_or_si128(_mm_srli_epi16(mb, 8), _mm_set1_epi32(0x00ff0000)); // 00 ff 00 XX 00 ff 00 XX 00 ff 00 G1 00 ff 00 G0
				}
				else if (std::is_same<T, CARGBColorOrder>::value || std::is_same<T, CRGBColorOrder>::value)
				{
					if (std::is_same<T, CARGBColorOrder>::value)
					{
						mt = _mm_loadl_epi64((const __m128i *)pp); // m = XX XX XX XX XX XX XX XX B1 G1 R1 XX B0 G0 R0 XX
						mb = _mm_loadl_epi64((const __m128i *)(pp + scbPredictStride)); // m = XX XX XX XX XX XX XX XX B1 G1 R1 XX B0 G0 R0 XX
					}
					else
					{
#ifdef __SSSE3__
						mt = _mm_loadl_epi64((const __m128i *)(pp - 2)); // m = XX XX XX XX XX XX XX XX B1 G1 R1 B0 G0 R0 XX XX
						mb = _mm_loadl_epi64((const __m128i *)(pp + scbPredictStride - 2)); // m = XX XX XX XX XX XX XX XX B1 G1 R1 B0 G0 R0 XX XX
						mt = _mm_shuffle_epi8(mt, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, -1, 4, 3, 2, -1));
						mb = _mm_shuffle_epi8(mb, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, -1, 4, 3, 2, -1));
#endif
					}
					rbt = _mm_srli_epi16(mt, 8); // 00 XX 00 XX 00 XX 00 XX 00 XX 00 XX 00 B0 00 R0
					rbb = _mm_srli_epi16(mb, 8); // 00 XX 00 XX 00 XX 00 XX 00 XX 00 XX 00 B0 00 R0
					xgt = _mm_or_si128(_mm_and_si128(mt, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 XX 00 ff 00 XX 00 ff 00 XX 00 ff 00 G0 00 ff
					xgb = _mm_or_si128(_mm_and_si128(mb, _mm_set1_epi32(0x00ff0000)), _mm_set1_epi32(0x000000ff)); // 00 XX 00 ff 00 XX 00 ff 00 XX 00 ff 00 G0 00 ff
				}

				__m128i yyt = _mm_add_epi32(_mm_madd_epi16(rbt, rb2y), _mm_madd_epi16(xgt, xg2y));
				__m128i yyb = _mm_add_epi32(_mm_madd_epi16(rbb, rb2y), _mm_madd_epi16(xgb, xg2y));
				yyt = _mm_srli_epi32(yyt, shift);
				yyb = _mm_srli_epi32(yyb, shift);
#ifdef __SSSE3__
				if (F >= CODEFEATURE_SSSE3)
				{
					yyt = _mm_shuffle_epi8(yyt, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, 0));
					yyb = _mm_shuffle_epi8(yyb, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, 0));
				}
				else
#endif
				{
					yyt = _mm_packs_epi32(yyt, yyt);
					yyb = _mm_packs_epi32(yyb, yyb);
					yyt = _mm_packus_epi16(yyt, yyt);
					yyb = _mm_packus_epi16(yyb, yyb);
				}
				*(uint16_t *)y = _mm_cvtsi128_si32(yyt);
				*(uint16_t *)(y + dwYPlaneGrossWidth) = _mm_cvtsi128_si32(yyb);

				__m128i rb = _mm_add_epi16(_mm_add_epi16(rbt, _mm_srli_epi64(rbt, 32)), _mm_add_epi16(rbb, _mm_srli_epi64(rbb, 32)));
				__m128i xg = _mm_add_epi16(_mm_add_epi16(xgt, _mm_srli_epi64(xgt, 32)), _mm_add_epi16(xgb, _mm_srli_epi64(xgb, 32)));
				auto xrgb2uv = [rb, xg, shift](__m128i rb2uv, __m128i xg2uv) -> uint8_t {
					__m128i uv = _mm_add_epi32(_mm_madd_epi16(rb, rb2uv), _mm_madd_epi16(xg, xg2uv));
					uv = _mm_srli_epi32(uv, shift + 2);
					return (uint8_t)_mm_cvtsi128_si32(uv);
				};
				*u = xrgb2uv(rb2u, xg2u);
				*v = xrgb2uv(rb2v, xg2v);

				y += 2;
				u++;
				v++;
			}
		}

		y += dwYPlaneGrossWidth;
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_SSSE3, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
#endif
