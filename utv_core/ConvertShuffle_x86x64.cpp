/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2) && !defined(GENERATE_AVX512_ICL)
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

static inline void print_zmm(const char* s, __m512i x)
{
	union {
		__m512i zmm;
		uint8_t b[64];
	};
	zmm = x;
	printf("%s =", s);
	for (int i = 0; i < 64; ++i)
		printf(" %02X", b[i]);
	printf("\n");
}

template<int F, typename VT, class T, typename std::enable_if_t<std::is_same_v<VT, __m512i>>*& = enabler>
static inline FORCEINLINE VECTOR_YUV422<__m512i> VECTORCALL tuned_ConvertPackedYUV422ToPlanarElement(__m512i m0, __m512i m1, __m512i m2, __m512i m3)
{
	__m512i ctly, ctluv0, ctluv1;
	if (std::is_same<T, CYUYVColorOrder>::value)
	{
		ctly = _mm512_set_epi8(
			126, 124, 122, 120, 118, 116, 114, 112,	110, 108, 106, 104, 102, 100,  98,  96,
			 94,  92,  90,  88,  86,  84,  82,  80,	 78,  76,  74,  72,  70,  68,  66,  64,
			 62,  60,  58,  56,  54,  52,  50,  48,	 46,  44,  42,  40,  38,  36,  34,  32,
			 30,  28,  26,  24,  22,  20,  18,  16,	 14,  12,  10,   8,   6,   4,   2,   0
		);
		ctluv0 = _mm512_set_epi8(
			127, 123, 119, 115, 111, 107, 103,  99,	 95,  91,  87,  83,  79,  75,  71,  67,
			 63,  59,  55,  51,  47,  43,  39,  35,	 31,  27,  23,  19,  15,  11,   7,   3,
			125, 121, 117, 113, 109, 105, 101,  97,	 93,  89,  85,  81,  77,  73,  69,  65,
			 61,  57,  53,  49,  45,  41,  37,  33,	 29,  25,  21,  17,  13,   9,   5,   1
		);
		ctluv1 = _mm512_set_epi8(
			125, 121, 117, 113, 109, 105, 101,  97,	 93,  89,  85,  81,  77,  73,  69,  65,
			 61,  57,  53,  49,  45,  41,  37,  33,	 29,  25,  21,  17,  13,   9,   5,   1,
			127, 123, 119, 115, 111, 107, 103,  99,	 95,  91,  87,  83,  79,  75,  71,  67,
			 63,  59,  55,  51,  47,  43,  39,  35,	 31,  27,  23,  19,  15,  11,   7,   3
		);
	}
	else
	{
		ctly = _mm512_set_epi8(
			127, 125, 123, 121, 119, 117, 115, 113,	111, 109, 107, 105, 103, 101,  99,  97,
			 95,  93,  91,  89,  87,  85,  83,  81,	 79,  77,  75,  73,  71,  69,  67,  65,
			 63,  61,  59,  57,  55,  53,  51,  49,	 47,  45,  43,  41,  39,  37,  35,  33,
			 31,  29,  27,  25,  23,  21,  19,  17,	 15,  13,  11,   9,   7,   5,   3,   1
		);
		ctluv0 = _mm512_set_epi8(
			126, 122, 118, 114, 110, 106, 102,  98,	 94,  90,  86,  82,  78,  74,  70,  66,
			 62,  58,  54,  50,  46,  42,  38,  34,	 30,  26,  22,  18,  14,  10,   6,   2,
			124, 120, 116, 112, 108, 104, 100,  96,	 92,  88,  84,  80,  76,  72,  68,  64,
			 60,  56,  52,  48,  44,  40,  36,  32,	 28,  24,  20,  16,  12,   8,   4,   0
		);
		ctluv1 = _mm512_set_epi8(
			124, 120, 116, 112, 108, 104, 100,  96,	 92,  88,  84,  80,  76,  72,  68,  64,
			 60,  56,  52,  48,  44,  40,  36,  32,	 28,  24,  20,  16,  12,   8,   4,   0,
			126, 122, 118, 114, 110, 106, 102,  98,	 94,  90,  86,  82,  78,  74,  70,  66,
			 62,  58,  54,  50,  46,  42,  38,  34,	 30,  26,  22,  18,  14,  10,   6,   2
		);
	}

	__m512i yy0 = _mm512_permutex2var_epi8(m0, ctly, m1);
	__m512i yy1 = _mm512_permutex2var_epi8(m2, ctly, m3);
	__m512i uv0 = _mm512_permutex2var_epi8(m0, ctluv0, m1);
//	__m512i uv1 = _mm512_permutex2var_epi8(m2, ctluv, m3);
	__m512i uv1 = _mm512_permutex2var_epi8(m2, ctluv1, m3);

//	__m512i uu = _mm512_inserti64x4(uv0, _mm512_castsi512_si256(uv1), 1);
//	__m512i vv = _mm512_permutex2var_epi64(uv0, _mm512_set_epi64(15, 14, 13, 12, 7, 6, 5, 4), uv1);
	__m512i uu = _mm512_mask_mov_epi64(uv0, 0xf0, uv1);
	__m512i vv = _mm512_permutex2var_epi64(uv0, _mm512_set_epi64(11, 10, 9, 8, 7, 6, 5, 4), uv1);

// ↑どっちが速いかは不明

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

#ifdef GENERATE_AVX512_ICL
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX512_ICL, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX512_ICL, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
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

template<int F, typename VT, class T, typename std::enable_if_t<std::is_same_v<VT, __m512i>>*& = enabler>
static inline FORCEINLINE VECTOR4<__m512i> VECTORCALL tuned_ConvertPlanarYUV422ToPackedElement(__m512i yy0, __m512i yy1, __m512i uu, __m512i vv)
{
	__m512i ctl0, ctl1;
	if (std::is_same<T, CYUYVColorOrder>::value)
	{
		ctl0 = _mm512_set_epi8(
			111,  31,  79,  30, 110,  29,  78,  28,	109,  27,  77,  26, 108,  25,  76,  24,
			107,  23,  75,  22, 106,  21,  74,  20,	105,  19,  73,  18, 104,  17,  72,  16,
			103,  15,  71,  14, 102,  13,  70,  12,	101,  11,  69,  10, 100,   9,  68,   8,
			 99,   7,  67,   6,  98,   5,  66,   4,	 97,   3,  65,   2,  96,   1,  64,   0
		);
		ctl1 = _mm512_set_epi8(
			127,  63,  95,  62, 126,  61,  94,  60,	125,  59,  93,  58, 124,  57,  92,  56,
			123,  55,  91,  54, 122,  53,  90,  52,	121,  51,  89,  50, 120,  49,  88,  48,
			119,  47,  87,  46, 118,  45,  86,  44,	117,  43,  85,  42, 116,  41,  84,  40,
			115,  39,  83,  38, 114,  37,  82,  36,	113,  35,  81,  34, 112,  33,  80,  32
		);
	}
	else
	{
		ctl0 = _mm512_set_epi8(
			 31, 111,  30,  79,  29, 110,  28,  78,	 27, 109,  26,  77,  25, 108,  24,  76,
			 23, 107,  22,  75,  21, 106,  20,  74,	 19, 105,  18,  73,  17, 104,  16,  72,
			 15, 103,  14,  71,  13, 102,  12,  70,	 11, 101,  10,  69,   9, 100,   8,  68,
			  7,  99,   6,  67,   5,  98,   4,  66,	  3,  97,   2,  65,   1,  96,   0,  64
		);
		ctl1 = _mm512_set_epi8(
			 63, 127,  62,  95,  61, 126,  60,  94,	 59, 125,  58,  93,  57, 124,  56,  92,
			 55, 123,  54,  91,  53, 122,  52,  90,	 51, 121,  50,  89,  49, 120,  48,  88,
			 47, 119,  46,  87,  45, 118,  44,  86,	 43, 117,  42,  85,  41, 116,  40,  84,
			 39, 115,  38,  83,  37, 114,  36,  82,	 35, 113,  34,  81,  33, 112,  32,  80
		);
	}

	__m512i uv0 = _mm512_inserti64x4(uu, _mm512_castsi512_si256(vv), 1);
	__m512i uv1 = _mm512_permutex2var_epi64(uu, _mm512_set_epi64(15, 14, 13, 12, 7, 6, 5, 4), vv);

	__m512i m0 = _mm512_permutex2var_epi8(yy0, ctl0, uv0);
	__m512i m1 = _mm512_permutex2var_epi8(yy0, ctl1, uv0);
	__m512i m2 = _mm512_permutex2var_epi8(yy1, ctl0, uv1);
	__m512i m3 = _mm512_permutex2var_epi8(yy1, ctl1, uv1);

	return { m0, m1, m2, m3 };
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

#ifdef GENERATE_AVX512_ICL
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX512_ICL, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX512_ICL, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m512i>>*& = enabler>
static inline FORCEINLINE VECTOR_RGB<__m512i> tuned_ConvertPackedBGRToPlanarElement(__m512i m0, __m512i m1, __m512i m2)
{
	__m512i ctlb = _mm512_set_epi8(
		125, 122, 119, 116, 113, 110, 107, 104, 101,  98,  95,  92,  89,  86,  83,  80,  77,  74,  71,  68,  65,
		 62,  59,  56,  53,  50,  47,  44,  41,  38,  35,  32,  29,  26,  23,  20,  17,  14,  11,   8,   5,   2,
		 63,  60,  57,  54,  51,  48,  45,  42,  39,  36,  33,  30,  27,  24,  21,  18,  15,  12,   9,   6,   3,   0
	);
	__m512i ctlg = _mm512_set_epi8(
		 62,  59,  56,  53,  50,  47,  44,  41,  38,  35,  32,  29,  26,  23,  20,  17,  14,  11,   8,   5,   2,
		 63,  60,  57,  54,  51,  48,  45,  42,  39,  36,  33,  30,  27,  24,  21,  18,  15,  12,   9,   6,   3,   0,
		125, 122, 119, 116, 113, 110, 107, 104, 101,  98,  95,  92,  89,  86,  83,  80,  77,  74,  71,  68,  65
);
	__m512i ctlr = _mm512_set_epi8(
		 63,  60,  57,  54,  51,  48,  45,  42,  39,  36,  33,  30,  27,  24,  21,  18,  15,  12,   9,   6,   3,   0,
		125, 122, 119, 116, 113, 110, 107, 104, 101,  98,  95,  92,  89,  86,  83,  80,  77,  74,  71,  68,  65,
		 62,  59,  56,  53,  50,  47,  44,  41,  38,  35,  32,  29,  26,  23,  20,  17,  14,  11,   8,   5,   2
	);
	__mmask64 k = 0x4924924924924924ULL;

	__m512i o0 = _mm512_mask_mov_epi8(m0, k, m1);
	__m512i o1 = _mm512_mask_mov_epi8(m1, k, m2);
	__m512i o2 = _mm512_mask_mov_epi8(m2, k, m0);

	__m512i bb = _mm512_permutex2var_epi8(o0, ctlb, o2);
	__m512i gg = _mm512_permutex2var_epi8(o1, ctlg, o0);
	__m512i rr = _mm512_permutex2var_epi8(o2, ctlr, o1);

	__m512i ggtmp = NeedOffset ? _mm512_add_epi8(gg, _mm512_set1_epi8((char)0x80)) : gg;
	bb = _mm512_sub_epi8(bb, ggtmp);
	rr = _mm512_sub_epi8(rr, ggtmp);

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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m512i>>*& = enabler>
static inline FORCEINLINE VECTOR_RGBA<__m512i> VECTORCALL tuned_ConvertPackedRGBXToPlanarElement(__m512i m0, __m512i m1, __m512i m2, __m512i m3)
{
	__m512i ctlgb0, ctlgb1, ctlar0, ctlar1;
	if (std::is_same<T, CBGRAColorOrder>::value)
	{
		ctlgb0 = _mm512_set_epi8(
			125, 121, 117, 113, 109, 105, 101,  97,	 93,  89,  85,  81,  77,  73,  69,  65,
			 61,  57,  53,  49,  45,  41,  37,  33,	 29,  25,  21,  17,  13,   9,   5,   1,
			124, 120, 116, 112, 108, 104, 100,  96,	 92,  88,  84,  80,  76,  72,  68,  64,
			 60,  56,  52,  48,  44,  40,  36,  32,	 28,  24,  20,  16,  12,   8,   4,   0
		);
		ctlgb1 = _mm512_set_epi8(
			124, 120, 116, 112, 108, 104, 100,  96,	 92,  88,  84,  80,  76,  72,  68,  64,
			 60,  56,  52,  48,  44,  40,  36,  32,	 28,  24,  20,  16,  12,   8,   4,   0,
			125, 121, 117, 113, 109, 105, 101,  97,	 93,  89,  85,  81,  77,  73,  69,  65,
			 61,  57,  53,  49,  45,  41,  37,  33,	 29,  25,  21,  17,  13,   9,   5,   1
		);
		ctlar0 = _mm512_set_epi8(
			127, 123, 119, 115, 111, 107, 103,  99,	 95,  91,  87,  83,  79,  75,  71,  67,
			 63,  59,  55,  51,  47,  43,  39,  35,	 31,  27,  23,  19,  15,  11,   7,   3,
			126, 122, 118, 114, 110, 106, 102,  98,	 94,  90,  86,  82,  78,  74,  70,  66,
			 62,  58,  54,  50,  46,  42,  38,  34,	 30,  26,  22,  18,  14,  10,   6,   2
		);
		ctlar1 = _mm512_set_epi8(
			126, 122, 118, 114, 110, 106, 102,  98,	 94,  90,  86,  82,  78,  74,  70,  66,
			 62,  58,  54,  50,  46,  42,  38,  34,	 30,  26,  22,  18,  14,  10,   6,   2,
			127, 123, 119, 115, 111, 107, 103,  99,	 95,  91,  87,  83,  79,  75,  71,  67,
			 63,  59,  55,  51,  47,  43,  39,  35,	 31,  27,  23,  19,  15,  11,   7,   3
		);
	}
	else
	{
		ctlgb0 = _mm512_set_epi8(
			126, 122, 118, 114, 110, 106, 102,  98,	 94,  90,  86,  82,  78,  74,  70,  66,
			 62,  58,  54,  50,  46,  42,  38,  34,	 30,  26,  22,  18,  14,  10,   6,   2,
			127, 123, 119, 115, 111, 107, 103,  99,	 95,  91,  87,  83,  79,  75,  71,  67,
			 63,  59,  55,  51,  47,  43,  39,  35,	 31,  27,  23,  19,  15,  11,   7,   3
		);
		ctlgb1 = _mm512_set_epi8(
			127, 123, 119, 115, 111, 107, 103,  99,	 95,  91,  87,  83,  79,  75,  71,  67,
			 63,  59,  55,  51,  47,  43,  39,  35,	 31,  27,  23,  19,  15,  11,   7,   3,
			126, 122, 118, 114, 110, 106, 102,  98,	 94,  90,  86,  82,  78,  74,  70,  66,
			 62,  58,  54,  50,  46,  42,  38,  34,	 30,  26,  22,  18,  14,  10,   6,   2
		);
		ctlar0 = _mm512_set_epi8(
			124, 120, 116, 112, 108, 104, 100,  96,	 92,  88,  84,  80,  76,  72,  68,  64,
			 60,  56,  52,  48,  44,  40,  36,  32,	 28,  24,  20,  16,  12,   8,   4,   0,
			125, 121, 117, 113, 109, 105, 101,  97,	 93,  89,  85,  81,  77,  73,  69,  65,
			 61,  57,  53,  49,  45,  41,  37,  33,	 29,  25,  21,  17,  13,   9,   5,   1
		);
		ctlar1 = _mm512_set_epi8(
			125, 121, 117, 113, 109, 105, 101,  97,	 93,  89,  85,  81,  77,  73,  69,  65,
			 61,  57,  53,  49,  45,  41,  37,  33,	 29,  25,  21,  17,  13,   9,   5,   1,
			124, 120, 116, 112, 108, 104, 100,  96,	 92,  88,  84,  80,  76,  72,  68,  64,
			 60,  56,  52,  48,  44,  40,  36,  32,	 28,  24,  20,  16,  12,   8,   4,   0
		);
	}

	__m512i gb0 = _mm512_permutex2var_epi8(m0, ctlgb0, m1);
//	__m512i gb1 = _mm512_permutex2var_epi8(m2, ctlgb, m3);
	__m512i gb1 = _mm512_permutex2var_epi8(m2, ctlgb1, m3);
	__m512i ar0 = _mm512_permutex2var_epi8(m0, ctlar0, m1);
//	__m512i ar1 = _mm512_permutex2var_epi8(m2, ctlar, m3);
	__m512i ar1 = _mm512_permutex2var_epi8(m2, ctlar1, m3);

//	__m512i bb = _mm512_inserti64x4(gb0, _mm512_castsi512_si256(gb1), 1);
//	__m512i gg = _mm512_permutex2var_epi64(gb0, _mm512_set_epi64(15, 14, 13, 12, 7, 6, 5, 4), gb1);
//	__m512i rr = _mm512_inserti64x4(ar0, _mm512_castsi512_si256(ar1), 1);
//	__m512i aa = _mm512_permutex2var_epi64(ar0, _mm512_set_epi64(15, 14, 13, 12, 7, 6, 5, 4), ar1);
	__m512i bb = _mm512_mask_mov_epi64(gb0, 0xf0, gb1);
	__m512i gg = _mm512_permutex2var_epi64(gb0, _mm512_set_epi64(11, 10, 9, 8, 7, 6, 5, 4), gb1);
	__m512i rr = _mm512_mask_mov_epi64(ar0, 0xf0, ar1);
	__m512i aa = _mm512_permutex2var_epi64(ar0, _mm512_set_epi64(11, 10, 9, 8, 7, 6, 5, 4), ar1);

// ↑どっちが速いかは不明

	__m512i ggtmp = NeedOffset ? _mm512_add_epi8(gg, _mm512_set1_epi8((char)0x80)) : gg;
	bb = _mm512_sub_epi8(bb, ggtmp);
	rr = _mm512_sub_epi8(rr, ggtmp);

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

#ifdef GENERATE_AVX512_ICL
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX512_ICL, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBToULRG<CODEFEATURE_AVX512_ICL, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertRGBAToULRA<CODEFEATURE_AVX512_ICL, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
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

template<int F, typename VT, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m512i>>*& = enabler>
static inline FORCEINLINE VECTOR3<__m512i> tuned_ConvertPlanarBGRToPackedElement(__m512i gg, __m512i bb, __m512i rr)
{
	__m512i ctl0 = _mm512_set_epi8(
		 21,  42,  84,  20,  41,  83,  19,  40,  82,  18,  39,  81,  17,  38,  80,
		 16,  37,  79,  15,  36,  78,  14,  35,  77,  13,  34,  76,  12,  33,  75,
		 11,  32,  74,  10,  31,  73,   9,  30,  72,   8,  29,  71,   7,  28,  70,
		  6,  27,  69,   5,  26,  68,   4,  25,  67,   3,  24,  66,   2,  23,  65,
		  1,  22,  64,   0
	);
	__m512i ctl1 = _mm512_set_epi8(
		 42,  63, 105,  41,  62, 104,  40,  61, 103,  39,  60, 102,  38,  59, 101,
		 37,  58, 100,  36,  57,  99,  35,  56,  98,  34,  55,  97,  33,  54,  96,
		 32,  53,  95,  31,  52,  94,  30,  51,  93,  29,  50,  92,  28,  49,  91,
		 27,  48,  90,  26,  47,  89,  25,  46,  88,  24,  45,  87,  23,  44,  86,
		 22,  43,  85,  21
	);
	__m512i ctl2 = _mm512_set_epi8(
		 63,  20, 127,  62,  19, 126,  61,  18, 125,  60,  17, 124,  59,  16, 123,
		 58,  15, 122,  57,  14, 121,  56,  13, 120,  55,  12, 119,  54,  11, 118,
		 53,  10, 117,  52,   9, 116,  51,   8, 115,  50,   7, 114,  49,   6, 113,
		 48,   5, 112,  47,   4, 111,  46,   3, 110,  45,   2, 109,  44,   1, 108,
		 43,   0, 107,  42
	);
	__mmask64 k = 0x4924924924924924ULL;

	__m512i ggtmp = NeedOffset ? _mm512_add_epi8(gg, _mm512_set1_epi8((char)0x80)) : gg;
	bb = _mm512_add_epi8(bb, ggtmp);
	rr = _mm512_add_epi8(rr, ggtmp);

	__m512i m0 = _mm512_permutex2var_epi8(bb, ctl0, gg);
	__m512i m1 = _mm512_permutex2var_epi8(gg, ctl1, rr);
	__m512i m2 = _mm512_permutex2var_epi8(rr, ctl2, bb);

	__m512i n0 = _mm512_mask_mov_epi8(m0, k, m2);
	__m512i n1 = _mm512_mask_mov_epi8(m1, k, m0);
	__m512i n2 = _mm512_mask_mov_epi8(m2, k, m1);

	return { n0, n1, n2 };
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

template<int F, typename VT, class T, bool NeedOffset, typename std::enable_if_t<std::is_same_v<VT, __m512i>>*& = enabler>
static inline FORCEINLINE VECTOR4<__m512i> VECTORCALL tuned_ConvertPlanarRGBXToPackedElement(__m512i gg, __m512i bb, __m512i rr, __m512i aa)
{
	__m512i ctl0, ctl1;
	if (std::is_same<T, CBGRAColorOrder>::value)
	{
		ctl0 = _mm512_set_epi8(
			111,  79,  47,  15, 110,  78,  46,  14, 109,  77,  45,  13, 108,  76,  44,  12,
			107,  75,  43,  11, 106,  74,  42,  10, 105,  73,  41,   9, 104,  72,  40,   8,
			103,  71,  39,   7, 102,  70,  38,   6, 101,  69,  37,   5, 100,  68,  36,   4,
			 99,  67,  35,   3,  98,  66,  34,   2,  97,  65,  33,   1,  96,  64,  32,   0
		);
		ctl1 = _mm512_set_epi8(
			127,  95,  63,  31, 126,  94,  62,  30, 125,  93,  61,  29, 124,  92,  60,  28,
			123,  91,  59,  27, 122,  90,  58,  26, 121,  89,  57,  25, 120,  88,  56,  24,
			119,  87,  55,  23, 118,  86,  54,  22, 117,  85,  53,  21, 116,  84,  52,  20,
			115,  83,  51,  19, 114,  82,  50,  18, 113,  81,  49,  17, 112,  80,  48,  16
		);
	}
	else
	{
		ctl0 = _mm512_set_epi8(
			 15,  47,  79, 111,  14,  46,  78, 110,  13,  45,  77, 109,  12,  44,  76, 108,
			 11,  43,  75, 107,  10,  42,  74, 106,   9,  41,  73, 105,   8,  40,  72, 104,
			  7,  39,  71, 103,   6,  38,  70, 102,   5,  37,  69, 101,   4,  36,  68, 100,
			  3,  35,  67,  99,   2,  34,  66,  98,   1,  33,  65,  97,   0,  32,  64,  96
		);
		ctl1 = _mm512_set_epi8(
			 31,  63,  95, 127,  30,  62,  94, 126,  29,  61,  93, 125,  28,  60,  92, 124,
			 27,  59,  91, 123,  26,  58,  90, 122,  25,  57,  89, 121,  24,  56,  88, 120,
			 23,  55,  87, 119,  22,  54,  86, 118,  21,  53,  85, 117,  20,  52,  84, 116,
			 19,  51,  83, 115,  18,  50,  82, 114,  17,  49,  81, 113,  16,  48,  80, 112
		);
	}

	__m512i ggtmp = NeedOffset ? _mm512_add_epi8(gg, _mm512_set1_epi8((char)0x80)) : gg;
	bb = _mm512_add_epi8(bb, ggtmp);
	rr = _mm512_add_epi8(rr, ggtmp);

	__m512i gb0 = _mm512_inserti64x4(bb, _mm512_castsi512_si256(gg), 1);
	__m512i gb1 = _mm512_permutex2var_epi64(bb, _mm512_set_epi64(15, 14, 13, 12, 7, 6, 5, 4), gg);
	__m512i ar0 = _mm512_inserti64x4(rr, _mm512_castsi512_si256(aa), 1);
	__m512i ar1 = _mm512_permutex2var_epi64(rr, _mm512_set_epi64(15, 14, 13, 12, 7, 6, 5, 4), aa);

	__m512i m0 = _mm512_permutex2var_epi8(gb0, ctl0, ar0);
	__m512i m1 = _mm512_permutex2var_epi8(gb0, ctl1, ar0);
	__m512i m2 = _mm512_permutex2var_epi8(gb1, ctl0, ar1);
	__m512i m3 = _mm512_permutex2var_epi8(gb1, ctl1, ar1);

	return { m0, m1, m2, m3 };
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

#ifdef GENERATE_AVX512_ICL
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX512_ICL, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRGToRGB<CODEFEATURE_AVX512_ICL, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void tuned_ConvertULRAToRGBA<CODEFEATURE_AVX512_ICL, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
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

#ifdef GENERATE_SSSE3
template void tuned_ConvertUQRGToR210<CODEFEATURE_SSSE3>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertUQRGToR210<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif
