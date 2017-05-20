/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1)
#error
#endif

template<int F, class C, class T>
void tuned_ConvertULY4ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	const int shift = 13;

	__m128i xy2rgb = _mm_set2_epi16_shift((-16 * C::Y2RGB + 0.5) / 0xff, C::Y2RGB, shift);
	__m128i vu2r = _mm_set2_epi16_shift(C::V2R, 0, shift);
	__m128i vu2g = _mm_set2_epi16_shift(C::V2G, C::U2G, shift);
	__m128i vu2b = _mm_set2_epi16_shift(0, C::U2B, shift);

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

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
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rr, rr), _mm_unpacklo_epi8(gg, bb));
				_mm_storeu_si128((__m128i *)pp, xrgb);
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rr), _mm_unpacklo_epi8(gg, bb));
				__m128i rgb = _mm_shuffle_epi8(xrgb, _mm_set_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
				_mm_storeu_si128((__m128i *)pp, rgb);
			}
#endif

			y += 4;
			u += 4;
			v += 4;
		}

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
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rr, rr), _mm_unpacklo_epi8(gg, bb));
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
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif


template<int F, class C, class T>
void tuned_ConvertRGBToULY4(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const int shift = 14;

	__m128i rb2y, xg2y, rb2u, xg2u, rb2v, xg2v;

	if (std::is_same<T, CBGRAColorOrder>::value || std::is_same<T, CBGRColorOrder>::value)
	{
		rb2y = _mm_set2_epi16_shift(C::R2Y, C::B2Y, shift);
		xg2y = _mm_set2_epi16_shift(16.5 / 0xff, C::G2Y, shift);
		rb2u = _mm_set2_epi16_shift(C::R2U, C::B2U, shift);
		xg2u = _mm_set2_epi16_shift(128.5 / 0xff, C::G2U, shift);
		rb2v = _mm_set2_epi16_shift(C::R2V, C::B2V, shift);
		xg2v = _mm_set2_epi16_shift(128.5 / 0xff, C::G2V, shift);
	}
	else
	{
		rb2y = _mm_set2_epi16_shift(C::B2Y, C::R2Y, shift);
		xg2y = _mm_set2_epi16_shift(C::G2Y, 16.5 / 0xff, shift);
		rb2u = _mm_set2_epi16_shift(C::B2U, C::R2U, shift);
		xg2u = _mm_set2_epi16_shift(C::G2U, 128.5 / 0xff, shift);
		rb2v = _mm_set2_epi16_shift(C::B2V, C::R2V, shift);
		xg2v = _mm_set2_epi16_shift(C::G2V, 128.5 / 0xff, shift);
	}

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

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
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_SSSE3, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif


// èàóùÇ™ ULY4 Ç∆ÇŸÇ⁄ìØÇ∂Ç»ÇÃÇÇ»ÇÒÇ∆Ç©ÇµÇΩÇ¢
template<int F, class C, class T>
void tuned_ConvertULY2ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	const int shift = 13;

	__m128i xy2rgb = _mm_set2_epi16_shift((-16 * C::Y2RGB + 0.5) / 0xff, C::Y2RGB, shift);
	__m128i vu2r = _mm_set2_epi16_shift(C::V2R, 0, shift);
	__m128i vu2g = _mm_set2_epi16_shift(C::V2G, C::U2G, shift);
	__m128i vu2b = _mm_set2_epi16_shift(0, C::U2B, shift);

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

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
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rr, rr), _mm_unpacklo_epi8(gg, bb));
				_mm_storeu_si128((__m128i *)pp, xrgb);
			}
#ifdef __SSSE3__
			else if (std::is_same<T, CRGBColorOrder>::value)
			{
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_setone_si128(), rr), _mm_unpacklo_epi8(gg, bb));
				__m128i rgb = _mm_shuffle_epi8(xrgb, _mm_set_epi8(-1, -1, -1, -1, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1));
				_mm_storeu_si128((__m128i *)pp, rgb);
			}
#endif

			y += 4;
			u += 2;
			v += 2;
		}

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
				__m128i xrgb = _mm_unpacklo_epi16(_mm_unpacklo_epi8(rr, rr), _mm_unpacklo_epi8(gg, bb));
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
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif


// ULY2ToRGB ÇŸÇ«Ç≈ÇÕÇ»Ç¢Ç™ ULY4 Ç∆Ç©Ç»ÇËãﬂÇ¢ÇÃÇà»â∫ó™
template<int F, class C, class T>
void tuned_ConvertRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const int shift = 14;

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

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

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
	}
}

#ifdef GENERATE_SSE2
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSE2, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_SSSE3
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_SSSE3, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif


template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	__m128i ctl;
	if (std::is_same<T, CYUYVColorOrder>::value)
		ctl = _mm_set_epi8(15, 11, 7, 3, 13, 9, 5, 1, 14, 12, 10, 8, 6, 4, 2, 0);
	else
		ctl = _mm_set_epi8(14, 10, 6, 2, 12, 8, 4, 0, 15, 13, 11, 9, 7, 5, 3, 1);

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i m0 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)pp), ctl);
			__m128i m1 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), ctl);
			__m128i m2 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), ctl);
			__m128i m3 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), ctl);

			__m128i yy0 = _mm_unpacklo_epi64(m0, m1);
			__m128i yy1 = _mm_unpacklo_epi64(m2, m3);
			__m128i uv0 = _mm_unpackhi_epi32(m0, m1);
			__m128i uv1 = _mm_unpackhi_epi32(m2, m3);

			__m128i uu = _mm_unpacklo_epi64(uv0, uv1);
			__m128i vv = _mm_unpackhi_epi64(uv0, uv1);

			_mm_storeu_si128((__m128i *)y, yy0);
			_mm_storeu_si128((__m128i *)(y + 16), yy1);
			_mm_storeu_si128((__m128i *)u, uu);
			_mm_storeu_si128((__m128i *)v, vv);

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
	}
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSSE3, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSSE3, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif


template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	__m128i ctl;
	if (std::is_same<T, CYUYVColorOrder>::value)
		ctl = _mm_set_epi8(15, 7, 11, 6, 14, 5, 10, 4, 13, 3, 9, 2, 12, 1, 8, 0);
	else
		ctl = _mm_set_epi8(7, 15, 6, 11, 5, 14, 4, 10, 3, 13, 2, 9, 1, 12, 0, 8);

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);

			__m128i uv0 = _mm_unpacklo_epi32(uu, vv);
			__m128i uv1 = _mm_unpackhi_epi32(uu, vv);

			_mm_storeu_si128((__m128i *)pp, _mm_shuffle_epi8(_mm_unpacklo_epi64(yy0, uv0), ctl));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_shuffle_epi8(_mm_unpackhi_epi64(yy0, uv0), ctl));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_shuffle_epi8(_mm_unpacklo_epi64(yy1, uv1), ctl));
			_mm_storeu_si128((__m128i *)(pp + 48), _mm_shuffle_epi8(_mm_unpackhi_epi64(yy1, uv1), ctl));

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
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSSE3, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSSE3, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif


template<int F>
static inline void tuned_ConvertBGRToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
#ifdef __SSSE3__
	__m128i ctl = _mm_set_epi8(13, 10, 7, 4, 1, 14, 11, 8, 5, 2, 15, 12, 9, 6, 3, 0);

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			__m128i m0 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)pp), ctl);        // G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
			__m128i m1 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), ctl); // R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
			__m128i m2 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), ctl); // Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

			__m128i n0 = _mm_alignr_epi8(m0, m2, 6); // B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb
			__m128i n1 = _mm_alignr_epi8(m2, m1, 6); // Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
			__m128i n2 = _mm_alignr_epi8(m1, m0, 6); // Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0

			__m128i bb = _mm_alignr_epi8(n1, n0, 5); // Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb
			__m128i gg = _mm_alignr_epi8(n0, n2, 5); // Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
			__m128i rr = _mm_alignr_epi8(n2, n1, 5); // R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5

			bb = _mm_alignr_epi8(bb, bb, 5);
			rr = _mm_alignr_epi8(rr, rr, 11);

			__m128i gg80 = _mm_add_epi8(gg, _mm_set1_epi8((char)0x80));
			bb = _mm_sub_epi8(bb, gg80);
			rr = _mm_sub_epi8(rr, gg80);

			_mm_storeu_si128((__m128i *)b, bb);
			_mm_storeu_si128((__m128i *)g, gg);
			_mm_storeu_si128((__m128i *)r, rr);

			b += 16;
			g += 16;
			r += 16;
		}

		for (; pp < p + cbWidth; pp += 3)
		{
			typedef CBGRColorOrder T;
			*g = pp[T::G];
			*b = pp[T::B] - pp[T::G] + 0x80;
			*r = pp[T::R] - pp[T::G] + 0x80;

			b += 1;
			g += 1;
			r += 1;
		}
	}
#endif
}

template<int F, class T, bool A>
static inline void tuned_ConvertRGBXToULRX(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	__m128i ctl;
	if (std::is_same<T, CBGRAColorOrder>::value)
		ctl = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	else
		ctl = _mm_set_epi8(12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3);

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;
	auto a = pABegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i m0 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)pp), ctl); // A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
			__m128i m1 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), ctl);
			__m128i m2 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), ctl);
			__m128i m3 = _mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), ctl);

			__m128i gb0 = _mm_unpacklo_epi32(m0, m1);
			__m128i gb1 = _mm_unpacklo_epi32(m2, m3);
			__m128i ar0 = _mm_unpackhi_epi32(m0, m1);
			__m128i ar1 = _mm_unpackhi_epi32(m2, m3);

			__m128i gg = _mm_unpackhi_epi64(gb0, gb1);
			__m128i gg80 = _mm_add_epi8(gg, _mm_set1_epi8((char)0x80));
			__m128i bb = _mm_sub_epi8(_mm_unpacklo_epi64(gb0, gb1), gg80);
			__m128i rr = _mm_sub_epi8(_mm_unpacklo_epi64(ar0, ar1), gg80);

			_mm_storeu_si128((__m128i *)b, bb);
			_mm_storeu_si128((__m128i *)g, gg);
			_mm_storeu_si128((__m128i *)r, rr);
			if (A)
				_mm_storeu_si128((__m128i *)a, _mm_unpackhi_epi64(ar0, ar1));

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}
#endif

		for (; pp < p + cbWidth; pp += 4)
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
	}
}

template<int F, class T>
void tuned_ConvertRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertBGRToULRG<F>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride);
	else
		tuned_ConvertRGBXToULRX<F, T, false>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertRGBXToULRX<F, T, true>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSSE3, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif


template<int F>
void tuned_ConvertULRGToBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
#ifdef __SSSE3__
	__m128i ctl = _mm_set_epi8(5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11, 0);

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);

			__m128i gg80 = _mm_add_epi8(gg, _mm_set1_epi8((char)0x80));
			bb = _mm_add_epi8(bb, gg80);
			rr = _mm_add_epi8(rr, gg80);

			rr = _mm_alignr_epi8(rr, rr, 5);  // R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
			bb = _mm_alignr_epi8(bb, bb, 11); // Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

			__m128i m0 = _mm_alignr_epi8(gg, rr, 11); // Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
			__m128i m1 = _mm_alignr_epi8(rr, bb, 11); // Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
			__m128i m2 = _mm_alignr_epi8(bb, gg, 11); // B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb

			__m128i n0 = _mm_alignr_epi8(m0, m2, 10); // G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
			__m128i n1 = _mm_alignr_epi8(m1, m0, 10); // R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
			__m128i n2 = _mm_alignr_epi8(m2, m1, 10); // Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

			_mm_storeu_si128((__m128i *)pp, _mm_shuffle_epi8(n0, ctl));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_shuffle_epi8(n1, ctl));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_shuffle_epi8(n2, ctl));

			b += 16;
			g += 16;
			r += 16;
		}

		for (; pp < p + cbWidth; pp += 3)
		{
			typedef CBGRColorOrder T;
			pp[T::G] = *g;
			pp[T::B] = *b + *g - 0x80;
			pp[T::R] = *r + *g - 0x80;

			b += 1;
			g += 1;
			r += 1;
		}
	}
#endif
}

template<int F, class T, bool A>
void tuned_ConvertULRXToRGBX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	__m128i ctl;
	if (std::is_same<T, CBGRAColorOrder>::value)
		ctl = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	else
		ctl = _mm_set_epi8(3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12);

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;
	auto a = pABegin;

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
				aa = _mm_set1_epi8((char)0xff);

			__m128i gg80 = _mm_add_epi8(gg, _mm_set1_epi8((char)0x80));
			bb = _mm_add_epi8(bb, gg80);
			rr = _mm_add_epi8(rr, gg80);

			__m128i gb0 = _mm_unpacklo_epi32(bb, gg); // G7 G6 G5 G4 B7 B6 B5 B4 G3 G2 G1 G0 B3 B2 B1 B0
			__m128i gb1 = _mm_unpackhi_epi32(bb, gg);
			__m128i ar0 = _mm_unpacklo_epi32(rr, aa);
			__m128i ar1 = _mm_unpackhi_epi32(rr, aa);

			__m128i m0 = _mm_unpacklo_epi64(gb0, ar0); // A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0
			__m128i m1 = _mm_unpackhi_epi64(gb0, ar0);
			__m128i m2 = _mm_unpacklo_epi64(gb1, ar1);
			__m128i m3 = _mm_unpackhi_epi64(gb1, ar1);

			_mm_storeu_si128((__m128i *)pp, _mm_shuffle_epi8(m0, ctl));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_shuffle_epi8(m1, ctl));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_shuffle_epi8(m2, ctl));
			_mm_storeu_si128((__m128i *)(pp + 48), _mm_shuffle_epi8(m3, ctl));

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::G] = *g;
			pp[T::B] = *b + *g - 0x80;
			pp[T::R] = *r + *g - 0x80;
			if (A)
				pp[T::A] = *a;
			else
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
void tuned_ConvertULRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertULRGToBGR<F>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
	else
		tuned_ConvertULRXToRGBX<F, T, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX<F, T, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSSE3
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSSE3, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_SSSE3, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_SSSE3, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin,  size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif
