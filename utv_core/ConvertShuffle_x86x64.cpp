/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

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
	using VT = std::conditional_t<F < CODEFEATURE_AVX2, __m128i, __m256i>;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

		for (; pp <= p + cbWidth - sizeof(VT) * 4; pp += sizeof(VT) * 4)
		{
			auto result = tuned_ConvertPackedYUV422ToPlanarElement<F, VT, T>(pp);
			_mmt_storeu<VT>(y, result.y0);
			_mmt_storeu<VT>(y + sizeof(VT), result.y1);
			_mmt_storeu<VT>(u, result.u);
			_mmt_storeu<VT>(v, result.v);

			y += sizeof(VT) * 2;
			u += sizeof(VT);
			v += sizeof(VT);
		}

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

#ifdef GENERATE_SSE41
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
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
	__m128i uv0 = _mm_unpacklo_epi8(uu, vv);
	__m128i uv1 = _mm_unpackhi_epi8(uu, vv);

	if (std::is_same<T, CYUYVColorOrder>::value)
	{
		return {
			_mm_unpacklo_epi8(yy0, uv0),
			_mm_unpackhi_epi8(yy0, uv0),
			_mm_unpacklo_epi8(yy1, uv1),
			_mm_unpackhi_epi8(yy1, uv1)
		};
	}
	else
	{
		return {
			_mm_unpacklo_epi8(uv0, yy0),
			_mm_unpackhi_epi8(uv0, yy0),
			_mm_unpacklo_epi8(uv1, yy1),
			_mm_unpackhi_epi8(uv1, yy1)
		};
	}
}

template<int F, typename VT, class T, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler>
static inline FORCEINLINE VECTOR4<__m256i> VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(__m256i yy0, __m256i yy1, __m256i uu, __m256i vv)
{
	yy0 = _mm256_permute4x64_epi64(yy0, 0xd8);
	yy1 = _mm256_permute4x64_epi64(yy1, 0xd8);
	uu = _mm256_permutevar8x32_epi32(uu, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	vv = _mm256_permutevar8x32_epi32(vv, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));

	__m256i uv0 = _mm256_unpacklo_epi8(uu, vv);
	__m256i uv1 = _mm256_unpackhi_epi8(uu, vv);

	if (std::is_same<T, CYUYVColorOrder>::value)
	{
		return {
			_mm256_unpacklo_epi8(yy0, uv0),
			_mm256_unpackhi_epi8(yy0, uv0),
			_mm256_unpacklo_epi8(yy1, uv1),
			_mm256_unpackhi_epi8(yy1, uv1)
		};
	}
	else
	{
		return {
			_mm256_unpacklo_epi8(uv0, yy0),
			_mm256_unpackhi_epi8(uv0, yy0),
			_mm256_unpacklo_epi8(uv1, yy1),
			_mm256_unpackhi_epi8(uv1, yy1)
		};
	}
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
	using VT = std::conditional_t<F < CODEFEATURE_AVX2, __m128i, __m256i>;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		auto y = pYBegin;
		auto u = pUBegin;
		auto v = pVBegin;

		auto pp = p;

		for (; pp <= p + cbWidth - sizeof(VT) * 4; pp += sizeof(VT) * 4)
		{
			VT yy0 = _mmt_loadu<VT>(y);
			VT yy1 = _mmt_loadu<VT>(y + sizeof(VT));
			VT uu = _mmt_loadu<VT>(u);
			VT vv = _mmt_loadu<VT>(v);
			tuned_ConvertPlanarYUV422ToPackedElement<F, VT, T>(pp, yy0, yy1, uu, vv);

			y += sizeof(VT) * 2;
			u += sizeof(VT);
			v += sizeof(VT);
		}

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

#ifdef GENERATE_SSE41
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler> /* �œK�����L���ȏꍇ�A�Ԃ��� a ��G��Ȃ���� a ���v�Z���閽�߂͐�������Ȃ��̂ŁA��͂� A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
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
	using VT = std::conditional_t<F < CODEFEATURE_AVX2, __m128i, __m256i>;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth, pABegin += cbPlaneWidth)
	{
		auto r = pRBegin;
		auto g = pGBegin;
		auto b = pBBegin;
		auto a = pABegin;

		auto pp = p;

		for (; pp <= p + cbWidth - T::BYPP * sizeof(VT); pp += T::BYPP * sizeof(VT))
		{
			auto result = tuned_ConvertPackedRGBXToPlanarElement<F, VT, T, true>(pp);
			_mmt_storeu<VT>(b, result.b);
			_mmt_storeu<VT>(g, result.g);
			_mmt_storeu<VT>(r, result.r);
			if (A)
				_mmt_storeu<VT>(a, result.a);

			b += sizeof(VT);
			g += sizeof(VT);
			r += sizeof(VT);
			if (A)
				a += sizeof(VT);
		}

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

#ifdef GENERATE_SSE41
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler> /* A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
static inline FORCEINLINE VECTOR4<__m128i> VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(__m128i gg, __m128i bb, __m128i rr, __m128i aa)
{
	__m128i ggtmp = NeedOffset ? _mm_add_epi8(gg, _mm_set1_epi8((char)0x80)) : gg;
	bb = _mm_add_epi8(bb, ggtmp);
	rr = _mm_add_epi8(rr, ggtmp);

	__m128i m0, m1, n0, n1;

	if (std::is_same<T, CBGRAColorOrder>::value)
	{
		m0 = _mm_unpacklo_epi8(bb, rr);
		m1 = _mm_unpackhi_epi8(bb, rr);
		n0 = _mm_unpacklo_epi8(gg, aa);
		n1 = _mm_unpackhi_epi8(gg, aa);
	}
	else
	{
		m0 = _mm_unpacklo_epi8(aa, gg);
		m1 = _mm_unpackhi_epi8(aa, gg);
		n0 = _mm_unpacklo_epi8(rr, bb);
		n1 = _mm_unpackhi_epi8(rr, bb);
	}

	return {
		_mm_unpacklo_epi8(m0, n0),
		_mm_unpackhi_epi8(m0, n0),
		_mm_unpacklo_epi8(m1, n1),
		_mm_unpackhi_epi8(m1, n1)
	};
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m256i>>*& = enabler> /* A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
static inline FORCEINLINE VECTOR4<__m256i> VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(__m256i gg, __m256i bb, __m256i rr, __m256i aa)
{
	__m256i ggtmp = NeedOffset ? _mm256_add_epi8(gg, _mm256_set1_epi8((char)0x80)) : gg;
	bb = _mm256_add_epi8(bb, ggtmp);
	rr = _mm256_add_epi8(rr, ggtmp);

	bb = _mm256_permutevar8x32_epi32(bb, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	gg = _mm256_permutevar8x32_epi32(gg, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	rr = _mm256_permutevar8x32_epi32(rr, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0));
	aa = _mm256_permutevar8x32_epi32(aa, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0)); // aa �ɒ萔��n�����ꍇ�ł� VPERMD ���Ă��܂����A���̏ꍇ�ł����[�v�O�ɏo�����̂ŋ��e���邱�Ƃɂ���BClang �Ȃ� VPERMD �����ɒP�Ȃ�萔���[�h�ɕϊ����Ă����B

	__m256i m0, m1, n0, n1;

	if (std::is_same<T, CBGRAColorOrder>::value)
	{
		m0 = _mm256_unpacklo_epi8(bb, rr);
		m1 = _mm256_unpackhi_epi8(bb, rr);
		n0 = _mm256_unpacklo_epi8(gg, aa);
		n1 = _mm256_unpackhi_epi8(gg, aa);
	}
	else
	{
		m0 = _mm256_unpacklo_epi8(aa, gg);
		m1 = _mm256_unpackhi_epi8(aa, gg);
		n0 = _mm256_unpacklo_epi8(rr, bb);
		n1 = _mm256_unpackhi_epi8(rr, bb);
	}

	return {
		_mm256_unpacklo_epi8(m0, n0),
		_mm256_unpackhi_epi8(m0, n0),
		_mm256_unpacklo_epi8(m1, n1),
		_mm256_unpackhi_epi8(m1, n1)
	};
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<T::BYPP == 4>::type*& = enabler> /* A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, VT gg, VT bb, VT rr, VT aa)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, VT, T, NeedOffset>(gg, bb, rr, aa);
	_mmt_storeu<VT>(pp,                  result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT),     result.v1);
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, result.v2);
	_mmt_storeu<VT>(pp + sizeof(VT) * 3, result.v3);
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler> /* A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, VT gg, VT bb, VT rr, VT aa)
{
	auto result = tuned_ConvertPlanarBGRToPackedElement<F, VT, NeedOffset>(gg, bb, rr);
	_mmt_storeu<VT>(pp,                  result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT),     result.v1);
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, result.v2);
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<T::BYPP == 4>::type*& = enabler> /* A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(uint8_t* pp, VT gg, VT bb, VT rr, VT aa, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarRGBXToPackedElement<F, VT, T, NeedOffset>(gg, bb, rr, aa);
	_mmt_storeu<VT>(pp,                  _mmt_add_epi8<VT>(result.v0, _mmt_loadu<VT>(pp - scbStride)));
	_mmt_storeu<VT>(pp + sizeof(VT),     _mmt_add_epi8<VT>(result.v1, _mmt_loadu<VT>(pp - scbStride + sizeof(VT))));
	_mmt_storeu<VT>(pp + sizeof(VT) * 2, _mmt_add_epi8<VT>(result.v2, _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 2)));
	_mmt_storeu<VT>(pp + sizeof(VT) * 3, _mmt_add_epi8<VT>(result.v3, _mmt_loadu<VT>(pp - scbStride + sizeof(VT) * 3)));
}

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if<std::is_same<T, CBGRColorOrder>::value>::type*& = enabler> /* A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
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
	using VT = std::conditional_t<F < CODEFEATURE_AVX2, __m128i, __m256i>;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth, pABegin += cbPlaneWidth)
	{
		auto r = pRBegin;
		auto g = pGBegin;
		auto b = pBBegin;
		auto a = pABegin;

		auto pp = p;

		for (; pp <= p + cbWidth - T::BYPP * sizeof(VT); pp += T::BYPP * sizeof(VT))
		{
			VT gg = _mmt_loadu<VT>(g);
			VT bb = _mmt_loadu<VT>(b);
			VT rr = _mmt_loadu<VT>(r);
			tuned_ConvertPlanarRGBXToPackedElement<F, VT, T, true>(pp, gg, bb, rr, A ? _mmt_loadu<VT>(a) : _mmt_set1_epi8<VT>((char)0xff));

			b += sizeof(VT);
			g += sizeof(VT);
			r += sizeof(VT);
			if (A)
				a += sizeof(VT);
		}

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

#ifdef GENERATE_SSE41
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
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

template<int F, typename VT, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
static inline FORCEINLINE VECTOR_UV<VT>  VECTORCALL tuned_ConvertPackedUVToPlanarElement(__m128i m0, __m128i m1)
{
	__m128i ctl = _mm_set_epi8(15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8, 6, 4, 2, 0);

	m0 = _mm_shuffle_epi8(m0, ctl);
	m1 = _mm_shuffle_epi8(m1, ctl);

	__m128i uu = _mm_unpacklo_epi64(m0, m1);
	__m128i vv = _mm_unpackhi_epi64(m0, m1);

	return { uu, vv };
}

template<int F, typename VT>
static inline FORCEINLINE VECTOR_UV<VT> tuned_ConvertPackedUVToPlanarElement(const uint8_t* pp)
{
	return tuned_ConvertPackedUVToPlanarElement<F, VT>(
		_mmt_loadu<VT>(pp),
		_mmt_loadu<VT>(pp + sizeof(VT))
	);
}

template<int F, typename VT>
static inline FORCEINLINE VECTOR_UV<VT> tuned_ConvertPackedUVToPlanarElement(const uint8_t* pp, ssize_t scbStride)
{
	return tuned_ConvertPackedUVToPlanarElement<F, VT>(
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp), _mmt_loadu<VT>(pp - scbStride)),
		_mmt_sub_epi8<VT>(_mmt_loadu<VT>(pp + sizeof(VT)), _mmt_loadu<VT>(pp - scbStride + sizeof(VT)))
	);
}

//

template<int F, typename VT, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
static inline FORCEINLINE VECTOR2<__m128i> VECTORCALL tuned_ConvertPlanarToPackedUVElement(__m128i uu, __m128i vv)
{
	return {
		_mm_unpacklo_epi8(uu, vv),
		_mm_unpackhi_epi8(uu, vv)
	};
}

template<int F, typename VT>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarToPackedUVElement(uint8_t* pp, VT uu, VT vv)
{
	auto result = tuned_ConvertPlanarToPackedUVElement<F, VT>(uu, vv);
	_mmt_storeu<VT>(pp, result.v0);
	_mmt_storeu<VT>(pp + sizeof(VT), result.v1);
}

template<int F, typename VT>
static inline FORCEINLINE void VECTORCALL tuned_ConvertPlanarToPackedUVElement(uint8_t* pp, VT uu, VT vv, ssize_t scbStride)
{
	auto result = tuned_ConvertPlanarToPackedUVElement<F, VT>(uu, vv);
	_mmt_storeu<VT>(pp, _mmt_add_epi8<VT>(result.v0, _mmt_loadu<VT>(pp - scbStride)));
	_mmt_storeu<VT>(pp + sizeof(VT), _mmt_add_epi8<VT>(result.v1, _mmt_loadu<VT>(pp - scbStride + sizeof(VT))));
}

//

template<VALUERANGE VR> static inline FORCEINLINE __m128i _mm_Convert16To10(__m128i x);
template<VALUERANGE VR> static inline FORCEINLINE __m128i _mm_Convert10To16(__m128i x);

static inline FORCEINLINE __m128i _mm_Convert16To10Fullrange(__m128i x)
{
	return _mm_srli_epi16(_mm_sub_epi16(_mm_add_epi16(x, _mm_set1_epi16(1 << 5)), _mm_srli_epi16(x, 10)), 6);
}

static inline FORCEINLINE __m128i _mm_Convert10To16Fullrange(__m128i x)
{
	__m128i tmp = _mm_slli_epi16(x, 6);
	return _mm_or_si128(tmp, _mm_srli_epi16(tmp, 10));
}

static inline FORCEINLINE __m128i _mm_Convert16To10Limited(__m128i x)
{
	return _mm_srli_epi16(_mm_adds_epu16(x, _mm_set1_epi16(1 << 5)), 6);
}

static inline FORCEINLINE __m128i _mm_Convert16To10Noround(__m128i x)
{
	return _mm_srli_epi16(x, 6);
}

static inline FORCEINLINE __m128i _mm_Convert10To16Limited(__m128i x)
{
	return _mm_slli_epi16(x, 6);
}

template<> static inline FORCEINLINE __m128i _mm_Convert16To10<VALUERANGE::FULL>(__m128i x)
{
	return _mm_Convert16To10Fullrange(x);
}

template<> static inline FORCEINLINE __m128i _mm_Convert16To10<VALUERANGE::LIMITED>(__m128i x)
{
	return _mm_Convert16To10Limited(x);
}

template<> static inline FORCEINLINE __m128i _mm_Convert16To10<VALUERANGE::NOROUND>(__m128i x)
{
	return _mm_Convert16To10Noround(x);
}

template<> static inline FORCEINLINE __m128i _mm_Convert10To16<VALUERANGE::FULL>(__m128i x)
{
	return _mm_Convert10To16Fullrange(x);
}

template<> static inline FORCEINLINE __m128i _mm_Convert10To16<VALUERANGE::LIMITED>(__m128i x)
{
	return _mm_Convert10To16Limited(x);
}


template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler> /* �œK�����L���ȏꍇ�A�Ԃ��� a ��G��Ȃ���� a ���v�Z���閽�߂͐�������Ȃ��̂ŁA��͂� A �̓e���v���[�g�p�����[�^�Ƃ��Ă͗v��Ȃ� */
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
	using VT = __m128i;

	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *a = (uint16_t *)pABegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		for (; pp <= p + cbWidth - sizeof(VT) * 4; pp += sizeof(VT) * 4)
		{
			auto result = tuned_ConvertB64aToPlanarElement10<F, VT, true>(pp);

			_mmt_storeu<VT>(b, result.b);
			_mmt_storeu<VT>(g, result.g);
			_mmt_storeu<VT>(r, result.r);
			if (A)
				_mmt_storeu<VT>(a, result.a);

			b += sizeof(VT) / 2;
			g += sizeof(VT) / 2;
			r += sizeof(VT) / 2;
			if (A)
				a += sizeof(VT) / 2;
		}

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

#ifdef GENERATE_SSE41
//template void tuned_ConvertRGBToUQRG<CODEFEATURE_SSE41, CB48rColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToUQRG<CODEFEATURE_SSE41, CB64aColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertB64aToUQRA<CODEFEATURE_SSE41>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
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
	aa = _mm_Convert10To16Fullrange(aa); // tuned_ConvertPlanarRGBXToPackedElement �Ɠ��l�A aa ���萔�̏ꍇ�ł����̍s�͂����܂Ŗ��ɂȂ�Ȃ��B

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
	using VT = __m128i;

	const uint16_t *r = (const uint16_t *)pRBegin;
	const uint16_t *g = (const uint16_t *)pGBegin;
	const uint16_t *b = (const uint16_t *)pBBegin;
	const uint16_t *a = (const uint16_t *)pABegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		for (; pp <= p + cbWidth - sizeof(VT) * 4; pp += sizeof(VT) * 4)
		{
			VT gg = _mmt_loadu<VT>(g);
			VT bb = _mmt_loadu<VT>(b);
			VT rr = _mmt_loadu<VT>(r);
			VT aa;
			if (A)
				aa = _mmt_loadu<VT>(a);
			else
				aa = _mmt_set1_epi16<VT>((short)0xffff);

			tuned_ConvertPlanarRGBXToB64aElement10<F, VT, true>(pp, gg, bb, rr, aa);

			b += sizeof(VT) / 2;
			g += sizeof(VT) / 2;
			r += sizeof(VT) / 2;
			if (A)
				a += sizeof(VT) / 2;
		}

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


#ifdef GENERATE_SSE41
//template void tuned_ConvertUQRGToRGB<CODEFEATURE_SSE41, CB48rColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRGToRGB<CODEFEATURE_SSE41, CB64aColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRAToB64a<CODEFEATURE_SSE41>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
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
	using VT = __m128i;

	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + nWidth * 4;
		const uint8_t *p = pStrideBegin;

		for (; p <= pStrideEnd - sizeof(VT) * 2; p += sizeof(VT) * 2)
		{
			auto result = tuned_ConvertR210ToPlanarElement10<F, VT, true>(p);
			_mmt_storeu<VT>(g, result.g);
			_mmt_storeu<VT>(b, result.b);
			_mmt_storeu<VT>(r, result.r);

			g += sizeof(VT) / 2;
			b += sizeof(VT) / 2;
			r += sizeof(VT) / 2;
		}

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

#ifdef GENERATE_SSE41
template void tuned_ConvertR210ToUQRG<CODEFEATURE_SSE41>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertR210ToUQRG<CODEFEATURE_AVX1>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
#endif

//

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m128i>>*& = enabler>
static inline FORCEINLINE VECTOR2<__m128i> VECTORCALL tuned_ConvertPlanarRGBXToR210Element10(__m128i gg, __m128i bb, __m128i rr)
{
	__m128i ggtmp = NeedOffset ? _mm_add_epi16(gg, _mm_set1_epi16(0x200)) : gg;
	gg = _mm_and_si128(gg, _mm_set1_epi16(0x3ff));
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
	using VT = __m128i;

	const uint16_t *g = (const uint16_t *)pGBegin;
	const uint16_t *b = (const uint16_t *)pBBegin;
	const uint16_t *r = (const uint16_t *)pRBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + nWidth * 4;
		uint8_t *p = pStrideBegin;

		for (; p <= pStrideEnd - sizeof(VT) * 2; p += sizeof(VT) * 2)
		{
			VT gg = _mmt_loadu<VT>(g);
			VT bb = _mmt_loadu<VT>(b);
			VT rr = _mmt_loadu<VT>(r);
			tuned_ConvertPlanarRGBXToR210Element10<F, VT, true>(p, gg, bb, rr);

			g += sizeof(VT) / 2;
			b += sizeof(VT) / 2;
			r += sizeof(VT) / 2;
		}

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

#ifdef GENERATE_SSE41
template void tuned_ConvertUQRGToR210<CODEFEATURE_SSE41>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertUQRGToR210<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif

//

template<int F, VALUERANGE VR>
void tuned_ConvertLittleEndian16ToHostEndian10(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd)
{
	auto p = (const uint16_t*)pSrcBegin;
	auto q = (uint16_t*)pDst;

	for (; p <= (const uint16_t*)pSrcEnd - 8; p += 8, q += 8)
	{
		auto v = _mm_loadu_si128((const __m128i*)p);
		v = _mm_Convert16To10<VR>(v);
		_mm_storeu_si128((__m128i*)q, v);
	}

	for (; p < (const uint16_t*)pSrcEnd; ++p, ++q)
		*q = Convert16To10<VR>(ltoh16(*p));
}

template<int F, VALUERANGE VR>
void tuned_ConvertHostEndian10ToLittleEndian16(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrc)
{
	auto p = (const uint16_t*)pSrc;
	auto q = (uint16_t*)pDstBegin;

	for (; q <= (uint16_t*)pDstEnd - 8; p += 8, q += 8)
	{
		auto v = _mm_loadu_si128((const __m128i*)p);
		v = _mm_Convert10To16<VR>(v);
		_mm_storeu_si128((__m128i*)q, v);
	}

	for (; q < (uint16_t*)pDstEnd; ++p, ++q)
		*q = htol16(Convert10To16<VR>(*p));
}

#ifdef GENERATE_SSE41
template void tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_SSE41, VALUERANGE::NOROUND>(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertHostEndian10ToLittleEndian16<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrc);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_AVX1, VALUERANGE::NOROUND>(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertHostEndian10ToLittleEndian16<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrc);
#endif

//

template<int F, VALUERANGE VR>
void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd)
{
	auto u = (uint16_t*)pUBegin;
	auto v = (uint16_t*)pVBegin;
	auto p = (const uint16_t*)pSrcBegin;

	for (; p <= (const uint16_t*)pSrcEnd - 16; p += 16, u += 8, v += 8)
	{
		auto m0 = _mm_loadu_si128((const __m128i*)p);
		auto m1 = _mm_loadu_si128((const __m128i*)(p + 8));
		m0 = _mm_Convert16To10<VR>(m0);
		m1 = _mm_Convert16To10<VR>(m1);
		m0 = _mm_shuffle_epi8(m0, _mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0));
		m1 = _mm_shuffle_epi8(m1, _mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0));
		auto uu = _mm_unpacklo_epi64(m0, m1);
		auto vv = _mm_unpackhi_epi64(m0, m1);
		_mm_storeu_si128((__m128i*)u, uu);
		_mm_storeu_si128((__m128i*)v, vv);
	}

	for (; p < (const uint16_t*)pSrcEnd; p += 2, ++u, ++v)
	{
		*u = Convert16To10<VR>(ltoh16(p[0]));
		*v = Convert16To10<VR>(ltoh16(p[1]));
	}
}

template<int F, VALUERANGE VR>
void tuned_ConvertPlanarHostEndian10ToPackedUVLittleEndian16(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin)
{
	auto u = (const uint16_t*)pUBegin;
	auto v = (const uint16_t*)pVBegin;
	auto p = (uint16_t*)pDstBegin;

	for (; p <= (uint16_t*)pDstEnd - 16; p += 16, u += 8, v += 8)
	{
		auto uu = _mm_loadu_si128((const __m128i*)u);
		auto vv = _mm_loadu_si128((const __m128i*)v);
		uu = _mm_Convert10To16<VR>(uu);
		vv = _mm_Convert10To16<VR>(vv);
		auto m0 = _mm_unpacklo_epi16(uu, vv);
		auto m1 = _mm_unpackhi_epi16(uu, vv);
		_mm_storeu_si128((__m128i*)p, m0);
		_mm_storeu_si128((__m128i*)(p + 8), m1);
	}

	for (; p < (uint16_t*)pDstEnd; p += 2, ++u, ++v)
	{
		p[0] = htol16(Convert10To16<VR>(*u));
		p[1] = htol16(Convert10To16<VR>(*v));
	}
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_SSE41, VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertPlanarHostEndian10ToPackedUVLittleEndian16<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_AVX1, VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void tuned_ConvertPlanarHostEndian10ToPackedUVLittleEndian16<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin);
#endif
