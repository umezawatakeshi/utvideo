/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2)
#error
#endif

extern void* enabler;

#include "POD.h"

template<int F, typename VT, class T, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
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

template<int F, typename VT, class T, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m256i> VECTORCALL tuned_ConvertPackedYUV422ToPlanarElement(__m256i m0, __m256i m1, __m256i m2, __m256i m3)
{
	__m256i ctl0, ctl1, ctl2, ctl3;
	if (std::is_same<T, CYUYVColorOrder>::value)
	{
		ctl0 = _mm256_set16_epi8(15, 11, 7, 3, 13, 9, 5, 1, 14, 12, 10, 8, 6, 4, 2, 0);
		ctl1 = _mm256_set16_epi8(14, 12, 10, 8, 6, 4, 2, 0, 15, 11, 7, 3, 13, 9, 5, 1);
		ctl2 = _mm256_set16_epi8(13, 9, 5, 1, 15, 11, 7, 3, 14, 12, 10, 8, 6, 4, 2, 0);
		ctl3 = _mm256_set16_epi8(14, 12, 10, 8, 6, 4, 2, 0, 13, 9, 5, 1, 15, 11, 7, 3);
	}
	else
	{
		ctl0 = _mm256_set16_epi8(14, 10, 6, 2, 12, 8, 4, 0, 15, 13, 11, 9, 7, 5, 3, 1);
		ctl1 = _mm256_set16_epi8(15, 13, 11, 9, 7, 5, 3, 1, 14, 10, 6, 2, 12, 8, 4, 0);
		ctl2 = _mm256_set16_epi8(12, 8, 4, 0, 14, 10, 6, 2, 15, 13, 11, 9, 7, 5, 3, 1);
		ctl3 = _mm256_set16_epi8(15, 13, 11, 9, 7, 5, 3, 1, 12, 8, 4, 0, 14, 10, 6, 2);
	}

	m0 = _mm256_shuffle_epi8(m0, ctl0);
	m1 = _mm256_shuffle_epi8(m1, ctl1);
	m2 = _mm256_shuffle_epi8(m2, ctl2);
	m3 = _mm256_shuffle_epi8(m3, ctl3);

	__m256i yy0 = _mm256_blend_epi32(m0, m1, 0xcc);
	__m256i yy1 = _mm256_blend_epi32(m2, m3, 0xcc);
	__m256i uv0 = _mm256_blend_epi32(m0, m1, 0x33);
	__m256i uv1 = _mm256_blend_epi32(m2, m3, 0x33);

	__m256i uu = _mm256_blend_epi32(uv0, uv1, 0xaa);
	__m256i vv = _mm256_blend_epi32(uv0, uv1, 0x55);

	yy0 = _mm256_permute4x64_epi64(yy0, 0xd8);
	yy1 = _mm256_permute4x64_epi64(yy1, 0xd8);
	uu = _mm256_permutevar8x32_epi32(uu, _mm256_set_epi32(5, 1, 7, 3, 4, 0, 6, 2));
	vv = _mm256_permutevar8x32_epi32(vv, _mm256_set_epi32(4, 0, 6, 2, 5, 1, 7, 3));

	return { yy0, yy1, uu, vv };
}

template<int F, typename VT, class T>
static inline FORCEINLINE VECTOR_YUV422<VT> tuned_ConvertPackedYUV422ToPlanarElement(const uint8_t* pp)
{
	return tuned_ConvertPackedYUV422ToPlanarElement<F, VT, T>(
		_mmt_loadu<VT>(pp),
		_mmt_loadu<VT>(pp + sizeof(VT)),
		_mmt_loadu<VT>(pp + sizeof(VT) * 2),
		_mmt_loadu<VT>(pp + sizeof(VT) * 3)
	);
}

template<int F, typename VT, class T>
static inline FORCEINLINE VECTOR_YUV422<VT> tuned_ConvertPackedYUV422ToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	return tuned_ConvertPackedYUV422ToPlanarElement<F, VT, T>(
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp),                  _mmt_loadu<VT>(pp - scbStride)),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT)),     _mmt_loadu<VT>(pp - scbStride + sizeof(VT))),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT) * 2), _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 2)),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT) * 3), _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 3))
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
			auto result = tuned_ConvertPackedYUV422ToPlanarElement<F, __m256i, T>(pp);
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
			auto result = tuned_ConvertPackedYUV422ToPlanarElement<F, __m128i, T>(pp);
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

template<int F, typename VT, class T, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
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

template<int F, typename VT, class T, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler>
static inline FORCEINLINE VECTOR4<__m256i> VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(__m256i yy0, __m256i yy1, __m256i uu, __m256i vv)
{
	__m256i ctl0, ctl1, ctl2, ctl3;
	if (std::is_same<T, CYUYVColorOrder>::value)
	{
		ctl0 = _mm256_set16_epi8(15, 7, 11, 6, 14, 5, 10, 4, 13, 3, 9, 2, 12, 1, 8, 0);
		ctl1 = _mm256_set16_epi8(7, 15, 3, 14, 6, 13, 2, 12, 5, 11, 1, 10, 4, 9, 0, 8);
		ctl2 = _mm256_set16_epi8(11, 7, 15, 6, 10, 5, 14, 4, 9, 3, 13, 2, 8, 1, 12, 0);
		ctl3 = _mm256_set16_epi8(3, 15, 7, 14, 2, 13, 6, 12, 1, 11, 5, 10, 0, 9, 4, 8);
	}
	else
	{
		ctl0 = _mm256_set16_epi8(7, 15, 6, 11, 5, 14, 4, 10, 3, 13, 2, 9, 1, 12, 0, 8);
		ctl1 = _mm256_set16_epi8(15, 7, 14, 3, 13, 6, 12, 2, 11, 5, 10, 1, 9, 4, 8, 0);
		ctl2 = _mm256_set16_epi8(7, 11, 6, 15, 5, 10, 4, 14, 3, 9, 2, 13, 1, 8, 0, 12);
		ctl3 = _mm256_set16_epi8(15, 3, 14, 7, 13, 2, 12, 6, 11, 1, 10, 5, 9, 0, 8, 4);
	}

	yy0 = _mm256_permute4x64_epi64(yy0, 0xd8);
	yy1 = _mm256_permute4x64_epi64(yy1, 0xd8);
	uu = _mm256_permutevar8x32_epi32(uu, _mm256_set_epi32(5, 1, 7, 3, 4, 0, 6, 2));
	vv = _mm256_permutevar8x32_epi32(vv, _mm256_set_epi32(1, 5, 3, 7, 0, 4, 2, 6));

	__m256i uv0 = _mm256_blend_epi32(uu, vv, 0xaa);
	__m256i uv1 = _mm256_blend_epi32(uu, vv, 0x55);

	__m256i m0 = _mm256_blend_epi32(yy0, uv0, 0xcc);
	__m256i m1 = _mm256_blend_epi32(yy0, uv0, 0x33);
	__m256i m2 = _mm256_blend_epi32(yy1, uv1, 0xcc);
	__m256i m3 = _mm256_blend_epi32(yy1, uv1, 0x33);

	return {
		_mm256_shuffle_epi8(m0, ctl0),
		_mm256_shuffle_epi8(m1, ctl1),
		_mm256_shuffle_epi8(m2, ctl2),
		_mm256_shuffle_epi8(m3, ctl3)
	};
}

template<int F, typename VT, class T>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(uint8_t* pp, VT yy0, VT yy1, VT uu, VT vv)
{
	auto result = tuned_ConvertPlanarYUV422ToPackedElement<F, VT, T>(yy0, yy1, uu, vv);
	_mmt_storeu<VT>(pp,                  result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT),     result.v1);
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, result.v2);
	_mmt_storeu<VT>(pp + sizeof(VT) * 3, result.v3);
}

template<int F, typename VT, class T>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(uint8_t* pp, VT yy0, VT yy1, VT uu, VT vv, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarYUV422ToPackedElement<F, VT, T>(yy0, yy1, uu, vv);
	_mmt_storeu<VT>(pp,                  _mmt_add_epi8<VT>(result.v0, _mmt_loadu<VT>(pp - scbStride)));
	_mmt_storeu<VT>(pp + sizeof(VT),     _mmt_add_epi8<VT>(result.v1, _mmt_loadu<VT>(pp - scbStride + sizeof(VT))));
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, _mmt_add_epi8<VT>(result.v2, _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 2)));
	_mmt_storeu<VT>(pp + sizeof(VT) * 3, _mmt_add_epi8<VT>(result.v3, _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 3)));
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
			tuned_ConvertPlanarYUV422ToPackedElement<F, __m256i, T>(pp, yy0, yy1, uu, vv);

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
			tuned_ConvertPlanarYUV422ToPackedElement<F, __m128i, T>(pp, yy0, yy1, uu, vv);

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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler>
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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler> /* 最適化が有効な場合、返した a を触らなければ a を計算する命令は生成されないので、やはり A はテンプレートパラメータとしては要らない */
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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m256i> VECTORCALL tuned_ConvertPackedRGBXToPlanarElement(__m256i m0, __m256i m1, __m256i m2, __m256i m3)
{
	__m256i ctl0, ctl1, ctl2, ctl3;
	if (std::is_same<T, CBGRAColorOrder>::value)
	{
		ctl0 = _mm256_set16_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
		ctl1 = _mm256_set16_epi8(13, 9, 5, 1, 12, 8, 4, 0, 15, 11, 7, 3, 14, 10, 6, 2);
		ctl2 = _mm256_set16_epi8(14, 10, 6, 2, 15, 11, 7, 3, 12, 8, 4, 0, 13, 9, 5, 1);
		ctl3 = _mm256_set16_epi8(12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3);
	}
	else
	{
		ctl0 = _mm256_set16_epi8(12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3);
		ctl1 = _mm256_set16_epi8(14, 10, 6, 2, 15, 11, 7, 3, 12, 8, 4, 0, 13, 9, 5, 1);
		ctl2 = _mm256_set16_epi8(13, 9, 5, 1, 12, 8, 4, 0, 15, 11, 7, 3, 14, 10, 6, 2);
		ctl3 = _mm256_set16_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	}

	m0 = _mm256_shuffle_epi8(m0, ctl0);
	m1 = _mm256_shuffle_epi8(m1, ctl1);
	m2 = _mm256_shuffle_epi8(m2, ctl2);
	m3 = _mm256_shuffle_epi8(m3, ctl3);

	__m256i n0 = _mm256_blend_epi32(m0, m1, 0xcc);
	__m256i n1 = _mm256_blend_epi32(m0, m1, 0x33);
	__m256i n2 = _mm256_blend_epi32(m2, m3, 0xcc);
	__m256i n3 = _mm256_blend_epi32(m2, m3, 0x33);

	__m256i bb = _mm256_blend_epi32(n0, n2, 0xaa);
	__m256i gg = _mm256_blend_epi32(n0, n2, 0x55);
	__m256i rr = _mm256_blend_epi32(n1, n3, 0xaa);
	__m256i aa = _mm256_blend_epi32(n1, n3, 0x55);

	bb = _mm256_permutevar8x32_epi32(bb, _mm256_set_epi32(7, 3, 5, 1, 6, 2, 4, 0));
	gg = _mm256_permutevar8x32_epi32(gg, _mm256_set_epi32(6, 2, 4, 0, 7, 3, 5, 1));
	rr = _mm256_permutevar8x32_epi32(rr, _mm256_set_epi32(5, 1, 7, 3, 4, 0, 6, 2));
	aa = _mm256_permutevar8x32_epi32(aa, _mm256_set_epi32(4, 0, 6, 2, 5, 1, 7, 3));

	__m256i ggtmp = NeedOffset ? _mm256_add_epi8(gg, _mm256_set1_epi8((char)0x80)) : gg;
	bb = _mm256_sub_epi8(bb, ggtmp);
	rr = _mm256_sub_epi8(rr, ggtmp);

	return { gg, bb, rr, aa };
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<T::BYPP == 4>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<VT> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp)
{
	return tuned_ConvertPackedRGBXToPlanarElement<F, VT, T, NeedOffset>(
		_mmt_loadu<VT>(pp),
		_mmt_loadu<VT>(pp + sizeof(VT)),
		_mmt_loadu<VT>(pp + sizeof(VT) * 2),
		_mmt_loadu<VT>(pp + sizeof(VT) * 3)
	);
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<VT> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp)
{
	auto ret = tuned_ConvertPackedBGRToPlanarElement<F, VT, NeedOffset>(
		_mmt_loadu<VT>(pp),
		_mmt_loadu<VT>(pp + sizeof(VT)),
		_mmt_loadu<VT>(pp + sizeof(VT) * 2)
	);
	return { ret.g, ret.b, ret.r, _mmt_set1_epi8<VT>((char)0xff) };
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<T::BYPP == 4>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<VT> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	return tuned_ConvertPackedRGBXToPlanarElement<F, VT, T, NeedOffset>(
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp),                  _mmt_loadu<VT>(pp - scbStride)),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT)),     _mmt_loadu<VT>(pp - scbStride + sizeof(VT))),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT) * 2), _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 2)),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT) * 3), _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 3))
	);
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<VT> tuned_ConvertPackedRGBXToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	auto ret = tuned_ConvertPackedBGRToPlanarElement<F, VT, NeedOffset>(
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp),                  _mmt_loadu<VT>(pp - scbStride)),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT)),     _mmt_loadu<VT>(pp - scbStride + sizeof(VT))),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT) * 2), _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 2))
	);
	return { ret.g, ret.b, ret.r, _mmt_set1_epi8<VT>((char)0xff) };
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
			auto result = tuned_ConvertPackedRGBXToPlanarElement<F, __m256i, T, true>(pp);
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
			auto result = tuned_ConvertPackedRGBXToPlanarElement<F, __m128i, T, true>(pp);
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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler>
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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler> /* A はテンプレートパラメータとしては要らない */
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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE VECTOR4<__m256i> VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(__m256i gg, __m256i bb, __m256i rr, __m256i aa)
{
	__m256i ctl0, ctl1, ctl2, ctl3;
	if (std::is_same<T, CBGRAColorOrder>::value)
	{
		ctl0 = _mm256_set16_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
		ctl1 = _mm256_set16_epi8(7, 3, 15, 11, 6, 2, 14, 10, 5, 1, 13, 9, 4, 0, 12, 8);
		ctl2 = _mm256_set16_epi8(11, 15, 3, 7, 10, 14, 2, 6, 9, 13, 1, 5, 8, 12, 0, 4);
		ctl3 = _mm256_set16_epi8(3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12);
	}
	else
	{
		ctl0 = _mm256_set16_epi8(3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12);
		ctl1 = _mm256_set16_epi8(11, 15, 3, 7, 10, 14, 2, 6, 9, 13, 1, 5, 8, 12, 0, 4);
		ctl2 = _mm256_set16_epi8(7, 3, 15, 11, 6, 2, 14, 10, 5, 1, 13, 9, 4, 0, 12, 8);
		ctl3 = _mm256_set16_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	}

	__m256i ggtmp = NeedOffset ? _mm256_add_epi8(gg, _mm256_set1_epi8((char)0x80)) : gg;
	bb = _mm256_add_epi8(bb, ggtmp);
	rr = _mm256_add_epi8(rr, ggtmp);

	bb = _mm256_permutevar8x32_epi32(bb, _mm256_set_epi32(7, 3, 5, 1, 6, 2, 4, 0));
	gg = _mm256_permutevar8x32_epi32(gg, _mm256_set_epi32(3, 7, 1, 5, 2, 6, 0, 4));
	rr = _mm256_permutevar8x32_epi32(rr, _mm256_set_epi32(5, 1, 7, 3, 4, 0, 6, 2));
	aa = _mm256_permutevar8x32_epi32(aa, _mm256_set_epi32(1, 5, 3, 7, 0, 4, 2, 6)); // aa に定数を渡した場合でも VPERMD してしまうが、その場合でもループ外に出されるので許容することにする。Clang なら VPERMD せずに単なる定数ロードに変換してくれる。

	__m256i n0 = _mm256_blend_epi32(bb, gg, 0xaa);
	__m256i n1 = _mm256_blend_epi32(rr, aa, 0xaa);
	__m256i n2 = _mm256_blend_epi32(bb, gg, 0x55);
	__m256i n3 = _mm256_blend_epi32(rr, aa, 0x55);

	__m256i m0 = _mm256_blend_epi32(n0, n1, 0xcc);
	__m256i m1 = _mm256_blend_epi32(n0, n1, 0x33);
	__m256i m2 = _mm256_blend_epi32(n2, n3, 0xcc);
	__m256i m3 = _mm256_blend_epi32(n2, n3, 0x33);

	return {
		_mm256_shuffle_epi8(m0, ctl0),
		_mm256_shuffle_epi8(m1, ctl1),
		_mm256_shuffle_epi8(m2, ctl2),
		_mm256_shuffle_epi8(m3, ctl3)
	};
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<T::BYPP == 4>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, VT gg, VT bb, VT rr, VT aa)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, VT, T, NeedOffset>(gg, bb, rr, aa);
	_mmt_storeu<VT>(pp,                  result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT),     result.v1);
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, result.v2);
	_mmt_storeu<VT>(pp + sizeof(VT) * 3, result.v3);
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, VT gg, VT bb, VT rr, VT aa)
{
	auto result = tuned_ConvertPlanarBGRToPackedElement<F, VT, NeedOffset>(gg, bb, rr);
	_mmt_storeu<VT>(pp,                  result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT),     result.v1);
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, result.v2);
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<T::BYPP == 4>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, VT gg, VT bb, VT rr, VT aa, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, VT, T, NeedOffset>(gg, bb, rr, aa);
	_mmt_storeu<VT>(pp,                  _mmt_add_epi8<VT>(result.v0, _mmt_loadu<VT>(pp - scbStride)));
	_mmt_storeu<VT>(pp + sizeof(VT),     _mmt_add_epi8<VT>(result.v1, _mmt_loadu<VT>(pp - scbStride + sizeof(VT))));
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, _mmt_add_epi8<VT>(result.v2, _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 2)));
	_mmt_storeu<VT>(pp + sizeof(VT) * 3, _mmt_add_epi8<VT>(result.v3, _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 3)));
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler> /* A はテンプレートパラメータとしては要らない */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, VT gg, VT bb, VT rr, VT aa, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarBGRToPackedElement<F, VT, NeedOffset>(gg, bb, rr);
	_mmt_storeu<VT>(pp,                  _mmt_add_epi8<VT>(result.v0, _mmt_loadu<VT>(pp - scbStride)));
	_mmt_storeu<VT>(pp + sizeof(VT),     _mmt_add_epi8<VT>(result.v1, _mmt_loadu<VT>(pp - scbStride + sizeof(VT))));
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, _mmt_add_epi8<VT>(result.v2, _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 2)));
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
			tuned_ConvertPlanarRGBXToPackedElement<F, __m256i, T, true>(pp, gg, bb, rr, A ? _mm256_loadu_si256((const __m256i *)a) : _mm256_set1_epi8((char)0xff));

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
			tuned_ConvertPlanarRGBXToPackedElement<F, __m128i, T, true>(pp, gg, bb, rr, A ? _mm_loadu_si128((const __m128i *)a) : _mm_set1_epi8((char)0xff));

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

static inline FORCEINLINE __m128i _mm_Convert16To10Fullrange(__m128i x)
{
	return _mm_srli_epi16(_mm_sub_epi16(_mm_add_epi16(x, _mm_set1_epi16(1 << 5)), _mm_srli_epi16(x, 10)), 6);
}

static inline FORCEINLINE __m128i _mm_Convert10To16Fullrange(__m128i x)
{
	__m128i tmp = _mm_slli_epi16(x, 6);
	return _mm_or_si128(tmp, _mm_srli_epi16(tmp, 10));
}


template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler> /* 最適化が有効な場合、返した a を触らなければ a を計算する命令は生成されないので、やはり A はテンプレートパラメータとしては要らない */
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

template<int F, typename VT, bool NeedOffset>
static inline FORCEINLINE VECTOR_RGBA<VT> tuned_ConvertB64aToPlanarElement10(const uint8_t* pp)
{
	return tuned_ConvertB64aToPlanarElement10<F, VT, NeedOffset>(
		_mmt_loadu<VT>(pp),
		_mmt_loadu<VT>(pp + sizeof(VT)),
		_mmt_loadu<VT>(pp + sizeof(VT) * 2),
		_mmt_loadu<VT>(pp + sizeof(VT) * 3)
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
			auto result = tuned_ConvertB64aToPlanarElement10<F, __m128i, true>(pp);

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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
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

template<int F, typename VT, bool NeedOffset>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToB64aElement10(uint8_t* pp, VT gg, VT bb, VT rr, VT aa)
{
	auto result = tuned_ConvertPlanarRGBXToB64aElement10<F, VT, NeedOffset>(gg, bb, rr, aa);
	_mmt_storeu<VT>(pp,                  result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT),     result.v1);
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, result.v2);
	_mmt_storeu<VT>(pp + sizeof(VT) * 3, result.v3);
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

			tuned_ConvertPlanarRGBXToB64aElement10<F, __m128i, true>(pp, gg, bb, rr, aa);

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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
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

template<int F, typename VT, bool NeedOffset>
static inline FORCEINLINE VECTOR_RGB<VT> tuned_ConvertR210ToPlanarElement10(const uint8_t* pp)
{
	return tuned_ConvertR210ToPlanarElement10<F, VT, NeedOffset>(
		_mmt_loadu<VT>(pp),
		_mmt_loadu<VT>(pp + sizeof(VT))
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
			auto result = tuned_ConvertR210ToPlanarElement10<F, __m128i, true>(p);
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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
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

template<int F, typename VT, bool NeedOffset>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToR210Element10(uint8_t* pp, VT gg, VT bb, VT rr)
{
	auto result = tuned_ConvertPlanarRGBXToR210Element10<F, VT, NeedOffset>(gg, bb, rr);
	_mmt_storeu<VT>(pp,              result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT), result.v1);
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
			tuned_ConvertPlanarRGBXToR210Element10<F, __m128i, true>(p, gg, bb, rr);

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
