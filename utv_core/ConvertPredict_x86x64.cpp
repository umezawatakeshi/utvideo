/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2)
#error
#endif

template<int F, bool Gradient>
static inline void tuned_ConvertBGRToULRG_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	typedef CBGRColorOrder T;

	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;

	for (auto p = pSrcBegin; p != (Gradient ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			auto planar = tuned_ConvertPackedBGRToPlanarElement<F, false>(pp);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;

			b += 16;
			g += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
#endif

		for (; pp < p + cbWidth; pp += 3)
		{
			uint8_t gg = pp[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;

			b += 1;
			g += 1;
			r += 1;
		}
	}

	if (Gradient) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			auto planar = tuned_ConvertPackedBGRToPlanarElement<F, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32)))
			);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;

			b += 16;
			g += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
#endif

		for (; pp < p + cbWidth; pp += 3)
		{
			uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;

			b += 1;
			g += 1;
			r += 1;
		}
	}
}

template<int F, class T, bool A, bool Gradient>
static inline void tuned_ConvertRGBXToULRX_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;
	uint8_t aprevb = 0x80;

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;
	auto a = pABegin;

	for (auto p = pSrcBegin; p != (Gradient ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);
		__m128i aprev;
		if (A)
			aprev = _mm_slli_si128(_mm_cvtsi32_si128(aprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, T, A, false>(pp);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictLeftAndCount8Element<F>(aprev, planar.a, pACountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
		if (A)
			aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 15));
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t gg = pp[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A) {
				uint8_t aa = pp[T::A];
				*a = aa - aprevb;
				++pACountTable[*a];
				aprevb = aa;
			}

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (Gradient) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();
		__m128i aprev;
		if (A)
			aprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, T, A, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48)))
			);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictLeftAndCount8Element<F>(aprev, planar.a, pACountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
		if (A)
			aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 15));
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A) {
				uint8_t aa = pp[T::A] - (pp - scbStride)[T::A];
				*a = aa - aprevb;
				++pACountTable[*a];
				aprevb = aa;
			}

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertBGRToULRG_PredictAndCount<F, false>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
	else
		tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, false>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, false>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertBGRToULRG_PredictAndCount<F, true>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
	else
		tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, true>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, true>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
#endif

//

template<int F, bool Gradient>
static inline void tuned_ConvertULRGToBGR_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	typedef CBGRColorOrder T;

	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;

	auto g = pGBegin;
	auto b = pBBegin;
	auto r = pRBegin;

	for (auto p = pDstBegin; p != (Gradient ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_set1_epi8(gprevb);
		__m128i bprev = _mm_set1_epi8(bprevb);
		__m128i rprev = _mm_set1_epi8(rprevb);

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			tuned_ConvertPlanarBGRToPackedElement<F, false>(pp, gvalue.v0, bvalue.v0, rvalue.v0);

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);
#endif

		for (; pp < p + cbWidth; pp += 3)
		{
			pp[T::G] = gprevb += g[0];
			pp[T::B] = (bprevb += b[0]) + gprevb;
			pp[T::R] = (rprevb += r[0]) + gprevb;

			g += 1;
			b += 1;
			r += 1;
		}
	}

	if (Gradient) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_set1_epi8(0);
		__m128i bprev = _mm_set1_epi8(0);
		__m128i rprev = _mm_set1_epi8(0);

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			auto packed = tuned_ConvertPlanarBGRToPackedElement<F, false>(gvalue.v0, bvalue.v0, rvalue.v0);
			_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(packed.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(packed.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(packed.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);
#endif

		for (; pp < p + cbWidth; pp += 3)
		{
			pp[T::G] = (gprevb += g[0]) + (pp - scbStride)[T::G];
			pp[T::B] = (bprevb += b[0]) + (pp - scbStride)[T::B] + gprevb;
			pp[T::R] = (rprevb += r[0]) + (pp - scbStride)[T::R] + gprevb;

			g += 1;
			b += 1;
			r += 1;
		}
	}
}

template<int F, class T, bool A, bool Gradient>
static inline void tuned_ConvertULRXToRGBX_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;
	uint8_t aprevb = 0x80;

	auto g = pGBegin;
	auto b = pBBegin;
	auto r = pRBegin;
	auto a = pABegin;

	for (auto p = pDstBegin; p != (Gradient ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_set1_epi8(gprevb);
		__m128i bprev = _mm_set1_epi8(bprevb);
		__m128i rprev = _mm_set1_epi8(rprevb);
		__m128i aprev;
		if (A)
			aprev = _mm_set1_epi8(aprevb);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			__m128i avalue_v0;
			if (A)
			{
				__m128i aa = _mm_loadu_si128((const __m128i *)a);
				auto avalue = tuned_RestoreLeft8Element<F>(aprev, aa);
				aprev = avalue.v1;
				avalue_v0 = avalue.v0;
			}
			else
			{
				avalue_v0 = _mm_set1_epi8((char)0xff);
			}
			tuned_ConvertPlanarRGBXToPackedElement<F, T, false>(pp, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);


			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);
		if (A)
			aprevb = _mm_cvtsi128_si32(aprev);
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::G] = gprevb += g[0];
			pp[T::B] = (bprevb += b[0]) + gprevb;
			pp[T::R] = (rprevb += r[0]) + gprevb;
			if (A)
				pp[T::A] = aprevb += a[0];
			else
				pp[T::A] = 0xff;

			g += 1;
			b += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (Gradient) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i gprev = _mm_set1_epi8(0);
		__m128i bprev = _mm_set1_epi8(0);
		__m128i rprev = _mm_set1_epi8(0);
		__m128i aprev;
		if (A)
			aprev = _mm_set1_epi8(0);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			__m128i avalue_v0;
			if (A)
			{
				__m128i aa = _mm_loadu_si128((const __m128i *)a);
				auto avalue = tuned_RestoreLeft8Element<F>(aprev, aa);
				aprev = avalue.v1;
				avalue_v0 = avalue.v0;
			}
			else
			{
				avalue_v0 = _mm_set1_epi8(0);
			}
			auto packed = tuned_ConvertPlanarRGBXToPackedElement<F, T, false>(gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);
			_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(packed.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(packed.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(packed.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));
			_mm_storeu_si128((__m128i *)(pp + 48), _mm_add_epi8(packed.v3, _mm_loadu_si128((__m128i *)(pp - scbStride + 48))));

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);
		if (A)
			aprevb = _mm_cvtsi128_si32(aprev);
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::G] = (gprevb += g[0]) + (pp - scbStride)[T::G];
			pp[T::B] = (bprevb += b[0]) + (pp - scbStride)[T::B] + gprevb;
			pp[T::R] = (rprevb += r[0]) + (pp - scbStride)[T::R] + gprevb;
			if (A)
				pp[T::A] = (aprevb += a[0]) + (pp - scbStride)[T::A];
			else
				pp[T::A] = 0xff;

			g += 1;
			b += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertULRGToBGR_Restore<F, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
	else
		tuned_ConvertULRXToRGBX_Restore<F, T, false, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertULRGToBGR_Restore<F, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
	else
		tuned_ConvertULRXToRGBX_Restore<F, T, false, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, class T, bool Gradient>
static inline void tuned_ConvertPackedYUV422ToULY2_PredictAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	uint8_t yprevb = 0x80;
	uint8_t uprevb = 0x80;
	uint8_t vprevb = 0x80;

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pSrcBegin; p != (Gradient ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i yprev = _mm_slli_si128(_mm_cvtsi32_si128(yprevb), 15);
		__m128i uprev = _mm_slli_si128(_mm_cvtsi32_si128(uprevb), 15);
		__m128i vprev = _mm_slli_si128(_mm_cvtsi32_si128(vprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(pp);
			_mm_storeu_si128((__m128i *)y, tuned_PredictLeftAndCount8Element<F>(yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictLeftAndCount8Element<F>(planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictLeftAndCount8Element<F>(uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictLeftAndCount8Element<F>(vprev, planar.v, pVCountTable));

			yprev = planar.y1;
			uprev = planar.u;
			vprev = planar.v;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(_mm_srli_si128(yprev, 15));
		uprevb = _mm_cvtsi128_si32(_mm_srli_si128(uprev, 15));
		vprevb = _mm_cvtsi128_si32(_mm_srli_si128(vprev, 15));
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0];
			*y = yy0 - yprevb;
			++pYCountTable[*y];
			uint8_t yy1 = pp[T::Y1];
			*(y + 1) = yy1 - yy0;
			++pYCountTable[*(y + 1)];
			uint8_t uu = pp[T::U];
			*u = uu - uprevb;
			++pUCountTable[*u];
			uint8_t vv = pp[T::V];
			*v = vv - vprevb;
			++pVCountTable[*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;

			y += 2;
			u += 1;
			v += 1;
		}
	}

	if (Gradient) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i yprev = _mm_setzero_si128();
		__m128i uprev = _mm_setzero_si128();
		__m128i vprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48)))
			);
			_mm_storeu_si128((__m128i *)y, tuned_PredictLeftAndCount8Element<F>(yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictLeftAndCount8Element<F>(planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictLeftAndCount8Element<F>(uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictLeftAndCount8Element<F>(vprev, planar.v, pVCountTable));

			yprev = planar.y1;
			uprev = planar.u;
			vprev = planar.v;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(_mm_srli_si128(yprev, 15));
		uprevb = _mm_cvtsi128_si32(_mm_srli_si128(uprev, 15));
		vprevb = _mm_cvtsi128_si32(_mm_srli_si128(vprev, 15));
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0] - (pp - scbStride)[T::Y0];
			*y = yy0 - yprevb;
			++pYCountTable[*y];
			uint8_t yy1 = pp[T::Y1] - (pp - scbStride)[T::Y1];
			*(y + 1) = yy1 - yy0;
			++pYCountTable[*(y + 1)];
			uint8_t uu = pp[T::U] - (pp - scbStride)[T::U];
			*u = uu - uprevb;
			++pUCountTable[*u];
			uint8_t vv = pp[T::V] - (pp - scbStride)[T::V];
			*v = vv - vprevb;
			++pVCountTable[*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;

			y += 2;
			u += 1;
			v += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, false>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, true>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
#endif

//

template<int F, class T, bool Gradient>
static inline void tuned_ConvertULY2ToPackedYUV422_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t yprevb = 0x80;
	uint8_t uprevb = 0x80;
	uint8_t vprevb = 0x80;

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pDstBegin; p != (Gradient ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i yprev = _mm_set1_epi8(yprevb);
		__m128i uprev = _mm_set1_epi8(uprevb);
		__m128i vprev = _mm_set1_epi8(vprevb);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			auto yvalue = tuned_RestoreLeft8Element<F>(yprev, yy0, yy1);
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uu);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vv);
			tuned_ConvertPlanarYUV422ToPackedElement<F, T>(pp, yvalue.v0, yvalue.v1, uvalue.v0, vvalue.v0);

			yprev = yvalue.v2;
			uprev = uvalue.v1;
			vprev = vvalue.v1;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(yprev);
		uprevb = _mm_cvtsi128_si32(uprev);
		vprevb = _mm_cvtsi128_si32(vprev);
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::Y0] = yprevb += y[0];
			pp[T::Y1] = yprevb += y[1];
			pp[T::U] = uprevb += u[0];
			pp[T::V] = vprevb += v[0];

			y += 2;
			u += 1;
			v += 1;
		}
	}

	if (Gradient) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

#ifdef __SSSE3__
		__m128i yprev = _mm_set1_epi8(0);
		__m128i uprev = _mm_set1_epi8(0);
		__m128i vprev = _mm_set1_epi8(0);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			auto yvalue = tuned_RestoreLeft8Element<F>(yprev, yy0, yy1);
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uu);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vv);
			auto packed = tuned_ConvertPlanarYUV422ToPackedElement<F, T>(yvalue.v0, yvalue.v1, uvalue.v0, vvalue.v0);
			_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(packed.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(packed.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(packed.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));
			_mm_storeu_si128((__m128i *)(pp + 48), _mm_add_epi8(packed.v3, _mm_loadu_si128((__m128i *)(pp - scbStride + 48))));

			yprev = yvalue.v2;
			uprev = uvalue.v1;
			vprev = vvalue.v1;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(yprev);
		uprevb = _mm_cvtsi128_si32(uprev);
		vprevb = _mm_cvtsi128_si32(vprev);
#endif

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::Y0] = (yprevb += y[0]) + (pp - scbStride)[T::Y0];
			pp[T::Y1] = (yprevb += y[1]) + (pp - scbStride)[T::Y1];
			pp[T::U] = (uprevb += u[0]) + (pp - scbStride)[T::U];
			pp[T::V] = (vprevb += v[0]) + (pp - scbStride)[T::V];

			y += 2;
			u += 1;
			v += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, false>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, true>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif
