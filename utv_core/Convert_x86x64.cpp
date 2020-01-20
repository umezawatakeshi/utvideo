/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
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

// 処理が ULY4 とほぼ同じなのをなんとかしたい
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

// ULY2ToRGB ほどではないが ULY4 とかなり近いのを以下略
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
					uint32_t b1 = _mm_cvtsi128_si32(bbb); // ここまで来て初めて t/b サフィックスという変数命名が破綻していることに気づく
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

//

template<int F, class T, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m128i> VECTORCALL tuned_ConvertPackedYUV422ToPlanarElement(__m128i m0, __m128i m1, __m128i m2, __m128i m3)
{
	__m128i ctl;
	if (std::is_same<T, CYUYVColorOrder>::value)
		ctl = _mm_set_epi8(15, 11, 7, 3, 13, 9, 5, 1, 14, 12, 10, 8, 6, 4, 2, 0);
	else
		ctl = _mm_set_epi8(14, 10, 6, 2, 12, 8, 4, 0, 15, 13, 11, 9, 7, 5, 3, 1);

	m0 = _mm_shuffle_epi8(m0, ctl);
	m1 = _mm_shuffle_epi8(m1, ctl);
	m2 = _mm_shuffle_epi8(m2, ctl);
	m3 = _mm_shuffle_epi8(m3, ctl);

	__m128i yy0 = _mm_unpacklo_epi64(m0, m1);
	__m128i yy1 = _mm_unpacklo_epi64(m2, m3);
	__m128i uv0 = _mm_unpackhi_epi32(m0, m1);
	__m128i uv1 = _mm_unpackhi_epi32(m2, m3);

	__m128i uu = _mm_unpacklo_epi64(uv0, uv1);
	__m128i vv = _mm_unpackhi_epi64(uv0, uv1);

	return { yy0, yy1, uu, vv };
}

template<int F, class T, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m256i> VECTORCALL tuned_ConvertPackedYUV422ToPlanarElement(__m256i m0, __m256i m1, __m256i m2, __m256i m3)
{
	__m256i ctl;
	if (std::is_same<T, CYUYVColorOrder>::value)
		ctl = _mm256_set16_epi8(15, 11, 7, 3, 13, 9, 5, 1, 14, 12, 10, 8, 6, 4, 2, 0);
	else
		ctl = _mm256_set16_epi8(14, 10, 6, 2, 12, 8, 4, 0, 15, 13, 11, 9, 7, 5, 3, 1);

	m0 = _mm256_shuffle_epi8(m0, ctl);
	m1 = _mm256_shuffle_epi8(m1, ctl);
	m2 = _mm256_shuffle_epi8(m2, ctl);
	m3 = _mm256_shuffle_epi8(m3, ctl);

	__m256i yy0 = _mm256_unpacklo_epi64(m0, m1);
	__m256i yy1 = _mm256_unpacklo_epi64(m2, m3);
	__m256i uv0 = _mm256_unpackhi_epi32(m0, m1);
	__m256i uv1 = _mm256_unpackhi_epi32(m2, m3);

	__m256i uu = _mm256_unpacklo_epi64(uv0, uv1);
	__m256i vv = _mm256_unpackhi_epi64(uv0, uv1);

	yy0 = _mm256_permute4x64_epi64(yy0, 0xd8);
	yy1 = _mm256_permute4x64_epi64(yy1, 0xd8);
	uu = _mm256_permutevar8x32_epi32(uu, _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0));
	vv = _mm256_permutevar8x32_epi32(vv, _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0));

	return { yy0, yy1, uu, vv };
}

template<int F, class T, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m128i> tuned_ConvertPackedYUV422ToPlanarElement(const uint8_t* pp)
{
	return tuned_ConvertPackedYUV422ToPlanarElement<F, T>(
		_mm_loadu_si128((const __m128i *)pp),
		_mm_loadu_si128((const __m128i *)(pp + 16)),
		_mm_loadu_si128((const __m128i *)(pp + 32)),
		_mm_loadu_si128((const __m128i *)(pp + 48))
	);
}

template<int F, class T, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m256i> tuned_ConvertPackedYUV422ToPlanarElement(const uint8_t* pp)
{
	return tuned_ConvertPackedYUV422ToPlanarElement<F, T>(
		_mm256_loadu_si256((const __m256i *)pp),
		_mm256_loadu_si256((const __m256i *)(pp + 32)),
		_mm256_loadu_si256((const __m256i *)(pp + 64)),
		_mm256_loadu_si256((const __m256i *)(pp + 96))
	);
}

template<int F, class T, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m128i> tuned_ConvertPackedYUV422ToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	return tuned_ConvertPackedYUV422ToPlanarElement<F, T>(
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48)))
	);
}

template<int F, class T, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m256i> tuned_ConvertPackedYUV422ToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	return tuned_ConvertPackedYUV422ToPlanarElement<F, T>(
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)pp), _mm256_loadu_si256((const __m256i *)(pp - scbStride))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 32)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 32))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 64)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 64))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 96)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 96)))
	);
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - 128; pp += 128)
		{
			auto result = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(pp);
			_mm256_storeu_si256((__m256i *)y, result.y0);
			_mm256_storeu_si256((__m256i *)(y + 32), result.y1);
			_mm256_storeu_si256((__m256i *)u, result.u);
			_mm256_storeu_si256((__m256i *)v, result.v);

			y += 64;
			u += 32;
			v += 32;
		}
#elif defined(__SSSE3__)
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto result = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(pp);
			_mm_storeu_si128((__m128i *)y, result.y0);
			_mm_storeu_si128((__m128i *)(y + 16), result.y1);
			_mm_storeu_si128((__m128i *)u, result.u);
			_mm_storeu_si128((__m128i *)v, result.v);

			y += 32;
			u += 16;
			v += 16;
		}
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			y[0] = pp[T::Y0];
			y[1] = pp[T::Y1];
			u[0] = pp[T::U];
			v[0] = pp[T::V];

			y += 2;
			u += 1;
			v += 1;
		}

		std::fill(y, pYBegin + cbYWidth, y[-1]);
		std::fill(u, pUBegin + cbCWidth, u[-1]);
		std::fill(v, pVBegin + cbCWidth, v[-1]);
	}
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSSE3, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSSE3, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX2, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX2, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

//

template<int F, class T, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR4<__m128i> VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(__m128i yy0, __m128i yy1, __m128i uu, __m128i vv)
{
	__m128i ctl;
	if (std::is_same<T, CYUYVColorOrder>::value)
		ctl = _mm_set_epi8(15, 7, 11, 6, 14, 5, 10, 4, 13, 3, 9, 2, 12, 1, 8, 0);
	else
		ctl = _mm_set_epi8(7, 15, 6, 11, 5, 14, 4, 10, 3, 13, 2, 9, 1, 12, 0, 8);

	__m128i uv0 = _mm_unpacklo_epi32(uu, vv);
	__m128i uv1 = _mm_unpackhi_epi32(uu, vv);

	return {
		_mm_shuffle_epi8(_mm_unpacklo_epi64(yy0, uv0), ctl),
		_mm_shuffle_epi8(_mm_unpackhi_epi64(yy0, uv0), ctl),
		_mm_shuffle_epi8(_mm_unpacklo_epi64(yy1, uv1), ctl),
		_mm_shuffle_epi8(_mm_unpackhi_epi64(yy1, uv1), ctl)
	};
}

template<int F, class T, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR4<__m256i> VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(__m256i yy0, __m256i yy1, __m256i uu, __m256i vv)
{
	__m256i ctl;
	if (std::is_same<T, CYUYVColorOrder>::value)
		ctl = _mm256_set16_epi8(15, 7, 11, 6, 14, 5, 10, 4, 13, 3, 9, 2, 12, 1, 8, 0);
	else
		ctl = _mm256_set16_epi8(7, 15, 6, 11, 5, 14, 4, 10, 3, 13, 2, 9, 1, 12, 0, 8);

	yy0 = _mm256_permute4x64_epi64(yy0, 0xd8);
	yy1 = _mm256_permute4x64_epi64(yy1, 0xd8);
	uu = _mm256_permutevar8x32_epi32(uu, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	vv = _mm256_permutevar8x32_epi32(vv, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));

	__m256i uv0 = _mm256_unpacklo_epi32(uu, vv);
	__m256i uv1 = _mm256_unpackhi_epi32(uu, vv);

	return {
		_mm256_shuffle_epi8(_mm256_unpacklo_epi64(yy0, uv0), ctl),
		_mm256_shuffle_epi8(_mm256_unpackhi_epi64(yy0, uv0), ctl),
		_mm256_shuffle_epi8(_mm256_unpacklo_epi64(yy1, uv1), ctl),
		_mm256_shuffle_epi8(_mm256_unpackhi_epi64(yy1, uv1), ctl)
	};
}

template<int F, class T, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(uint8_t* pp, __m128i yy0, __m128i yy1, __m128i uu, __m128i vv)
{
	auto result = tuned_ConvertPlanarYUV422ToPackedElement<F, T>(yy0, yy1, uu, vv);
	_mm_storeu_si128((__m128i *)pp, result.v0);
	_mm_storeu_si128((__m128i *)(pp + 16), result.v1);
	_mm_storeu_si128((__m128i *)(pp + 32), result.v2);
	_mm_storeu_si128((__m128i *)(pp + 48), result.v3);
}

template<int F, class T, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(uint8_t* pp, __m256i yy0, __m256i yy1, __m256i uu, __m256i vv)
{
	auto result = tuned_ConvertPlanarYUV422ToPackedElement<F, T>(yy0, yy1, uu, vv);
	_mm256_storeu_si256((__m256i *)pp, result.v0);
	_mm256_storeu_si256((__m256i *)(pp + 32), result.v1);
	_mm256_storeu_si256((__m256i *)(pp + 64), result.v2);
	_mm256_storeu_si256((__m256i *)(pp + 96), result.v3);
}

template<int F, class T, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(uint8_t* pp, __m128i yy0, __m128i yy1, __m128i uu, __m128i vv, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarYUV422ToPackedElement<F, T>(yy0, yy1, uu, vv);
	_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(result.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
	_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(result.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
	_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(result.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));
	_mm_storeu_si128((__m128i *)(pp + 48), _mm_add_epi8(result.v3, _mm_loadu_si128((__m128i *)(pp - scbStride + 48))));
}

template<int F, class T, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(uint8_t* pp, __m256i yy0, __m256i yy1, __m256i uu, __m256i vv, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarYUV422ToPackedElement<F, T>(yy0, yy1, uu, vv);
	_mm256_storeu_si256((__m256i *)pp, _mm256_add_epi8(result.v0, _mm256_loadu_si256((__m256i *)(pp - scbStride))));
	_mm256_storeu_si256((__m256i *)(pp + 32), _mm256_add_epi8(result.v1, _mm256_loadu_si256((__m256i *)(pp - scbStride + 32))));
	_mm256_storeu_si256((__m256i *)(pp + 64), _mm256_add_epi8(result.v2, _mm256_loadu_si256((__m256i *)(pp - scbStride + 64))));
	_mm256_storeu_si256((__m256i *)(pp + 96), _mm256_add_epi8(result.v3, _mm256_loadu_si256((__m256i *)(pp - scbStride + 96))));
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	for (auto p = pDstBegin; p != pDstEnd; p += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - 128; pp += 128)
		{
			__m256i yy0 = _mm256_loadu_si256((const __m256i *)y);
			__m256i yy1 = _mm256_loadu_si256((const __m256i *)(y + 32));
			__m256i uu = _mm256_loadu_si256((const __m256i *)u);
			__m256i vv = _mm256_loadu_si256((const __m256i *)v);
			tuned_ConvertPlanarYUV422ToPackedElement<F, T>(pp, yy0, yy1, uu, vv);

			y += 64;
			u += 32;
			v += 32;
		}
#elif defined(__SSSE3__)
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);
			tuned_ConvertPlanarYUV422ToPackedElement<F, T>(pp, yy0, yy1, uu, vv);

			y += 32;
			u += 16;
			v += 16;
		}
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::Y0] = y[0];
			pp[T::Y1] = y[1];
			pp[T::U] = u[0];
			pp[T::V] = v[0];

			y += 2;
			u += 1;
			v += 1;
		}
	}
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSSE3, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSSE3, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX2, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX2, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
#endif

//

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGB<__m128i> tuned_ConvertPackedBGRToPlanarElement(__m128i m0, __m128i m1, __m128i m2)
{
	__m128i ctl = _mm_set_epi8(13, 10, 7, 4, 1, 14, 11, 8, 5, 2, 15, 12, 9, 6, 3, 0);

	m0 = _mm_shuffle_epi8(m0, ctl); // G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
	m1 = _mm_shuffle_epi8(m1, ctl); // R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	m2 = _mm_shuffle_epi8(m2, ctl); // Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

	__m128i n0 = _mm_alignr_epi8(m0, m2, 6); // B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb
	__m128i n1 = _mm_alignr_epi8(m2, m1, 6); // Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
	__m128i n2 = _mm_alignr_epi8(m1, m0, 6); // Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0

	__m128i bb = _mm_alignr_epi8(n1, n0, 5); // Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb
	__m128i gg = _mm_alignr_epi8(n0, n2, 5); // Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	__m128i rr = _mm_alignr_epi8(n2, n1, 5); // R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5

	bb = _mm_alignr_epi8(bb, bb, 5);
	rr = _mm_alignr_epi8(rr, rr, 11);

	__m128i ggtmp = NeedOffset ? _mm_add_epi8(gg, _mm_set1_epi8((char)0x80)) : gg;
	bb = _mm_sub_epi8(bb, ggtmp);
	rr = _mm_sub_epi8(rr, ggtmp);

	return { gg, bb, rr };
}

template<int F, bool NeedOffset = true, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGB<__m256i> tuned_ConvertPackedBGRToPlanarElement(__m256i m0, __m256i m1, __m256i m2)
{
	__m256i ctl = _mm256_set16_epi8(13, 10, 7, 4, 1, 14, 11, 8, 5, 2, 15, 12, 9, 6, 3, 0);

	__m256i o0 = _mm256_permute2x128_si256(m0, m1, 0x30);
	__m256i o1 = _mm256_permute2x128_si256(m2, m0, 0x03);
	__m256i o2 = _mm256_permute2x128_si256(m1, m2, 0x30);

	o0 = _mm256_shuffle_epi8(o0, ctl); // G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
	o1 = _mm256_shuffle_epi8(o1, ctl); // R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	o2 = _mm256_shuffle_epi8(o2, ctl); // Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

	__m256i n0 = _mm256_alignr_epi8(o0, o2, 6); // B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb
	__m256i n1 = _mm256_alignr_epi8(o2, o1, 6); // Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
	__m256i n2 = _mm256_alignr_epi8(o1, o0, 6); // Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0

	__m256i bb = _mm256_alignr_epi8(n1, n0, 5); // Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb
	__m256i gg = _mm256_alignr_epi8(n0, n2, 5); // Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	__m256i rr = _mm256_alignr_epi8(n2, n1, 5); // R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5

	bb = _mm256_alignr_epi8(bb, bb, 5);
	rr = _mm256_alignr_epi8(rr, rr, 11);

	__m256i ggtmp = NeedOffset ? _mm256_add_epi8(gg, _mm256_set1_epi8((char)0x80)) : gg;
	bb = _mm256_sub_epi8(bb, ggtmp);
	rr = _mm256_sub_epi8(rr, ggtmp);

	return { gg, bb, rr };
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler> /* 最適化が有効な場合、返した a を触らなければ a を計算する命令は生成されないので、やはり A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE VECTOR_RGBA<__m128i> VECTORCALL tuned_ConvertPackedRGBXToPlanarElement(__m128i m0, __m128i m1, __m128i m2, __m128i m3)
{
	__m128i ctl;
	if (std::is_same<T, CBGRAColorOrder>::value)
		ctl = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	else
		ctl = _mm_set_epi8(12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3);

	m0 = _mm_shuffle_epi8(m0, ctl); // A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	m1 = _mm_shuffle_epi8(m1, ctl);
	m2 = _mm_shuffle_epi8(m2, ctl);
	m3 = _mm_shuffle_epi8(m3, ctl);

	__m128i gb0 = _mm_unpacklo_epi32(m0, m1);
	__m128i gb1 = _mm_unpacklo_epi32(m2, m3);
	__m128i ar0 = _mm_unpackhi_epi32(m0, m1);
	__m128i ar1 = _mm_unpackhi_epi32(m2, m3);

	__m128i gg = _mm_unpackhi_epi64(gb0, gb1);
	__m128i bb = _mm_unpacklo_epi64(gb0, gb1);
	__m128i rr = _mm_unpacklo_epi64(ar0, ar1);
	__m128i aa = _mm_unpackhi_epi64(ar0, ar1);

	__m128i ggtmp = NeedOffset ? _mm_add_epi8(gg, _mm_set1_epi8((char)0x80)) : gg;
	bb = _mm_sub_epi8(bb, ggtmp);
	rr = _mm_sub_epi8(rr, ggtmp);

	return { gg, bb, rr, aa };
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m256i> VECTORCALL tuned_ConvertPackedRGBXToPlanarElement(__m256i m0, __m256i m1, __m256i m2, __m256i m3)
{
	__m256i ctl;
	if (std::is_same<T, CBGRAColorOrder>::value)
		ctl = _mm256_set16_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	else
		ctl = _mm256_set16_epi8(12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3);

	m0 = _mm256_shuffle_epi8(m0, ctl); // A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	m1 = _mm256_shuffle_epi8(m1, ctl);
	m2 = _mm256_shuffle_epi8(m2, ctl);
	m3 = _mm256_shuffle_epi8(m3, ctl);

	__m256i gb0 = _mm256_unpacklo_epi32(m0, m1);
	__m256i gb1 = _mm256_unpacklo_epi32(m2, m3);
	__m256i ar0 = _mm256_unpackhi_epi32(m0, m1);
	__m256i ar1 = _mm256_unpackhi_epi32(m2, m3);

	__m256i gg = _mm256_unpackhi_epi64(gb0, gb1);
	__m256i bb = _mm256_unpacklo_epi64(gb0, gb1);
	__m256i rr = _mm256_unpacklo_epi64(ar0, ar1);
	__m256i aa = _mm256_unpackhi_epi64(ar0, ar1);

	gg = _mm256_permutevar8x32_epi32(gg, _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0));
	bb = _mm256_permutevar8x32_epi32(bb, _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0));
	rr = _mm256_permutevar8x32_epi32(rr, _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0));
	aa = _mm256_permutevar8x32_epi32(aa, _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0));

	__m256i ggtmp = NeedOffset ? _mm256_add_epi8(gg, _mm256_set1_epi8((char)0x80)) : gg;
	bb = _mm256_sub_epi8(bb, ggtmp);
	rr = _mm256_sub_epi8(rr, ggtmp);

	return { gg, bb, rr, aa };
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m128i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp)
{
	return tuned_ConvertPackedRGBXToPlanarElement<F, T, NeedOffset>(
		_mm_loadu_si128((const __m128i *)pp),
		_mm_loadu_si128((const __m128i *)(pp + 16)),
		_mm_loadu_si128((const __m128i *)(pp + 32)),
		_mm_loadu_si128((const __m128i *)(pp + 48))
	);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m256i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp)
{
	return tuned_ConvertPackedRGBXToPlanarElement<F, T, NeedOffset>(
		_mm256_loadu_si256((const __m256i *)pp),
		_mm256_loadu_si256((const __m256i *)(pp + 32)),
		_mm256_loadu_si256((const __m256i *)(pp + 64)),
		_mm256_loadu_si256((const __m256i *)(pp + 96))
	);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m128i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp)
{
	auto ret = tuned_ConvertPackedBGRToPlanarElement<F, NeedOffset>(
		_mm_loadu_si128((const __m128i *)pp),
		_mm_loadu_si128((const __m128i *)(pp + 16)),
		_mm_loadu_si128((const __m128i *)(pp + 32))
	);
	return { ret.g, ret.b, ret.r, _mm_set1_epi8((char)0xff) };
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m256i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp)
{
	auto ret = tuned_ConvertPackedBGRToPlanarElement<F, NeedOffset>(
		_mm256_loadu_si256((const __m256i *)pp),
		_mm256_loadu_si256((const __m256i *)(pp + 32)),
		_mm256_loadu_si256((const __m256i *)(pp + 64))
	);
	return { ret.g, ret.b, ret.r, _mm256_set1_epi8((char)0xff) };
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m128i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	return tuned_ConvertPackedRGBXToPlanarElement<F, T, NeedOffset>(
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48)))
	);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m256i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	return tuned_ConvertPackedRGBXToPlanarElement<F, T, NeedOffset>(
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)pp), _mm256_loadu_si256((const __m256i *)(pp - scbStride))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 32)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 32))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 64)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 64))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 96)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 96)))
	);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m128i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	auto ret = tuned_ConvertPackedBGRToPlanarElement<F, NeedOffset>(
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
		_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32)))
	);
	return { ret.g, ret.b, ret.r, _mm_set1_epi8((char)0xff) };
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m256i> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	auto ret = tuned_ConvertPackedBGRToPlanarElement<F, NeedOffset>(
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)pp), _mm256_loadu_si256((const __m256i *)(pp - scbStride))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 32)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 32))),
		_mm256_sub_epi8(_mm256_loadu_si256((const __m256i *)(pp + 64)), _mm256_loadu_si256((const __m256i *)(pp - scbStride + 64)))
	);
	return { ret.g, ret.b, ret.r, _mm256_set1_epi8((char)0xff) };
}

template<int F, class T, bool A>
static inline void tuned_ConvertRGBXToULRX(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth, pABegin += cbPlaneWidth)
	{
		auto r = pRBegin;
		auto g = pGBegin;
		auto b = pBBegin;
		auto a = pABegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - T::BYPP * 32; pp += T::BYPP * 32)
		{
			auto result = tuned_ConvertPackedRGBXToPlanarElement<F, T>(pp);
			_mm256_storeu_si256((__m256i *)b, result.b);
			_mm256_storeu_si256((__m256i *)g, result.g);
			_mm256_storeu_si256((__m256i *)r, result.r);
			if (A)
				_mm256_storeu_si256((__m256i *)a, result.a);

			b += 32;
			g += 32;
			r += 32;
			if (A)
				a += 32;
		}
#elif defined(__SSSE3__)
		for (; pp <= p + cbWidth - T::BYPP * 16; pp += T::BYPP * 16)
		{
			auto result = tuned_ConvertPackedRGBXToPlanarElement<F, T>(pp);
			_mm_storeu_si128((__m128i *)b, result.b);
			_mm_storeu_si128((__m128i *)g, result.g);
			_mm_storeu_si128((__m128i *)r, result.r);
			if (A)
				_mm_storeu_si128((__m128i *)a, result.a);

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}
#endif

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			*g = pp[T::G];
			*b = pp[T::B] - pp[T::G] + 0x80;
			*r = pp[T::R] - pp[T::G] + 0x80;
			if (A)
				*a = pp[T::A];

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}

		std::fill(g, pGBegin + cbPlaneWidth, g[-1]);
		std::fill(b, pBBegin + cbPlaneWidth, b[-1]);
		std::fill(r, pRBegin + cbPlaneWidth, r[-1]);
		if (A)
			std::fill(a, pABegin + cbPlaneWidth, a[-1]);
	}
}

template<int F, class T>
void tuned_ConvertRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	tuned_ConvertRGBXToULRX<F, T, false>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, cbPlaneWidth);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	tuned_ConvertRGBXToULRX<F, T, true>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, cbPlaneWidth);
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSSE3, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX2, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

//

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR3<__m128i> tuned_ConvertPlanarBGRToPackedElement(__m128i gg, __m128i bb, __m128i rr)
{
	__m128i ctl = _mm_set_epi8(5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11, 0);

	__m128i ggtmp = NeedOffset ? _mm_add_epi8(gg, _mm_set1_epi8((char)0x80)) : gg;
	bb = _mm_add_epi8(bb, ggtmp);
	rr = _mm_add_epi8(rr, ggtmp);

	rr = _mm_alignr_epi8(rr, rr, 5);  // R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
	bb = _mm_alignr_epi8(bb, bb, 11); // Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

	__m128i m0 = _mm_alignr_epi8(gg, rr, 11); // Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
	__m128i m1 = _mm_alignr_epi8(rr, bb, 11); // Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
	__m128i m2 = _mm_alignr_epi8(bb, gg, 11); // B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb

	__m128i n0 = _mm_alignr_epi8(m0, m2, 10); // G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
	__m128i n1 = _mm_alignr_epi8(m1, m0, 10); // R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	__m128i n2 = _mm_alignr_epi8(m2, m1, 10); // Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

	return {
		_mm_shuffle_epi8(n0, ctl),
		_mm_shuffle_epi8(n1, ctl),
		_mm_shuffle_epi8(n2, ctl)
	};
}

template<int F, bool NeedOffset = true, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR3<__m256i> tuned_ConvertPlanarBGRToPackedElement(__m256i gg, __m256i bb, __m256i rr)
{
	__m256i ctl = _mm256_set16_epi8(5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11, 0);

	__m256i ggtmp = NeedOffset ? _mm256_add_epi8(gg, _mm256_set1_epi8((char)0x80)) : gg;
	bb = _mm256_add_epi8(bb, ggtmp);
	rr = _mm256_add_epi8(rr, ggtmp);

	rr = _mm256_alignr_epi8(rr, rr, 5);  // R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
	bb = _mm256_alignr_epi8(bb, bb, 11); // Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

	__m256i m0 = _mm256_alignr_epi8(gg, rr, 11); // Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
	__m256i m1 = _mm256_alignr_epi8(rr, bb, 11); // Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
	__m256i m2 = _mm256_alignr_epi8(bb, gg, 11); // B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb

	__m256i n0 = _mm256_alignr_epi8(m0, m2, 10); // G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
	__m256i n1 = _mm256_alignr_epi8(m1, m0, 10); // R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	__m256i n2 = _mm256_alignr_epi8(m2, m1, 10); // Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

	__m256i o0 = _mm256_shuffle_epi8(n0, ctl);
	__m256i o1 = _mm256_shuffle_epi8(n1, ctl);
	__m256i o2 = _mm256_shuffle_epi8(n2, ctl);

	return {
		_mm256_permute2x128_si256(o0, o1, 0x20),
		_mm256_permute2x128_si256(o2, o0, 0x30),
		_mm256_permute2x128_si256(o1, o2, 0x31)
	};
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE VECTOR4<__m128i> VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(__m128i gg, __m128i bb, __m128i rr, __m128i aa)
{
	__m128i ctl;
	if (std::is_same<T, CBGRAColorOrder>::value)
		ctl = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	else
		ctl = _mm_set_epi8(3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12);

	__m128i ggtmp = NeedOffset ? _mm_add_epi8(gg, _mm_set1_epi8((char)0x80)) : gg;
	bb = _mm_add_epi8(bb, ggtmp);
	rr = _mm_add_epi8(rr, ggtmp);

	__m128i gb0 = _mm_unpacklo_epi32(bb, gg); // G7 G6 G5 G4 B7 B6 B5 B4 G3 G2 G1 G0 B3 B2 B1 B0
	__m128i gb1 = _mm_unpackhi_epi32(bb, gg);
	__m128i ar0 = _mm_unpacklo_epi32(rr, aa);
	__m128i ar1 = _mm_unpackhi_epi32(rr, aa);

	__m128i m0 = _mm_unpacklo_epi64(gb0, ar0); // A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	__m128i m1 = _mm_unpackhi_epi64(gb0, ar0);
	__m128i m2 = _mm_unpacklo_epi64(gb1, ar1);
	__m128i m3 = _mm_unpackhi_epi64(gb1, ar1);

	return {
		_mm_shuffle_epi8(m0, ctl),
		_mm_shuffle_epi8(m1, ctl),
		_mm_shuffle_epi8(m2, ctl),
		_mm_shuffle_epi8(m3, ctl)
	};
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE VECTOR4<__m256i> VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(__m256i gg, __m256i bb, __m256i rr, __m256i aa)
{
	__m256i ctl;
	if (std::is_same<T, CBGRAColorOrder>::value)
		ctl = _mm256_set16_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	else
		ctl = _mm256_set16_epi8(3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12);

	__m256i ggtmp = NeedOffset ? _mm256_add_epi8(gg, _mm256_set1_epi8((char)0x80)) : gg;
	bb = _mm256_add_epi8(bb, ggtmp);
	rr = _mm256_add_epi8(rr, ggtmp);

	gg = _mm256_permutevar8x32_epi32(gg, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	bb = _mm256_permutevar8x32_epi32(bb, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	rr = _mm256_permutevar8x32_epi32(rr, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	aa = _mm256_permutevar8x32_epi32(aa, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0)); // aa に定数を渡した場合でも VPERMD してしまうが、その場合でもループ外に出されるので許容することにする。Clang なら VPERMD せずに単なる定数ロードに変換してくれる。

	__m256i gb0 = _mm256_unpacklo_epi32(bb, gg); // G7 G6 G5 G4 B7 B6 B5 B4 G3 G2 G1 G0 B3 B2 B1 B0
	__m256i gb1 = _mm256_unpackhi_epi32(bb, gg);
	__m256i ar0 = _mm256_unpacklo_epi32(rr, aa);
	__m256i ar1 = _mm256_unpackhi_epi32(rr, aa);

	__m256i m0 = _mm256_unpacklo_epi64(gb0, ar0); // A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	__m256i m1 = _mm256_unpackhi_epi64(gb0, ar0);
	__m256i m2 = _mm256_unpacklo_epi64(gb1, ar1);
	__m256i m3 = _mm256_unpackhi_epi64(gb1, ar1);

	return {
		_mm256_shuffle_epi8(m0, ctl),
		_mm256_shuffle_epi8(m1, ctl),
		_mm256_shuffle_epi8(m2, ctl),
		_mm256_shuffle_epi8(m3, ctl)
	};
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m128i gg, __m128i bb, __m128i rr, __m128i aa)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, T, NeedOffset>(gg, bb, rr, aa);
	_mm_storeu_si128((__m128i *)pp, result.v0);
	_mm_storeu_si128((__m128i *)(pp + 16), result.v1);
	_mm_storeu_si128((__m128i *)(pp + 32), result.v2);
	_mm_storeu_si128((__m128i *)(pp + 48), result.v3);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m256i gg, __m256i bb, __m256i rr, __m256i aa)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, T, NeedOffset>(gg, bb, rr, aa);
	_mm256_storeu_si256((__m256i *)pp, result.v0);
	_mm256_storeu_si256((__m256i *)(pp + 32), result.v1);
	_mm256_storeu_si256((__m256i *)(pp + 64), result.v2);
	_mm256_storeu_si256((__m256i *)(pp + 96), result.v3);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m128i gg, __m128i bb, __m128i rr, __m128i aa)
{
	auto result = tuned_ConvertPlanarBGRToPackedElement<F, NeedOffset>(gg, bb, rr);
	_mm_storeu_si128((__m128i *)pp, result.v0);
	_mm_storeu_si128((__m128i *)(pp + 16), result.v1);
	_mm_storeu_si128((__m128i *)(pp + 32), result.v2);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m256i gg, __m256i bb, __m256i rr, __m256i aa)
{
	auto result = tuned_ConvertPlanarBGRToPackedElement<F, NeedOffset>(gg, bb, rr);
	_mm256_storeu_si256((__m256i *)pp, result.v0);
	_mm256_storeu_si256((__m256i *)(pp + 32), result.v1);
	_mm256_storeu_si256((__m256i *)(pp + 64), result.v2);
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m128i gg, __m128i bb, __m128i rr, __m128i aa, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, T, NeedOffset>(gg, bb, rr, aa);
	_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(result.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
	_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(result.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
	_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(result.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));
	_mm_storeu_si128((__m128i *)(pp + 48), _mm_add_epi8(result.v3, _mm_loadu_si128((__m128i *)(pp - scbStride + 48))));
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<T::BYPP == 4>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m256i gg, __m256i bb, __m256i rr, __m256i aa, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, T, NeedOffset>(gg, bb, rr, aa);
	_mm256_storeu_si256((__m256i *)pp, _mm256_add_epi8(result.v0, _mm256_loadu_si256((__m256i *)(pp - scbStride))));
	_mm256_storeu_si256((__m256i *)(pp + 32), _mm256_add_epi8(result.v1, _mm256_loadu_si256((__m256i *)(pp - scbStride + 32))));
	_mm256_storeu_si256((__m256i *)(pp + 64), _mm256_add_epi8(result.v2, _mm256_loadu_si256((__m256i *)(pp - scbStride + 64))));
	_mm256_storeu_si256((__m256i *)(pp + 96), _mm256_add_epi8(result.v3, _mm256_loadu_si256((__m256i *)(pp - scbStride + 96))));
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m128i gg, __m128i bb, __m128i rr, __m128i aa, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarBGRToPackedElement<F, NeedOffset>(gg, bb, rr);
	_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(result.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
	_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(result.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
	_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(result.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));
}

template<int F, class T, bool NeedOffset = true, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler, typename std::enable_if<F == CODEFEATURE_AVX2>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, __m256i gg, __m256i bb, __m256i rr, __m256i aa, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarBGRToPackedElement<F, NeedOffset>(gg, bb, rr);
	_mm256_storeu_si256((__m256i *)pp, _mm256_add_epi8(result.v0, _mm256_loadu_si256((__m256i *)(pp - scbStride))));
	_mm256_storeu_si256((__m256i *)(pp + 32), _mm256_add_epi8(result.v1, _mm256_loadu_si256((__m256i *)(pp - scbStride + 32))));
	_mm256_storeu_si256((__m256i *)(pp + 64), _mm256_add_epi8(result.v2, _mm256_loadu_si256((__m256i *)(pp - scbStride + 64))));
}

template<int F, class T, bool A>
static inline void tuned_ConvertULRXToRGBX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	for (auto p = pDstBegin; p != pDstEnd; p += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth, pABegin += cbPlaneWidth)
	{
		auto r = pRBegin;
		auto g = pGBegin;
		auto b = pBBegin;
		auto a = pABegin;

		auto pp = p;

#if defined(__AVX2__)
		for (; pp <= p + cbWidth - T::BYPP * 32; pp += T::BYPP * 32)
		{
			__m256i gg = _mm256_loadu_si256((const __m256i *)g);
			__m256i bb = _mm256_loadu_si256((const __m256i *)b);
			__m256i rr = _mm256_loadu_si256((const __m256i *)r);
			tuned_ConvertPlanarRGBXToPackedElement<F, T>(pp, gg, bb, rr, A ? _mm256_loadu_si256((const __m256i *)a) : _mm256_set1_epi8((char)0xff));

			b += 32;
			g += 32;
			r += 32;
			if (A)
				a += 32;
		}
#elif defined(__SSSE3__)
		for (; pp <= p + cbWidth - T::BYPP * 16; pp += T::BYPP * 16)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			tuned_ConvertPlanarRGBXToPackedElement<F, T>(pp, gg, bb, rr, A ? _mm_loadu_si128((const __m128i *)a) : _mm_set1_epi8((char)0xff));

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}
#endif

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			pp[T::G] = *g;
			pp[T::B] = *b + *g - 0x80;
			pp[T::R] = *r + *g - 0x80;
			if (A)
				pp[T::A] = *a;
			else if (T::HAS_ALPHA)
				pp[T::A] = 0xff;

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertULRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	tuned_ConvertULRXToRGBX<F, T, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride, cbPlaneWidth);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	tuned_ConvertULRXToRGBX<F, T, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride, cbPlaneWidth);
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSSE3, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin,  size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

#ifdef GENERATE_AVX2
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX2, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
#endif

//

static inline __m128i _mm_Convert16To10Fullrange(__m128i x)
{
	return _mm_srli_epi16(_mm_sub_epi16(_mm_add_epi16(x, _mm_set1_epi16(1 << 5)), _mm_srli_epi16(x, 10)), 6);
}

static inline __m128i _mm_Convert10To16Fullrange(__m128i x)
{
	__m128i tmp = _mm_slli_epi16(x, 6);
	return _mm_or_si128(tmp, _mm_srli_epi16(tmp, 10));
}


template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler> /* 最適化が有効な場合、返した a を触らなければ a を計算する命令は生成されないので、やはり A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE VECTOR_RGBA<__m128i> VECTORCALL tuned_ConvertB64aToPlanarElement10(__m128i m0, __m128i m1, __m128i m2, __m128i m3)
{
	__m128i ctl = _mm_set_epi8(8, 9, 0, 1, 10, 11, 2, 3, 12, 13, 4, 5, 14, 15, 6, 7);

	m0 = _mm_shuffle_epi8(m0, ctl); // A1 A0 R1 R0 G1 G0 B1 B0
	m1 = _mm_shuffle_epi8(m1, ctl);
	m2 = _mm_shuffle_epi8(m2, ctl);
	m3 = _mm_shuffle_epi8(m3, ctl);

	__m128i gb0 = _mm_unpacklo_epi32(m0, m1);
	__m128i gb1 = _mm_unpacklo_epi32(m2, m3);
	__m128i ar0 = _mm_unpackhi_epi32(m0, m1);
	__m128i ar1 = _mm_unpackhi_epi32(m2, m3);

	__m128i gg = _mm_Convert16To10Fullrange(_mm_unpackhi_epi64(gb0, gb1));
	__m128i ggtmp = NeedOffset ? _mm_add_epi16(gg, _mm_set1_epi16(0x200)) : gg;
	__m128i bb = _mm_and_si128(_mm_sub_epi16(_mm_Convert16To10Fullrange(_mm_unpacklo_epi64(gb0, gb1)), ggtmp), _mm_set1_epi16(0x3ff));
	__m128i rr = _mm_and_si128(_mm_sub_epi16(_mm_Convert16To10Fullrange(_mm_unpacklo_epi64(ar0, ar1)), ggtmp), _mm_set1_epi16(0x3ff));
	__m128i aa = _mm_Convert16To10Fullrange(_mm_unpackhi_epi64(ar0, ar1));

	return{ gg, bb, rr, aa };
}

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m128i> tuned_ConvertB64aToPlanarElement10(const uint8_t* pp)
{
	return tuned_ConvertB64aToPlanarElement10<F, NeedOffset>(
		_mm_loadu_si128((const __m128i *)pp),
		_mm_loadu_si128((const __m128i *)(pp + 16)),
		_mm_loadu_si128((const __m128i *)(pp + 32)),
		_mm_loadu_si128((const __m128i *)(pp + 48))
	);
}

#if 0
template<int F>
static inline void tuned_ConvertB48rToUQRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

template<int F, bool A>
static inline void tuned_ConvertRGBXToUQRX(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *a = (uint16_t *)pABegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto result = tuned_ConvertB64aToPlanarElement10<F, true>(pp);

			_mm_storeu_si128((__m128i *)b, result.b);
			_mm_storeu_si128((__m128i *)g, result.g);
			_mm_storeu_si128((__m128i *)r, result.r);
			if (A)
				_mm_storeu_si128((__m128i *)a, result.a);

			b += 8;
			g += 8;
			r += 8;
			if (A)
				a += 8;
		}
#endif

		for (; pp < p + cbWidth; pp += 8)
		{
			uint16_t *ppp = (uint16_t *)pp;

			uint16_t gg = Convert16To10Fullrange(btoh16(ppp[2]));
			uint16_t bb = Convert16To10Fullrange(btoh16(ppp[3]));
			uint16_t rr = Convert16To10Fullrange(btoh16(ppp[1]));

			*g++ = gg;
			*b++ = (bb - gg + 0x200) & 0x3ff;
			*r++ = (rr - gg + 0x200) & 0x3ff;

			if (A)
			{
				uint16_t aa = Convert16To10Fullrange(btoh16(ppp[0]));
				*a++ = aa;
			}
		}
	}

}

template<int F, class T>
void tuned_ConvertRGBToUQRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
#if 0
	if (std::is_same<T, CB48rColorOrder>::value)
		tuned_ConvertB48rToUQRG<F>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride);
	else
#endif
	static_assert(std::is_same<T, CB64aColorOrder>::value, "tuned_ConvertB48rToUQRG is not implemented.");
	tuned_ConvertRGBXToUQRX<F, false>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertB64aToUQRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertRGBXToUQRX<F, true>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

#ifdef GENERATE_SSSE3
//template void tuned_ConvertRGBToUQRG<CODEFEATURE_SSSE3, CB48rColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToUQRG<CODEFEATURE_SSSE3, CB64aColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertB64aToUQRA<CODEFEATURE_SSSE3>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
//template void tuned_ConvertRGBToUQRG<CODEFEATURE_AVX1, CB48rColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToUQRG<CODEFEATURE_AVX1, CB64aColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertB64aToUQRA<CODEFEATURE_AVX1>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR4<__m128i> VECTORCALL tuned_ConvertPlanarRGBXToB64aElement10(__m128i gg, __m128i bb, __m128i rr, __m128i aa)
{
	__m128i ctl = _mm_set_epi8(2, 3, 6, 7, 10, 11, 14, 15, 0, 1, 4, 5, 8, 9, 12, 13);

	__m128i ggtmp = NeedOffset ? _mm_add_epi16(gg, _mm_set1_epi16(0x200)) : gg;
	gg = _mm_Convert10To16Fullrange(gg);
	bb = _mm_Convert10To16Fullrange(_mm_add_epi16(bb, ggtmp));
	rr = _mm_Convert10To16Fullrange(_mm_add_epi16(rr, ggtmp));
	aa = _mm_Convert10To16Fullrange(aa); // tuned_ConvertPlanarRGBXToPackedElement と同様、 aa が定数の場合でもこの行はそこまで問題にならない。

	__m128i gb0 = _mm_unpacklo_epi32(bb, gg); // G3 G2 B3 B2 G1 G0 B1 B0
	__m128i gb1 = _mm_unpackhi_epi32(bb, gg);
	__m128i ar0 = _mm_unpacklo_epi32(rr, aa);
	__m128i ar1 = _mm_unpackhi_epi32(rr, aa);

	__m128i m0 = _mm_unpacklo_epi64(gb0, ar0); // A1 A0 R1 R0 G1 G0 B1 B0
	__m128i m1 = _mm_unpackhi_epi64(gb0, ar0);
	__m128i m2 = _mm_unpacklo_epi64(gb1, ar1);
	__m128i m3 = _mm_unpackhi_epi64(gb1, ar1);

	return {
		_mm_shuffle_epi8(m0, ctl),
		_mm_shuffle_epi8(m1, ctl),
		_mm_shuffle_epi8(m2, ctl),
		_mm_shuffle_epi8(m3, ctl)
	};
}

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToB64aElement10(uint8_t* pp, __m128i gg, __m128i bb, __m128i rr, __m128i aa)
{
	auto result = tuned_ConvertPlanarRGBXToB64aElement10<F, NeedOffset>(gg, bb, rr, aa);
	_mm_storeu_si128((__m128i *)pp, result.v0);
	_mm_storeu_si128((__m128i *)(pp + 16), result.v1);
	_mm_storeu_si128((__m128i *)(pp + 32), result.v2);
	_mm_storeu_si128((__m128i *)(pp + 48), result.v3);
}

#if 0
template<int F>
static inline void tuned_ConvertUQRGToB48r(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
#endif

template<int F, bool A>
static inline void tuned_ConvertUQRXToRGBX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	const uint16_t *r = (const uint16_t *)pRBegin;
	const uint16_t *g = (const uint16_t *)pGBegin;
	const uint16_t *b = (const uint16_t *)pBBegin;
	const uint16_t *a = (const uint16_t *)pABegin;

	__m128i aa;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			if (A)
				aa = _mm_loadu_si128((const __m128i *)a);
			else
				aa = _mm_set1_epi16((short)0xffff);

			tuned_ConvertPlanarRGBXToB64aElement10<F, true>(pp, gg, bb, rr, aa);

			b += 8;
			g += 8;
			r += 8;
			if (A)
				a += 8;
		}
#endif

		for (; pp < p + cbWidth; pp += 8)
		{
			uint16_t *ppp = (uint16_t *)pp;

			ppp[2] = htob16(Convert10To16Fullrange(*g));
			ppp[3] = htob16(Convert10To16Fullrange(*b + *g - 0x200));
			ppp[1] = htob16(Convert10To16Fullrange(*r + *g - 0x200));
			g++; b++; r++;
			if (A)
			{
				ppp[0] = htob16(Convert10To16Fullrange(*a));
				a++;
			}
			else
				ppp[0] = 0xffff;
		}
	}
}

template<int F, class T>
void tuned_ConvertUQRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
#if 0
	if (std::is_same<T, CB48rColorOrder>::value)
		tuned_ConvertUQRGToB48r<F>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin,cbWidth, scbStride);
	else
#endif
	static_assert(std::is_same<T, CB64aColorOrder>::value, "tuned_ConvertUQRGToB48r is not implemented.");
	tuned_ConvertUQRXToRGBX<F, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertUQRAToB64a(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertUQRXToRGBX<F, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}


#ifdef GENERATE_SSSE3
//template void tuned_ConvertUQRGToRGB<CODEFEATURE_SSSE3, CB48rColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRGToRGB<CODEFEATURE_SSSE3, CB64aColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRAToB64a<CODEFEATURE_SSSE3>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
//template void tuned_ConvertUQRGToRGB<CODEFEATURE_AVX1, CB48rColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRGToRGB<CODEFEATURE_AVX1, CB64aColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRAToB64a<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGB<__m128i> VECTORCALL tuned_ConvertR210ToPlanarElement10(__m128i m0, __m128i m1)
{
	__m128i rb0 = _mm_shuffle_epi8(m0, _mm_set_epi8(12, 13, 8, 9, 4, 5, 0, 1, 14, 15, 10, 11, 6, 7, 2, 3)); // XXRRRRRRRRRRXXXX|XXXXXXBBBBBBBBBB
	__m128i rb1 = _mm_shuffle_epi8(m1, _mm_set_epi8(12, 13, 8, 9, 4, 5, 0, 1, 14, 15, 10, 11, 6, 7, 2, 3));
	__m128i g00 = _mm_shuffle_epi8(m0, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 13, 14, 9, 10, 5, 6, 1, 2)); // XXXXGGGGGGGGGGXX
	__m128i g01 = _mm_shuffle_epi8(m1, _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 13, 14, 9, 10, 5, 6, 1, 2));
	__m128i bb = _mm_unpacklo_epi64(rb0, rb1);
	__m128i rr = _mm_srli_epi16(_mm_unpackhi_epi64(rb0, rb1), 4);
	__m128i gg = _mm_srli_epi16(_mm_unpacklo_epi64(g00, g01), 2);
	__m128i ggtmp = NeedOffset ? _mm_add_epi16(gg, _mm_set1_epi16(0x200)) : gg;
	bb = _mm_and_si128(_mm_sub_epi16(bb, ggtmp), _mm_set1_epi16(0x3ff));
	rr = _mm_and_si128(_mm_sub_epi16(rr, ggtmp), _mm_set1_epi16(0x3ff));
	gg = _mm_and_si128(gg, _mm_set1_epi16(0x3ff));

	return { gg, bb, rr };
}

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGB<__m128i> tuned_ConvertR210ToPlanarElement10(const uint8_t* pp)
{
	return tuned_ConvertR210ToPlanarElement10<F, NeedOffset>(
		_mm_loadu_si128((const __m128i *)pp),
		_mm_loadu_si128((const __m128i *)(pp + 16))
	);
}

template<int F>
void tuned_ConvertR210ToUQRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride)
{
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + nWidth * 4;
		const uint8_t *p = pStrideBegin;

#ifdef __SSSE3__
		for (; p <= pStrideEnd - 32; p += 32)
		{
			auto result = tuned_ConvertR210ToPlanarElement10<F>(p);
			_mm_storeu_si128((__m128i*)g, result.g);
			_mm_storeu_si128((__m128i*)b, result.b);
			_mm_storeu_si128((__m128i*)r, result.r);

			g += 8;
			b += 8;
			r += 8;
		}
#endif

		for (; p < pStrideEnd; p += 4)
		{
			uint32_t val = btoh32(*(const uint32_t *)p);
			uint16_t gg = val >> 10;
			uint16_t bb = val - gg + 0x200;
			uint16_t rr = (val >> 20) - gg + 0x200;

			*g++ = gg & 0x3ff;
			*b++ = bb & 0x3ff;
			*r++ = rr & 0x3ff;
		}
	}
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertR210ToUQRG<CODEFEATURE_SSSE3>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertR210ToUQRG<CODEFEATURE_AVX1>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
#endif

//

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE VECTOR2<__m128i> VECTORCALL tuned_ConvertPlanarRGBXToR210Element10(__m128i gg, __m128i bb, __m128i rr)
{
	__m128i ggtmp = NeedOffset ? _mm_add_epi16(gg, _mm_set1_epi16(0x200)) : gg;
	bb = _mm_and_si128(_mm_add_epi16(bb, ggtmp), _mm_set1_epi16(0x3ff));
	rr = _mm_and_si128(_mm_add_epi16(rr, ggtmp), _mm_set1_epi16(0x3ff));
	rr = _mm_slli_epi16(rr, 4);
	__m128i rb0 = _mm_unpacklo_epi16(bb, rr);
	__m128i rb1 = _mm_unpackhi_epi16(bb, rr);
	__m128i g00 = _mm_unpacklo_epi16(gg, _mm_setzero_si128());
	__m128i g01 = _mm_unpackhi_epi16(gg, _mm_setzero_si128());
	g00 = _mm_slli_epi32(g00, 10);
	g01 = _mm_slli_epi32(g01, 10);
	__m128i m0 = _mm_or_si128(rb0, g00);
	__m128i m1 = _mm_or_si128(rb1, g01);
	m0 = _mm_shuffle_epi8(m0, _mm_set_epi8(12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3));
	m1 = _mm_shuffle_epi8(m1, _mm_set_epi8(12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3));

	return {
		m0,
		m1
	};
}

template<int F, bool NeedOffset = true, typename std::enable_if<F < CODEFEATURE_AVX2>::type*& = enabler>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToR210Element10(uint8_t* pp, __m128i gg, __m128i bb, __m128i rr)
{
	auto result = tuned_ConvertPlanarRGBXToR210Element10<F, NeedOffset>(gg, bb, rr);
	_mm_storeu_si128((__m128i *)pp, result.v0);
	_mm_storeu_si128((__m128i *)(pp + 16), result.v1);
}

template<int F>
void tuned_ConvertUQRGToR210(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride)
{
	const uint16_t *g = (const uint16_t *)pGBegin;
	const uint16_t *b = (const uint16_t *)pBBegin;
	const uint16_t *r = (const uint16_t *)pRBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + nWidth * 4;
		uint8_t *p = pStrideBegin;

#ifdef __SSSE3__
		for (; p <= pStrideEnd - 32; p += 32)
		{
			__m128i gg = _mm_loadu_si128((const __m128i*)g);
			__m128i bb = _mm_loadu_si128((const __m128i*)b);
			__m128i rr = _mm_loadu_si128((const __m128i*)r);
			tuned_ConvertPlanarRGBXToR210Element10<F>(p, gg, bb, rr);

			g += 8;
			b += 8;
			r += 8;
		}
#endif

		for (; p < pStrideEnd; p += 4)
		{
			uint32_t gg = *g;
			uint32_t bb = (*b + *g - 0x200) & 0x3ff;
			uint32_t rr = (*r + *g - 0x200) & 0x3ff;

			*(uint32_t *)p = htob32((rr << 20) | (gg << 10) | bb);
			g++;
			b++;
			r++;
		}

		memset(p, 0, pStrideBegin + ((nWidth + 63) / 64 * 256) - p);
	}
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertUQRGToR210<CODEFEATURE_SSSE3>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertUQRGToR210<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif
