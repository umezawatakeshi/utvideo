/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

template<int F, class T, bool A, PREDICTION_TYPE Pred>
static inline void tuned_ConvertRGBXToULRX_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;
	uint8_t aprevb = 0x80;

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;
	auto a = pABegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);
		__m128i aprev;
		if (A)
			aprev = _mm_slli_si128(_mm_cvtsi32_si128(aprevb), 15);

		for (; pp <= p + cbWidth - T::BYPP * 16; pp += T::BYPP * 16)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, __m128i, T, false>(pp);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F, true, A ? 4 : 2, 0>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F, true, 4, 2>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F, true, 4, 3>(rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictLeftAndCount8Element<F, true, 4, 1>(aprev, planar.a, pACountTable));

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

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			uint8_t gg = pp[T::G];
			*g = gg - gprevb;
			++pGCountTable[0][*g];
			uint8_t bb = pp[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[0][*b];
			uint8_t rr = pp[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[0][*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A)
			{
				uint8_t aa = pp[T::A];
				*a = aa - aprevb;
				++pACountTable[0][*a];
				aprevb = aa;
			}

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();
		__m128i aprev;
		if (A)
			aprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - T::BYPP * 16; pp += T::BYPP * 16)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, __m128i, T, false>(pp, scbStride);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F, true, A ? 4 : 2, 0>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F, true, 4, 2>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F, true, 4, 3>(rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictLeftAndCount8Element<F, true, 4, 1>(aprev, planar.a, pACountTable));

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

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
			*g = gg - gprevb;
			++pGCountTable[0][*g];
			uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[0][*b];
			uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[0][*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A)
			{
				uint8_t aa = pp[T::A] - (pp - scbStride)[T::A];
				*a = aa - aprevb;
				++pACountTable[0][*a];
				aprevb = aa;
			}

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	gprevb = 0;
	bprevb = 0;
	rprevb = 0;
	if (A)
		aprevb = 0;

	uint8_t gtopprevb = 0;
	uint8_t btopprevb = 0;
	uint8_t rtopprevb = 0;
	uint8_t atopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);
		__m128i aprev;
		if (A)
			aprev = _mm_slli_si128(_mm_cvtsi32_si128(aprevb), 15);
		__m128i gtopprev = _mm_slli_si128(_mm_cvtsi32_si128(gtopprevb), 15);
		__m128i btopprev = _mm_slli_si128(_mm_cvtsi32_si128(btopprevb), 15);
		__m128i rtopprev = _mm_slli_si128(_mm_cvtsi32_si128(rtopprevb), 15);
		__m128i atopprev;
		if (A)
			atopprev = _mm_slli_si128(_mm_cvtsi32_si128(atopprevb), 15);

		for (; pp <= p + cbWidth - T::BYPP * 16; pp += T::BYPP * 16)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, __m128i, T, true>(pp);
			auto top = tuned_ConvertPackedRGBXToPlanarElement<F, __m128i, T, true>(pp - scbStride);
			_mm_storeu_si128((__m128i *)g, tuned_PredictWrongMedianAndCount8Element<F, true, A ? 4 : 2, 0>(gtopprev, top.g, gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictWrongMedianAndCount8Element<F, true, 4, 2>(btopprev, top.b, bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictWrongMedianAndCount8Element<F, true, 4, 3>(rtopprev, top.r, rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictWrongMedianAndCount8Element<F, true, 4, 1>(atopprev, top.a, aprev, planar.a, pACountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;
			gtopprev = top.g;
			btopprev = top.b;
			rtopprev = top.r;
			if (A)
				atopprev = top.a;

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
		gtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(gtopprev, 15));
		btopprevb = _mm_cvtsi128_si32(_mm_srli_si128(btopprev, 15));
		rtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(rtopprev, 15));
		if (A)
			atopprevb = _mm_cvtsi128_si32(_mm_srli_si128(atopprev, 15));

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			uint8_t gg = pp[T::G];
			uint8_t gtop = (pp - scbStride)[T::G];
			*g = gg - median<uint8_t>(gprevb, gtop, gprevb + gtop - gtopprevb);
			++pGCountTable[0][*g];
			uint8_t bb = pp[T::B] - gg + 0x80;
			uint8_t btop = (pp - scbStride)[T::B] - gtop + 0x80;
			*b = bb - median<uint8_t>(bprevb, btop, bprevb + btop - btopprevb);
			++pBCountTable[0][*b];
			uint8_t rr = pp[T::R] - gg + 0x80;
			uint8_t rtop = (pp - scbStride)[T::R] - gtop + 0x80;
			*r = rr - median<uint8_t>(rprevb, rtop, rprevb + rtop - rtopprevb);
			++pRCountTable[0][*r];
			if (A)
			{
				uint8_t aa = pp[T::A];
				uint8_t atop = (pp - scbStride)[T::A];
				*a = aa - median<uint8_t>(aprevb, atop, aprevb + atop - atopprevb);
				++pACountTable[0][*a];
				aprevb = aa;
				atopprevb = atop;
			}

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			gtopprevb = gtop;
			btopprevb = btop;
			rtopprevb = rtop;

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256])
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256])
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256])
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, CYLINDRICAL_WRONG_MEDIAN>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, CYLINDRICAL_WRONG_MEDIAN>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
#endif

//

template<int F, class T, bool A, PREDICTION_TYPE Pred, bool NTSTORE>
static inline void tuned_ConvertULRXToRGBX_RestoreImpl(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;
	uint8_t aprevb = 0x80;

	auto g = pGBegin;
	auto b = pBBegin;
	auto r = pRBegin;
	auto a = pABegin;

	uint8_t* linebuf = NULL;
	if (NTSTORE && Pred == PLANAR_GRADIENT)
		linebuf = (uint8_t*)_aligned_malloc(cbWidth, 16);

	__m128i gprev = _mm_set1_epi8(gprevb);
	__m128i bprev = _mm_set1_epi8(bprevb);
	__m128i rprev = _mm_set1_epi8(rprevb);
	__m128i aprev;
	if (A)
		aprev = _mm_set1_epi8(aprevb);

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		if (!NTSTORE)
		{
			gprev = _mm_set1_epi8(gprevb);
			bprev = _mm_set1_epi8(bprevb);
			rprev = _mm_set1_epi8(rprevb);
			if (A)
				aprev = _mm_set1_epi8(aprevb);
		}

		auto lb = linebuf;
		for (; pp <= p + cbWidth - T::BYPP * 16; pp += T::BYPP * 16, lb += T::BYPP * 16)
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
			if (!NTSTORE || Pred != PLANAR_GRADIENT)
				tuned_ConvertPlanarRGBXToPackedElement<F, __m128i, T, false, NTSTORE>(pp, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);
			else
				tuned_ConvertPlanarRGBXToPackedElement<F, __m128i, T, false, NTSTORE>(pp, lb, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);


			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
			if (A)
				a += 16;
		}

		if (!NTSTORE)
		{
			gprevb = _mm_cvtsi128_si32(gprev);
			bprevb = _mm_cvtsi128_si32(bprev);
			rprevb = _mm_cvtsi128_si32(rprev);
			if (A)
				aprevb = _mm_cvtsi128_si32(aprev);

			for (; pp < p + cbWidth; pp += T::BYPP)
			{
				pp[T::G] = gprevb += g[0];
				pp[T::B] = (bprevb += b[0]) + gprevb;
				pp[T::R] = (rprevb += r[0]) + gprevb;
				if (A)
					pp[T::A] = aprevb += a[0];
				else if (T::HAS_ALPHA)
					pp[T::A] = 0xff;

				g += 1;
				b += 1;
				r += 1;
				if (A)
					a += 1;
			}
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_set1_epi8(0);
		__m128i bprev = _mm_set1_epi8(0);
		__m128i rprev = _mm_set1_epi8(0);
		__m128i aprev;
		if (A)
			aprev = _mm_set1_epi8(0);

		auto lb = linebuf;
		for (; pp <= p + cbWidth - T::BYPP * 16; pp += T::BYPP * 16, lb += T::BYPP * 16)
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
			if (!NTSTORE)
				tuned_ConvertPlanarRGBXToPackedElement<F, __m128i, T, false>(pp, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0, scbStride);
			else
				tuned_ConvertPlanarRGBXToPackedElement<F, __m128i, T, false, NTSTORE>(pp, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0, lb);

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
			if (A)
				a += 16;
		}

		if (!NTSTORE)
		{
			gprevb = _mm_cvtsi128_si32(gprev);
			bprevb = _mm_cvtsi128_si32(bprev);
			rprevb = _mm_cvtsi128_si32(rprev);
			if (A)
				aprevb = _mm_cvtsi128_si32(aprev);

			for (; pp < p + cbWidth; pp += T::BYPP)
			{
				pp[T::G] = (gprevb += g[0]) + (pp - scbStride)[T::G];
				pp[T::B] = (bprevb += b[0]) + (pp - scbStride)[T::B] + gprevb;
				pp[T::R] = (rprevb += r[0]) + (pp - scbStride)[T::R] + gprevb;
				if (A)
					pp[T::A] = (aprevb += a[0]) + (pp - scbStride)[T::A];
				else if (T::HAS_ALPHA)
					pp[T::A] = 0xff;

				g += 1;
				b += 1;
				r += 1;
				if (A)
					a += 1;
			}
		}
	}

	gprevb = 0;
	bprevb = 0;
	rprevb = 0;
	if (A)
		aprevb = 0;

	uint8_t gtopprevb = 0;
	uint8_t btopprevb = 0;
	uint8_t rtopprevb = 0;
	uint8_t atopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i left;
		__m128i prev;
		__m128i topprev;
		__m128i ctl;

		if (std::is_same<T, CBGRAColorOrder>::value)
			ctl = _mm_set_epi8(-1, 13, -1, 13, -1, 9, -1, 9, -1, 5, -1, 5, -1, 1, -1, 1);
		else if (std::is_same<T, CARGBColorOrder>::value)
			ctl = _mm_set_epi8(14, -1, 14, -1, 10, -1, 10, -1, 6, -1, 6, -1, 2, -1, 2, -1);
		else if (std::is_same<T, CBGRColorOrder>::value)
			ctl = _mm_set_epi8(-1, -1, -1, -1, 10, -1, 10, 7, -1, 7, 4, -1, 4, 1, -1, 1);

		if (std::is_same<T, CBGRAColorOrder>::value)
		{
			left = _mm_cvtsi32_si128((gprevb << 8) | (bprevb) | (rprevb << 16) | (A ? aprevb << 24 : 0));
			prev = _mm_slli_si128(left, 12);
			topprev = _mm_slli_si128(_mm_cvtsi32_si128((gtopprevb << 8) | (btopprevb) | (rtopprevb << 16) | (A ? atopprevb << 24 : 0)), 12);
		}
		else if (std::is_same<T, CARGBColorOrder>::value)
		{
			left = _mm_cvtsi32_si128((gprevb << 16) | (bprevb << 24) | (rprevb << 8) | (A ? aprevb : 0));
			prev = _mm_slli_si128(left, 12);
			topprev = _mm_slli_si128(_mm_cvtsi32_si128((gtopprevb << 16) | (btopprevb << 24) | (rtopprevb << 8) | (A ? atopprevb : 0)), 12);
		}
		else if (std::is_same<T, CBGRColorOrder>::value)
		{
			left = _mm_cvtsi32_si128((gprevb << 8) | (bprevb) | (rprevb << 16));
			prev = _mm_slli_si128(left, 13);
			topprev = _mm_slli_si128(_mm_cvtsi32_si128((gtopprevb << 8) | (btopprevb) | (rtopprevb << 16)), 13);
		}

		for (; pp <= p + cbWidth - 16 /* Ç±Ç±ÇÕ T::BYPP * 4 Ç≈ÇÕÇ»Ç≠ xmm ÇÃÉåÉWÉXÉ^ïùÇ≈Ç†ÇÈ 16 Ç≈Ç»Ç¢Ç∆Ç¢ÇØÇ»Ç¢ */; pp += T::BYPP * 4)
		{
			__m128i gg = _mm_cvtsi32_si128(*(const uint32_t *)g);
			__m128i bb = _mm_cvtsi32_si128(*(const uint32_t *)b);
			__m128i rr = _mm_cvtsi32_si128(*(const uint32_t *)r);
			__m128i aa = A ? _mm_cvtsi32_si128(*(const uint32_t *)a) : _mm_set1_epi8(0);

			__m128i residual;
			__m128i top;
			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				residual = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, aa));
				__m128i toptmp = _mm_loadu_si128((const __m128i *)(pp - scbStride));
				top = _mm_sub_epi8(_mm_add_epi8(toptmp, _mm_set1_epi32(0x00800080)), _mm_shuffle_epi8(toptmp, ctl));
			}
			else if (std::is_same<T, CARGBColorOrder>::value)
			{
				residual = _mm_unpacklo_epi16(_mm_unpacklo_epi8(aa, rr), _mm_unpacklo_epi8(gg, bb));
				__m128i toptmp = _mm_loadu_si128((const __m128i *)(pp - scbStride));
				top = _mm_sub_epi8(_mm_add_epi8(toptmp, _mm_set1_epi32(0x80008000)), _mm_shuffle_epi8(toptmp, ctl));
			}
			else if (std::is_same<T, CBGRColorOrder>::value)
			{
				residual = _mm_shuffle_epi8(_mm_unpacklo_epi64(_mm_unpacklo_epi32(bb, gg), rr), _mm_set_epi8(-1, -1, -1, -1, 11, 7, 3, 10, 6, 2, 9, 5, 1, 8, 4, 0));
				__m128i toptmp = _mm_loadu_si128((const __m128i *)(pp - scbStride));
				top = _mm_sub_epi8(_mm_add_epi8(toptmp, _mm_set_epi8(0, 0, 0, 0, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80)), _mm_shuffle_epi8(toptmp, ctl));
			}

			__m128i value;
			__m128i grad;
			__m128i pred;
			__m128i topleft;

			topleft = _mm_alignr_epi8(top, topprev, 16 - T::BYPP);
			__m128i top_minus_topleft = _mm_sub_epi8(top, topleft);

			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 16 - T::BYPP);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 16 - T::BYPP);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 16 - T::BYPP);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			if (std::is_same<T, CBGRColorOrder>::value)
			{
				_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(_mm_sub_epi8(value, _mm_set_epi8(0, 0, 0, 0, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80)), _mm_shuffle_epi8(value, ctl)));

				prev = _mm_alignr_epi8(value, value, 12);
				left = _mm_alignr_epi8(value, value, 9);
				topprev = _mm_alignr_epi8(top, top, 12);
			}
			else
			{
				if (std::is_same<T, CBGRAColorOrder>::value)
				{
					_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(_mm_sub_epi8(value, _mm_set1_epi32(0x00800080)), _mm_shuffle_epi8(value, ctl)));
				}
				else
				{
					_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(_mm_sub_epi8(value, _mm_set1_epi32(0x80008000)), _mm_shuffle_epi8(value, ctl)));
				}

				prev = value;
				left = _mm_alignr_epi8(value, value, 12);
				topprev = top;
			}

			b += 4;
			g += 4;
			r += 4;
			if (A)
				a += 4;
		}

		if (std::is_same<T, CBGRAColorOrder>::value)
		{
			uint32_t tmp = _mm_cvtsi128_si32(left);
			gprevb = tmp >> 8;
			bprevb = tmp;
			rprevb = tmp >> 16;
			if (A)
				aprevb = tmp >> 24;

			uint32_t toptmp = _mm_cvtsi128_si32(_mm_srli_si128(topprev, 12));
			gtopprevb = toptmp >> 8;
			btopprevb = toptmp;
			rtopprevb = toptmp >> 16;
			if (A)
				atopprevb = toptmp >> 24;
		}
		else if (std::is_same<T, CARGBColorOrder>::value)
		{
			uint32_t tmp = _mm_cvtsi128_si32(left);
			gprevb = tmp >> 16;
			bprevb = tmp >> 24;
			rprevb = tmp >> 8;
			if (A)
				aprevb = tmp;

			uint32_t toptmp = _mm_cvtsi128_si32(_mm_srli_si128(topprev, 12));
			gtopprevb = toptmp >> 16;
			btopprevb = toptmp >> 24;
			rtopprevb = toptmp >> 8;
			if (A)
				atopprevb = toptmp;
		}
		else if (std::is_same<T, CBGRColorOrder>::value)
		{
			uint32_t tmp = _mm_cvtsi128_si32(left);
			gprevb = tmp >> 8;
			bprevb = tmp;
			rprevb = tmp >> 16;

			uint32_t toptmp = _mm_cvtsi128_si32(_mm_srli_si128(topprev, 13));
			gtopprevb = toptmp >> 8;
			btopprevb = toptmp;
			rtopprevb = toptmp >> 16;
		}

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			uint8_t gtop = (pp - scbStride)[T::G];
			uint8_t gg = *g + median<uint8_t>(gprevb, gtop, gprevb + gtop - gtopprevb);
			pp[T::G] = gg;
			uint8_t btop = (pp - scbStride)[T::B] - gtop + 0x80;
			uint8_t bb = *b + median<uint8_t>(bprevb, btop, bprevb + btop - btopprevb);
			pp[T::B] = bb + gg - 0x80;
			uint8_t rtop = (pp - scbStride)[T::R] - gtop + 0x80;
			uint8_t rr = *r + median<uint8_t>(rprevb, rtop, rprevb + rtop - rtopprevb);
			pp[T::R] = rr + gg - 0x80;
			if (A)
			{
				uint8_t atop = (pp - scbStride)[T::A];
				uint8_t aa = *a + median<uint8_t>(aprevb, atop, aprevb + atop - atopprevb);
				pp[T::A] = aa;
				aprevb = aa;
				atopprevb = atop;
			}
			else if (T::HAS_ALPHA)
				pp[T::A] = 0xff;

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			gtopprevb = gtop;
			btopprevb = btop;
			rtopprevb = rtop;

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (NTSTORE && Pred == PLANAR_GRADIENT)
		_aligned_free(linebuf);
}

template<int F, class T, bool A, PREDICTION_TYPE Pred>
static inline void tuned_ConvertULRXToRGBX_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, const uint8_t* pABegin, size_t cbWidth, ssize_t scbStride)
{
	if (Pred != CYLINDRICAL_WRONG_MEDIAN && IS_ALIGNED(pDstBegin, 16) && IS_MULTIPLE(cbWidth, T::BYPP * 16) && IS_MULTIPLE(scbStride, 16))
		tuned_ConvertULRXToRGBX_RestoreImpl<F, T, A, Pred, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
	else
		tuned_ConvertULRXToRGBX_RestoreImpl<F, T, A, Pred, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, false, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, false, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, false, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
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
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
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
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, class T, PREDICTION_TYPE Pred>
static inline void tuned_ConvertPackedYUV422ToULY2_PredictAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	uint8_t yprevb = 0x80;
	uint8_t uprevb = 0x80;
	uint8_t vprevb = 0x80;

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_slli_si128(_mm_cvtsi32_si128(yprevb), 15);
		__m128i uprev = _mm_slli_si128(_mm_cvtsi32_si128(uprevb), 15);
		__m128i vprev = _mm_slli_si128(_mm_cvtsi32_si128(vprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, __m128i, T>(pp);
			_mm_storeu_si128((__m128i *)y, tuned_PredictLeftAndCount8Element<F, true, 2, 0>(yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictLeftAndCount8Element<F, true, 2, 0>(planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictLeftAndCount8Element<F, true, 4, 2>(uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictLeftAndCount8Element<F, true, 4, 3>(vprev, planar.v, pVCountTable));

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

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0];
			*y = yy0 - yprevb;
			++pYCountTable[0][*y];
			uint8_t yy1 = pp[T::Y1];
			*(y + 1) = yy1 - yy0;
			++pYCountTable[1][*(y + 1)];
			uint8_t uu = pp[T::U];
			*u = uu - uprevb;
			++pUCountTable[0][*u];
			uint8_t vv = pp[T::V];
			*v = vv - vprevb;
			++pVCountTable[0][*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;

			y += 2;
			u += 1;
			v += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_setzero_si128();
		__m128i uprev = _mm_setzero_si128();
		__m128i vprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, __m128i, T>(pp, scbStride);
			_mm_storeu_si128((__m128i *)y, tuned_PredictLeftAndCount8Element<F, true, 2, 0>(yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictLeftAndCount8Element<F, true, 2, 0>(planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictLeftAndCount8Element<F, true, 4, 2>(uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictLeftAndCount8Element<F, true, 4, 3>(vprev, planar.v, pVCountTable));

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

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0] - (pp - scbStride)[T::Y0];
			*y = yy0 - yprevb;
			++pYCountTable[0][*y];
			uint8_t yy1 = pp[T::Y1] - (pp - scbStride)[T::Y1];
			*(y + 1) = yy1 - yy0;
			++pYCountTable[1][*(y + 1)];
			uint8_t uu = pp[T::U] - (pp - scbStride)[T::U];
			*u = uu - uprevb;
			++pUCountTable[0][*u];
			uint8_t vv = pp[T::V] - (pp - scbStride)[T::V];
			*v = vv - vprevb;
			++pVCountTable[0][*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;

			y += 2;
			u += 1;
			v += 1;
		}
	}

	yprevb = 0;
	uprevb = 0;
	vprevb = 0;

	uint8_t ytopprevb = 0;
	uint8_t utopprevb = 0;
	uint8_t vtopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_slli_si128(_mm_cvtsi32_si128(yprevb), 15);
		__m128i uprev = _mm_slli_si128(_mm_cvtsi32_si128(uprevb), 15);
		__m128i vprev = _mm_slli_si128(_mm_cvtsi32_si128(vprevb), 15);
		__m128i ytopprev = _mm_slli_si128(_mm_cvtsi32_si128(ytopprevb), 15);
		__m128i utopprev = _mm_slli_si128(_mm_cvtsi32_si128(utopprevb), 15);
		__m128i vtopprev = _mm_slli_si128(_mm_cvtsi32_si128(vtopprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, __m128i, T>(pp);
			auto top = tuned_ConvertPackedYUV422ToPlanarElement<F, __m128i, T>(pp - scbStride);
			_mm_storeu_si128((__m128i *)y, tuned_PredictWrongMedianAndCount8Element<F, true, 2, 0>(ytopprev, top.y0, yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictWrongMedianAndCount8Element<F, true, 2, 0>(top.y0, top.y1, planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictWrongMedianAndCount8Element<F, true, 4, 2>(utopprev, top.u, uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictWrongMedianAndCount8Element<F, true, 4, 3>(vtopprev, top.v, vprev, planar.v, pVCountTable));

			yprev = planar.y1;
			uprev = planar.u;
			vprev = planar.v;
			ytopprev = top.y1;
			utopprev = top.u;
			vtopprev = top.v;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(_mm_srli_si128(yprev, 15));
		uprevb = _mm_cvtsi128_si32(_mm_srli_si128(uprev, 15));
		vprevb = _mm_cvtsi128_si32(_mm_srli_si128(vprev, 15));
		ytopprevb = _mm_cvtsi128_si32(_mm_srli_si128(ytopprev, 15));
		utopprevb = _mm_cvtsi128_si32(_mm_srli_si128(utopprev, 15));
		vtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(vtopprev, 15));

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0];
			uint8_t ytop0 = (pp - scbStride)[T::Y0];
			*y = yy0 - median<uint8_t>(yprevb, ytop0, yprevb + ytop0 - ytopprevb);
			++pYCountTable[0][*y];
			uint8_t yy1 = pp[T::Y1];
			uint8_t ytop1 = (pp - scbStride)[T::Y1];
			*(y + 1) = yy1 - median<uint8_t>(yy0, ytop1, yy0 + ytop1 - ytop0);
			++pYCountTable[1][*(y + 1)];
			uint8_t uu = pp[T::U];
			uint8_t utop = (pp - scbStride)[T::U];
			*u = uu - median<uint8_t>(uprevb, utop, uprevb + utop - utopprevb);
			++pUCountTable[0][*u];
			uint8_t vv = pp[T::V];
			uint8_t vtop = (pp - scbStride)[T::V];
			*v = vv - median<uint8_t>(vprevb, vtop, vprevb + vtop - vtopprevb);
			++pVCountTable[0][*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;
			ytopprevb = ytop1;
			utopprevb = utop;
			vtopprevb = vtop;

			y += 2;
			u += 1;
			v += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, CYLINDRICAL_LEFT>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, PLANAR_GRADIENT>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, CYLINDRICAL_WRONG_MEDIAN>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
#endif

//

template<int F, class T, PREDICTION_TYPE Pred, bool NTSTORE>
static inline void tuned_ConvertULY2ToPackedYUV422_RestoreImpl(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t yprevb = 0x80;
	uint8_t uprevb = 0x80;
	uint8_t vprevb = 0x80;

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	uint8_t* linebuf = NULL;
	if (NTSTORE && Pred == PLANAR_GRADIENT)
		linebuf = (uint8_t*)_aligned_malloc(cbWidth, 16);

	__m128i yprev = _mm_set1_epi8(yprevb);
	__m128i uprev = _mm_set1_epi8(uprevb);
	__m128i vprev = _mm_set1_epi8(vprevb);

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		if (!NTSTORE)
		{
			yprev = _mm_set1_epi8(yprevb);
			uprev = _mm_set1_epi8(uprevb);
			vprev = _mm_set1_epi8(vprevb);
		}

		auto lb = linebuf;
		for (; pp <= p + cbWidth - 64; pp += 64, lb += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			auto yvalue0 = tuned_RestoreLeft8Element<F>(yprev, yy0);
			auto yvalue1 = tuned_RestoreLeft8Element<F>(yvalue0.v1, yy1);
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uu);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vv);
			if (!NTSTORE || Pred != PLANAR_GRADIENT)
				tuned_ConvertPlanarYUV422ToPackedElement<F, __m128i, T, NTSTORE>(pp, yvalue0.v0, yvalue1.v0, uvalue.v0, vvalue.v0);
			else
				tuned_ConvertPlanarYUV422ToPackedElement<F, __m128i, T, NTSTORE>(pp, lb, yvalue0.v0, yvalue1.v0, uvalue.v0, vvalue.v0);

			yprev = yvalue1.v1;
			uprev = uvalue.v1;
			vprev = vvalue.v1;

			y += 32;
			u += 16;
			v += 16;
		}

		if (!NTSTORE)
		{
			yprevb = _mm_cvtsi128_si32(yprev);
			uprevb = _mm_cvtsi128_si32(uprev);
			vprevb = _mm_cvtsi128_si32(vprev);

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
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_set1_epi8(0);
		__m128i uprev = _mm_set1_epi8(0);
		__m128i vprev = _mm_set1_epi8(0);

		auto lb = linebuf;
		for (; pp <= p + cbWidth - 64; pp += 64, lb += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			auto yvalue0 = tuned_RestoreLeft8Element<F>(yprev, yy0);
			auto yvalue1 = tuned_RestoreLeft8Element<F>(yvalue0.v1, yy1);
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uu);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vv);
			if (!NTSTORE)
				tuned_ConvertPlanarYUV422ToPackedElement<F, __m128i, T>(pp, yvalue0.v0, yvalue1.v0, uvalue.v0, vvalue.v0, scbStride);
			else
				tuned_ConvertPlanarYUV422ToPackedElement<F, __m128i, T, NTSTORE>(pp, yvalue0.v0, yvalue1.v0, uvalue.v0, vvalue.v0, lb);

			yprev = yvalue1.v1;
			uprev = uvalue.v1;
			vprev = vvalue.v1;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(yprev);
		uprevb = _mm_cvtsi128_si32(uprev);
		vprevb = _mm_cvtsi128_si32(vprev);

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

	__m128i prev = _mm_set1_epi8(0);
	__m128i topprev = _mm_set1_epi8(0);

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i ctl_fromprev;
		__m128i ctl_fromcur;
		if (std::is_same<T, CYUYVColorOrder>::value)
		{
			ctl_fromprev = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, 1, 2);
			ctl_fromcur = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1);
		}
		else
		{
			ctl_fromprev = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 3, 0);
			ctl_fromcur = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1);
		}

		for (; pp < p + cbWidth; pp += 4)
		{
			__m128i residual;
			if (std::is_same<T, CYUYVColorOrder>::value)
				residual = _mm_cvtsi32_si128(y[0] | (u[0] << 8) | (y[1] << 16) | (v[0] << 24));
			else
				residual = _mm_cvtsi32_si128(u[0] | (y[0] << 8) | (v[0] << 16) | (y[1] << 24));
			__m128i top = _mm_cvtsi32_si128(*(const uint32_t *)(pp - scbStride));

			__m128i left = _mm_shuffle_epi8(prev, ctl_fromprev);
			__m128i topleft = _mm_or_si128(
				_mm_shuffle_epi8(topprev, ctl_fromprev),
				_mm_shuffle_epi8(top, ctl_fromcur));

			__m128i top_minus_topleft = _mm_sub_epi8(top, topleft);
			__m128i grad;
			__m128i pred;
			__m128i value;

			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_or_si128(left, _mm_shuffle_epi8(value, ctl_fromcur));
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			*(uint32_t *)pp = _mm_cvtsi128_si32(value);

			prev = value;
			topprev = top;

			y += 2;
			u += 1;
			v += 1;
		}
	}

	if (NTSTORE && Pred == PLANAR_GRADIENT)
		_aligned_free(linebuf);
}

template<int F, class T, PREDICTION_TYPE Pred>
static inline void tuned_ConvertULY2ToPackedYUV422_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pYBegin, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	if (Pred != CYLINDRICAL_WRONG_MEDIAN && IS_ALIGNED(pDstBegin, 16) && IS_MULTIPLE(cbWidth, 4 * 16) && IS_MULTIPLE(scbStride, 16))
		tuned_ConvertULY2ToPackedYUV422_RestoreImpl<F, T, Pred, true>(pDstBegin, pDstEnd,pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
	else
		tuned_ConvertULY2ToPackedYUV422_RestoreImpl<F, T, Pred, false>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertPackedUVToPlanar_PredictAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	uint8_t uprev = 0x80;
	uint8_t vprev = 0x80;

	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

		__m128i uprevv = _mm_slli_si128(_mm_cvtsi32_si128(uprev), 15);
		__m128i vprevv = _mm_slli_si128(_mm_cvtsi32_si128(vprev), 15);

		for (; pp <= p + cbWidth - 32; pp += 32)
		{
			auto planar = tuned_ConvertPackedUVToPlanarElement<F, __m128i>(pp);
			_mm_storeu_si128((__m128i*)u, tuned_PredictLeftAndCount8Element<F, true, 2, 0>(uprevv, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i*)v, tuned_PredictLeftAndCount8Element<F, true, 2, 1>(vprevv, planar.v, pVCountTable));

			uprevv = planar.u;
			vprevv = planar.v;

			u += 16;
			v += 16;
		}

		uprev = _mm_cvtsi128_si32(_mm_srli_si128(uprevv, 15));
		vprev = _mm_cvtsi128_si32(_mm_srli_si128(vprevv, 15));

		for (; pp < p + cbWidth; pp += 2, ++u, ++v)
		{
			auto uu = pp[0];
			*u = uu - uprev;
			++pUCountTable[0][*u];
			uprev = uu;
			auto vv = pp[1];
			*v = vv - vprev;
			++pVCountTable[0][*v];
			vprev = vv;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i uprevv = _mm_setzero_si128();
		__m128i vprevv = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 32; pp += 32)
		{
			auto planar = tuned_ConvertPackedUVToPlanarElement<F, __m128i>(pp, scbStride);
			_mm_storeu_si128((__m128i*)u, tuned_PredictLeftAndCount8Element<F, true, 2, 0>(uprevv, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i*)v, tuned_PredictLeftAndCount8Element<F, true, 2, 1>(vprevv, planar.v, pVCountTable));

			uprevv = planar.u;
			vprevv = planar.v;

			u += 16;
			v += 16;
		}

		uprev = _mm_cvtsi128_si32(_mm_srli_si128(uprevv, 15));
		vprev = _mm_cvtsi128_si32(_mm_srli_si128(vprevv, 15));

		for (; pp < p + cbWidth; pp += 2, ++u, ++v)
		{
			auto uu = pp[0] - (pp - scbStride)[0];
			*u = uu - uprev;
			++pUCountTable[0][*u];
			uprev = uu;
			auto vv = pp[1] - (pp - scbStride)[1];
			*v = vv - vprev;
			++pVCountTable[0][*v];
			vprev = vv;
		}
	}

	uprev = 0;
	vprev = 0;

	uint8_t utopprev = 0;
	uint8_t vtopprev = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i uprevv = _mm_slli_si128(_mm_cvtsi32_si128(uprev), 15);
		__m128i vprevv = _mm_slli_si128(_mm_cvtsi32_si128(vprev), 15);
		__m128i utopprevv = _mm_slli_si128(_mm_cvtsi32_si128(utopprev), 15);
		__m128i vtopprevv = _mm_slli_si128(_mm_cvtsi32_si128(vtopprev), 15);

		for (; pp <= p + cbWidth - 32; pp += 32)
		{
			auto planar = tuned_ConvertPackedUVToPlanarElement<F, __m128i>(pp);
			auto top = tuned_ConvertPackedUVToPlanarElement<F, __m128i>(pp - scbStride);
			_mm_storeu_si128((__m128i*)u, tuned_PredictWrongMedianAndCount8Element<F, true, 2, 0>(utopprevv, top.u, uprevv, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i*)v, tuned_PredictWrongMedianAndCount8Element<F, true, 2, 1>(vtopprevv, top.v, vprevv, planar.v, pVCountTable));

			uprevv = planar.u;
			vprevv = planar.v;
			utopprevv = top.u;
			vtopprevv = top.v;

			u += 16;
			v += 16;
		}

		uprev = _mm_cvtsi128_si32(_mm_srli_si128(uprevv, 15));
		vprev = _mm_cvtsi128_si32(_mm_srli_si128(vprevv, 15));
		utopprev = _mm_cvtsi128_si32(_mm_srli_si128(utopprevv, 15));
		vtopprev = _mm_cvtsi128_si32(_mm_srli_si128(vtopprevv, 15));

		for (; pp < p + cbWidth; pp += 2, ++u, ++v)
		{
			auto uu = pp[0];
			auto utop = (pp - scbStride)[0];
			*u = uu - median<uint8_t>(uprev, utop, uprev + utop - utopprev);
			++pUCountTable[0][*u];
			uprev = uu;
			utopprev = utop;
			auto vv = pp[1];
			auto vtop = (pp - scbStride)[1];
			*v = vv - median<uint8_t>(vprev, vtop, vprev + vtop - vtopprev);
			++pVCountTable[0][*v];
			vprev = vv;
			vtopprev = vtop;
		}
	}
}

template<int F>
void tuned_ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	tuned_ConvertPackedUVToPlanar_PredictAndCount<F, CYLINDRICAL_LEFT>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

template<int F>
void tuned_ConvertPackedUVToPlanar_PredictPlanarGradientAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	tuned_ConvertPackedUVToPlanar_PredictAndCount<F, PLANAR_GRADIENT>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

template<int F>
void tuned_ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	tuned_ConvertPackedUVToPlanar_PredictAndCount<F, CYLINDRICAL_WRONG_MEDIAN>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedUVToPlanar_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedUVToPlanar_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void tuned_ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
#endif

//

template<int F, PREDICTION_TYPE Pred, bool NTSTORE>
static inline void tuned_ConvertPlanarToPackedUV_RestoreImpl(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t uprev = 0x80;
	uint8_t vprev = 0x80;

	auto u = pUBegin;
	auto v = pVBegin;

	uint8_t* linebuf = NULL;
	if (NTSTORE && Pred == PLANAR_GRADIENT)
		linebuf = (uint8_t*)_aligned_malloc(cbWidth, 16);

	__m128i uprevv = _mm_set1_epi8(uprev);
	__m128i vprevv = _mm_set1_epi8(vprev);

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		if (!NTSTORE)
		{
			uprevv = _mm_set1_epi8(uprev);
			vprevv = _mm_set1_epi8(vprev);
		}

		auto lb = linebuf;
		for (; pp <= p + cbWidth - 32; pp += 32, lb += 32)
		{
			__m128i uu = _mm_loadu_si128((const __m128i*)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprevv, uu);
			__m128i vv = _mm_loadu_si128((const __m128i*)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprevv, vv);
			if (!NTSTORE || Pred != PLANAR_GRADIENT)
				tuned_ConvertPlanarToPackedUVElement<F, __m128i, NTSTORE>(pp, uvalue.v0, vvalue.v0);
			else
				tuned_ConvertPlanarToPackedUVElement<F, __m128i, NTSTORE>(pp, lb, uvalue.v0, vvalue.v0);

			uprevv = uvalue.v1;
			vprevv = vvalue.v1;

			u += 16;
			v += 16;
		}

		if (!NTSTORE)
		{
			uprev = _mm_cvtsi128_si32(uprevv);
			vprev = _mm_cvtsi128_si32(vprevv);

			for (; pp < p + cbWidth; pp += 2)
			{
				pp[0] = uprev += *u;
				pp[1] = vprev += *v;

				++u;
				++v;
			}
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i uprevv = _mm_set1_epi8(0);
		__m128i vprevv = _mm_set1_epi8(0);

		auto lb = linebuf;
		for (; pp <= p + cbWidth - 32; pp += 32, lb += 32)
		{
			__m128i uu = _mm_loadu_si128((const __m128i*)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprevv, uu);
			__m128i vv = _mm_loadu_si128((const __m128i*)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprevv, vv);
			if (!NTSTORE)
				tuned_ConvertPlanarToPackedUVElement<F, __m128i>(pp, uvalue.v0, vvalue.v0, scbStride);
			else
				tuned_ConvertPlanarToPackedUVElement<F, __m128i>(pp, uvalue.v0, vvalue.v0, lb);

			uprevv = uvalue.v1;
			vprevv = vvalue.v1;

			u += 16;
			v += 16;
		}

		if (!NTSTORE)
		{
			uprev = _mm_cvtsi128_si32(uprevv);
			vprev = _mm_cvtsi128_si32(vprevv);

			for (; pp < p + cbWidth; pp += 2)
			{
				pp[0] = (uprev += *u) + (pp - scbStride)[0];
				pp[1] = (vprev += *v) + (pp - scbStride)[1];

				++u;
				++v;
			}
		}
	}

	uprev = 0;
	vprev = 0;

	uint8_t utopprev = 0;
	uint8_t vtopprev = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i left;
		__m128i prev;
		__m128i topprev;

		left = _mm_cvtsi32_si128((vprev << 8) | uprev);
		prev = _mm_slli_si128(left, 14);
		topprev = _mm_slli_si128(_mm_cvtsi32_si128((vtopprev << 8) | utopprev), 14);

		for (; pp <= p + cbWidth - 16; pp += 16)
		{
			__m128i uu = _mm_loadu_si64(u);
			__m128i vv = _mm_loadu_si64(v);

			__m128i residual;
			__m128i top;
			residual = _mm_unpacklo_epi8(uu, vv);
			top = _mm_loadu_si128((const __m128i*)(pp - scbStride));

			__m128i value;
			__m128i grad;
			__m128i pred;
			__m128i topleft;

			topleft = _mm_alignr_epi8(top, topprev, 14);
			__m128i top_minus_topleft = _mm_sub_epi8(top, topleft);

			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 14);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 14);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 14);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 14);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 14);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 14);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 14);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			_mm_storeu_si128((__m128i*)pp, value);

			prev = value;
			left = _mm_alignr_epi8(value, value, 14);
			topprev = top;

			u += 8;
			v += 8;
		}

		uint32_t tmp = _mm_cvtsi128_si32(left);
		uprev = tmp;
		vprev = tmp >> 8;

		uint32_t toptmp = _mm_cvtsi128_si32(_mm_srli_si128(topprev, 14));
		utopprev = toptmp;
		vtopprev = toptmp >> 8;

		for (; pp < p + cbWidth; pp += 2)
		{
			uint8_t utop = (pp - scbStride)[0];
			uint8_t uu = *u + median<uint8_t>(uprev, utop, uprev + utop - utopprev);
			pp[0] = uu;
			uint8_t vtop = (pp - scbStride)[1];
			uint8_t vv = *v + median<uint8_t>(vprev, vtop, vprev + vtop - vtopprev);
			pp[1] = vv;

			uprev = uu;
			utopprev = utop;
			vprev = vv;
			vtopprev = vtop;

			++u;
			++v;
		}
	}

	if (NTSTORE && Pred == PLANAR_GRADIENT)
		_aligned_free(linebuf);
}

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertPlanarToPackedUV_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	if (Pred != CYLINDRICAL_WRONG_MEDIAN && IS_ALIGNED(pDstBegin, 16) && IS_MULTIPLE(cbWidth, 2 * 16) && IS_MULTIPLE(scbStride, 16))
		tuned_ConvertPlanarToPackedUV_RestoreImpl<F, Pred, true>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
	else
		tuned_ConvertPlanarToPackedUV_RestoreImpl<F, Pred, false>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertPlanarToPackedUV_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertPlanarToPackedUV_Restore<F, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertPlanarToPackedUV_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertPlanarToPackedUV_Restore<F, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertPlanarToPackedUV_Restore<F, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPlanarToPackedUV_RestoreCylindricalLeft<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPlanarToPackedUV_RestorePlanarGradient<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPlanarToPackedUV_RestoreCylindricalLeft<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPlanarToPackedUV_RestorePlanarGradient<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, bool A, PREDICTION_TYPE Pred>
static inline void tuned_ConvertB64aToUQRX_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024])
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;
	uint16_t aprevb = 0x200;

	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *a = (uint16_t *)pABegin;

	uint16_t* topbuf = NULL;
	if (Pred != CYLINDRICAL_LEFT)
		topbuf = (uint16_t*)_aligned_malloc(A ? cbWidth : cbWidth / 4 * 3, 16);

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;
		auto tt = topbuf;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 14);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 14);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 14);
		__m128i aprev;
		if (A)
			aprev = _mm_slli_si128(_mm_cvtsi32_si128(aprevb), 14);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertB64aToPlanarElement10<F, __m128i, false>(pp);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount10Element<F, true, A ? 4 : 2, 0>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount10Element<F, true, 4, 2>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount10Element<F, true, 4, 3>(rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictLeftAndCount10Element<F, true, 4, 1>(aprev, planar.a, pACountTable));
			if (Pred != CYLINDRICAL_LEFT)
			{
				_mm_store_si128((__m128i*)tt, planar.g);
				_mm_store_si128((__m128i*)(tt + 8), planar.b);
				_mm_store_si128((__m128i*)(tt + 16), planar.r);
				if (A)
					_mm_store_si128((__m128i*)(tt + 24), planar.a);
				tt += A ? 32 : 24;
			}

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;

			b += 8;
			g += 8;
			r += 8;
			if (A)
				a += 8;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 14));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 14));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 14));
		if (A)
			aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 14));

		for (; pp < p + cbWidth; pp += 8)
		{
			uint16_t gg = Convert16To10Fullrange(btoh16(((uint16_t *)pp)[2]));
			uint16_t bb = Convert16To10Fullrange(btoh16(((uint16_t*)pp)[3])) - gg;
			uint16_t rr = Convert16To10Fullrange(btoh16(((uint16_t*)pp)[1])) - gg;
			if (Pred != CYLINDRICAL_LEFT)
			{
				tt[0] = gg;
				tt[1] = bb;
				tt[2] = rr;
			}

			*g = (gg - gprevb) & 0x3ff;
			++pGCountTable[0][*g];
			*b = (bb - bprevb) & 0x3ff;
			++pBCountTable[0][*b];
			*r = (rr - rprevb) & 0x3ff;
			++pRCountTable[0][*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A) {
				uint16_t aa = Convert16To10Fullrange(btoh16(((uint16_t *)pp)[0]));
				if (Pred != CYLINDRICAL_LEFT)
					tt[3] = aa;
				*a = (aa - aprevb) & 0x3ff;
				++pACountTable[0][*a];
				aprevb = aa;
			}
			if (Pred != CYLINDRICAL_LEFT)
				tt += A ? 4 : 3;

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;
		auto tt = topbuf;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();
		__m128i aprev;
		if (A)
			aprev = _mm_slli_si128(_mm_cvtsi32_si128(aprevb), 14);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertB64aToPlanarElement10<F, __m128i, false>(pp);
			auto memo = planar;
			planar.g = _mm_sub_epi16(planar.g, _mm_load_si128((const __m128i*)tt));
			planar.b = _mm_sub_epi16(planar.b, _mm_load_si128((const __m128i*)(tt + 8)));
			planar.r = _mm_sub_epi16(planar.r, _mm_load_si128((const __m128i*)(tt + 16)));
			_mm_storeu_si128((__m128i*)g, tuned_PredictLeftAndCount10Element<F, true, A ? 4 : 2, 0>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i*)b, tuned_PredictLeftAndCount10Element<F, true, 4, 2>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i*)r, tuned_PredictLeftAndCount10Element<F, true, 4, 3>(rprev, planar.r, pRCountTable));
			if (A)
			{
				planar.a = _mm_sub_epi16(planar.a, _mm_load_si128((const __m128i*)(tt + 24)));
				_mm_storeu_si128((__m128i*)a, tuned_PredictLeftAndCount10Element<F, true, 4, 1>(aprev, planar.a, pACountTable));
			}
			if (Pred != CYLINDRICAL_LEFT)
			{
				_mm_store_si128((__m128i*)tt, memo.g);
				_mm_store_si128((__m128i*)(tt + 8), memo.b);
				_mm_store_si128((__m128i*)(tt + 16), memo.r);
				if (A)
					_mm_store_si128((__m128i*)(tt + 24), memo.a);
				tt += A ? 32 : 24;
			}

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;

			b += 8;
			g += 8;
			r += 8;
			if (A)
				a += 8;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 14));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 14));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 14));
		if (A)
			aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 14));

		for (; pp < p + cbWidth; pp += 8)
		{
			uint16_t gg = Convert16To10Fullrange(btoh16(((uint16_t*)pp)[2]));
			uint16_t bb = Convert16To10Fullrange(btoh16(((uint16_t*)pp)[3])) - gg;
			uint16_t rr = Convert16To10Fullrange(btoh16(((uint16_t*)pp)[1])) - gg;
			if (Pred != CYLINDRICAL_LEFT)
			{
				auto gmemo = gg;
				gg -= tt[0];
				tt[0] = gmemo;
				auto bmemo = bb;
				bb -= tt[1];
				tt[1] = bmemo;
				auto rmemo = rr;
				rr -= tt[2];
				tt[2] = rmemo;
			}

			*g = (gg - gprevb) & 0x3ff;
			++pGCountTable[0][*g];
			*b = (bb - bprevb) & 0x3ff;
			++pBCountTable[0][*b];
			*r = (rr - rprevb) & 0x3ff;
			++pRCountTable[0][*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A) {
				uint16_t aa = Convert16To10Fullrange(btoh16(((uint16_t*)pp)[0]));
				if (Pred != CYLINDRICAL_LEFT)
				{
					auto amemo = aa;
					aa -= tt[3];
					tt[3] = amemo;
				}
				*a = (aa - aprevb) & 0x3ff;
				++pACountTable[0][*a];
				aprevb = aa;
			}
			if (Pred != CYLINDRICAL_LEFT)
				tt += A ? 4 : 3;

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (Pred != CYLINDRICAL_LEFT)
		_aligned_free(topbuf);
}

template<int F>
void tuned_ConvertB64aToUQRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024])
{
	tuned_ConvertB64aToUQRX_PredictAndCount<F, false, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F>
void tuned_ConvertB64aToUQRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024])
{
	tuned_ConvertB64aToUQRX_PredictAndCount<F, true, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<int F>
void tuned_ConvertB64aToUQRG_PredictPlanarGradientAndCount(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024])
{
	tuned_ConvertB64aToUQRX_PredictAndCount<F, false, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F>
void tuned_ConvertB64aToUQRA_PredictPlanarGradientAndCount(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, uint8_t* pABegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024])
{
	tuned_ConvertB64aToUQRX_PredictAndCount<F, true, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertB64aToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template void tuned_ConvertB64aToUQRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);
template void tuned_ConvertB64aToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template void tuned_ConvertB64aToUQRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, uint8_t* pABegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertB64aToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template void tuned_ConvertB64aToUQRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);
template void tuned_ConvertB64aToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template void tuned_ConvertB64aToUQRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, uint8_t* pABegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);
#endif

//

template<int F, bool A, PREDICTION_TYPE Pred, bool NTSTORE>
static inline void tuned_ConvertUQRXToB64a_RestoreImpl(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;
	uint16_t aprevb = 0x200;

	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *a = (uint16_t *)pABegin;

	uint16_t* linebuf = NULL;
	if (Pred != CYLINDRICAL_LEFT)
		linebuf = (uint16_t*)_aligned_malloc(A ? cbWidth : cbWidth / 4 * 3, 16);

	__m128i gprev = _mm_set1_epi16(gprevb);
	__m128i bprev = _mm_set1_epi16(bprevb);
	__m128i rprev = _mm_set1_epi16(rprevb);
	__m128i aprev;
	if (A)
		aprev = _mm_set1_epi16(aprevb);

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		if (!NTSTORE)
		{
			gprev = _mm_set1_epi16(gprevb);
			bprev = _mm_set1_epi16(bprevb);
			rprev = _mm_set1_epi16(rprevb);
			aprev;
			if (A)
				aprev = _mm_set1_epi16(aprevb);
		}

		auto lb = linebuf;
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft10Element<F, false>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft10Element<F, false>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft10Element<F, false>(rprev, rr);
			if (Pred != CYLINDRICAL_LEFT)
			{
				_mm_store_si128((__m128i*)lb, gvalue.v0);
				_mm_store_si128((__m128i*)(lb + 8), bvalue.v0);
				_mm_store_si128((__m128i*)(lb + 16), rvalue.v0);
			}
			__m128i avalue_v0;
			if (A)
			{
				__m128i aa = _mm_loadu_si128((const __m128i *)a);
				auto avalue = tuned_RestoreLeft10Element<F, false>(aprev, aa);
				aprev = avalue.v1;
				avalue_v0 = avalue.v0;
				if (Pred != CYLINDRICAL_LEFT)
					_mm_store_si128((__m128i*)(lb + 24), avalue.v0);
			}
			else
			{
				avalue_v0 = _mm_set1_epi16((short)0xffff);
			}
			tuned_ConvertPlanarRGBXToB64aElement10<F, __m128i, false, NTSTORE>(pp, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);
			if (Pred != CYLINDRICAL_LEFT)
				lb += A ? 32 : 24;

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 8;
			b += 8;
			r += 8;
			if (A)
				a += 8;
		}

		if (!NTSTORE)
		{
			gprevb = _mm_cvtsi128_si32(gprev);
			bprevb = _mm_cvtsi128_si32(bprev);
			rprevb = _mm_cvtsi128_si32(rprev);
			if (A)
				aprevb = _mm_cvtsi128_si32(aprev);

			for (; pp < p + cbWidth; pp += 8)
			{

				auto gg = gprevb += g[0];
				auto bb = (bprevb += b[0]) + gprevb;
				auto rr = (rprevb += r[0]) + gprevb;
				if (Pred != CYLINDRICAL_LEFT)
				{
					lb[0] = gg;
					lb[1] = bb;
					lb[2] = rr;
				}
				((uint16_t*)pp)[2] = htob16(Convert10To16Fullrange(gg));
				((uint16_t*)pp)[3] = htob16(Convert10To16Fullrange(bb));
				((uint16_t*)pp)[1] = htob16(Convert10To16Fullrange(rr));
				if (A)
				{
					auto aa = aprevb += a[0];
					if (Pred != CYLINDRICAL_LEFT)
						lb[3] = aa;
					((uint16_t*)pp)[0] = htob16(Convert10To16Fullrange(aa));
				}
				else
					((uint16_t*)pp)[0] = 0xffff;
				if (Pred != CYLINDRICAL_LEFT)
					lb += A ? 4 : 3;

				g += 1;
				b += 1;
				r += 1;
				if (A)
					a += 1;
			}
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();
		__m128i aprev;
		if (A)
			aprev = _mm_setzero_si128();

		auto lb = linebuf;
		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i gg = _mm_loadu_si128((const __m128i*)g);
			auto gvalue = tuned_RestoreLeft10Element<F, false>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i*)b);
			auto bvalue = tuned_RestoreLeft10Element<F, false>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i*)r);
			auto rvalue = tuned_RestoreLeft10Element<F, false>(rprev, rr);
			if (Pred != CYLINDRICAL_LEFT)
			{
				gvalue.v0 = _mm_add_epi16(gvalue.v0, _mm_load_si128((const __m128i*)lb));
				bvalue.v0 = _mm_add_epi16(bvalue.v0, _mm_load_si128((const __m128i*)(lb + 8)));
				rvalue.v0 = _mm_add_epi16(rvalue.v0, _mm_load_si128((const __m128i*)(lb + 16)));
				_mm_store_si128((__m128i*)lb, gvalue.v0);
				_mm_store_si128((__m128i*)(lb + 8), bvalue.v0);
				_mm_store_si128((__m128i*)(lb + 16), rvalue.v0);
			}
			__m128i avalue_v0;
			if (A)
			{
				__m128i aa = _mm_loadu_si128((const __m128i*)a);
				auto avalue = tuned_RestoreLeft10Element<F, false>(aprev, aa);
				aprev = avalue.v1;
				avalue_v0 = avalue.v0;
				if (Pred != CYLINDRICAL_LEFT)
				{
					avalue.v0 = _mm_add_epi16(avalue.v0, _mm_load_si128((const __m128i*)(lb + 16)));
					_mm_store_si128((__m128i*)(lb + 24), avalue.v0);
				}
			}
			else
			{
				avalue_v0 = _mm_set1_epi16((short)0xffff);
			}
			tuned_ConvertPlanarRGBXToB64aElement10<F, __m128i, false, NTSTORE>(pp, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);
			if (Pred != CYLINDRICAL_LEFT)
				lb += A ? 32 : 24;

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 8;
			b += 8;
			r += 8;
			if (A)
				a += 8;
		}

		if (!NTSTORE)
		{
			gprevb = _mm_cvtsi128_si32(gprev);
			bprevb = _mm_cvtsi128_si32(bprev);
			rprevb = _mm_cvtsi128_si32(rprev);
			if (A)
				aprevb = _mm_cvtsi128_si32(aprev);

			for (; pp < p + cbWidth; pp += 8)
			{

				auto gg = gprevb += g[0];
				auto bb = (bprevb += b[0]) + gprevb;
				auto rr = (rprevb += r[0]) + gprevb;
				if (Pred != CYLINDRICAL_LEFT)
				{
					gg += lb[0];
					bb += lb[1];
					rr += lb[2];
					lb[0] = gg;
					lb[1] = bb;
					lb[2] = rr;
				}
				((uint16_t*)pp)[2] = htob16(Convert10To16Fullrange(gg));
				((uint16_t*)pp)[3] = htob16(Convert10To16Fullrange(bb));
				((uint16_t*)pp)[1] = htob16(Convert10To16Fullrange(rr));
				if (A)
				{
					auto aa = aprevb += a[0];
					if (Pred != CYLINDRICAL_LEFT)
					{
						aa += lb[3];
						lb[3] = aa;
					}
					((uint16_t*)pp)[0] = htob16(Convert10To16Fullrange(aa));
				}
				else
					((uint16_t*)pp)[0] = 0xffff;
				if (Pred != CYLINDRICAL_LEFT)
					lb += A ? 4 : 3;

				g += 1;
				b += 1;
				r += 1;
				if (A)
					a += 1;
			}
		}
	}

	if (Pred != CYLINDRICAL_LEFT)
		_aligned_free(linebuf);
}

template<int F, bool A, PREDICTION_TYPE Pred>
static inline void tuned_ConvertUQRXToB64a_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, const uint8_t* pABegin, size_t cbWidth, ssize_t scbStride)
{
	if (IS_ALIGNED(pDstBegin, 16) && IS_MULTIPLE(cbWidth, 16 * 4) && IS_MULTIPLE(scbStride, 16))
		tuned_ConvertUQRXToB64a_RestoreImpl<F, A, Pred, true>(pDstBegin, pDstEnd, pGBegin, pBBegin,pRBegin, pABegin, cbWidth, scbStride);
	else
		tuned_ConvertUQRXToB64a_RestoreImpl<F, A, Pred, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertUQRGToB64a_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertUQRXToB64a_Restore<F, false, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertUQRAToB64a_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertUQRXToB64a_Restore<F, true, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertUQRGToB64a_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertUQRXToB64a_Restore<F, false, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertUQRAToB64a_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, const uint8_t* pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertUQRXToB64a_Restore<F, true, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertUQRGToB64a_RestoreCylindricalLeft<CODEFEATURE_SSE41>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRAToB64a_RestoreCylindricalLeft<CODEFEATURE_SSE41>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRGToB64a_RestorePlanarGradient<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRAToB64a_RestorePlanarGradient<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, const uint8_t* pABegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertUQRGToB64a_RestoreCylindricalLeft<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRAToB64a_RestoreCylindricalLeft<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRGToB64a_RestorePlanarGradient<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertUQRAToB64a_RestorePlanarGradient<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, const uint8_t* pABegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertR210ToUQRG_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024])
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;

	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;

	uint16_t* topbuf = NULL;
	if (Pred != CYLINDRICAL_LEFT)
		topbuf = (uint16_t*)_aligned_malloc(nWidth * 6, 16);

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		const uint8_t *pStrideEnd = p + nWidth * 4;
		auto pp = p;
		auto tt = topbuf;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 14);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 14);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 14);

		for (; pp <= pStrideEnd - 32; pp += 32)
		{
			auto planar = tuned_ConvertR210ToPlanarElement10<F, __m128i, false>(pp);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount10Element<F, true, 2, 0>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount10Element<F, true, 4, 2>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount10Element<F, true, 4, 3>(rprev, planar.r, pRCountTable));
			if (Pred != CYLINDRICAL_LEFT)
			{
				_mm_store_si128((__m128i*)tt, planar.g);
				_mm_store_si128((__m128i*)(tt + 8), planar.b);
				_mm_store_si128((__m128i*)(tt + 16), planar.r);
				tt += 24;
			}

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;

			b += 8;
			g += 8;
			r += 8;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 14));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 14));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 14));

		for (; pp < pStrideEnd; pp += 4)
		{
			uint32_t val = btoh32(*(const uint32_t *)pp);

			uint16_t gg = val >> 10;
			uint16_t bb = val - gg;
			uint16_t rr = (val >> 20) - gg;
			if (Pred != CYLINDRICAL_LEFT)
			{
				tt[0] = gg;
				tt[1] = bb;
				tt[2] = rr;
				tt += 3;
			}

			*g = (gg - gprevb) & 0x3ff;
			*b = (bb - bprevb) & 0x3ff;
			*r = (rr - rprevb) & 0x3ff;
			++pGCountTable[0][*g];
			++pBCountTable[0][*b];
			++pRCountTable[0][*r];
			gprevb = gg;
			bprevb = bb;
			rprevb = rr;

			b += 1;
			g += 1;
			r += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		const uint8_t* pStrideEnd = p + nWidth * 4;
		auto pp = p;
		auto tt = topbuf;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();

		for (; pp <= pStrideEnd - 32; pp += 32)
		{
			auto planar = tuned_ConvertR210ToPlanarElement10<F, __m128i, false>(pp);
			auto memo = planar;
			planar.g = _mm_sub_epi16(planar.g, _mm_load_si128((const __m128i*)tt));
			planar.b = _mm_sub_epi16(planar.b, _mm_load_si128((const __m128i*)(tt + 8)));
			planar.r = _mm_sub_epi16(planar.r, _mm_load_si128((const __m128i*)(tt + 16)));
			_mm_storeu_si128((__m128i*)g, tuned_PredictLeftAndCount10Element<F, true, 2, 0>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i*)b, tuned_PredictLeftAndCount10Element<F, true, 4, 2>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i*)r, tuned_PredictLeftAndCount10Element<F, true, 4, 3>(rprev, planar.r, pRCountTable));

			_mm_store_si128((__m128i*)tt, memo.g);
			_mm_store_si128((__m128i*)(tt + 8), memo.b);
			_mm_store_si128((__m128i*)(tt + 16), memo.r);
			tt += 24;

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;

			b += 8;
			g += 8;
			r += 8;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 14));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 14));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 14));

		for (; pp < pStrideEnd; pp += 4)
		{
			uint32_t val = btoh32(*(const uint32_t*)pp);

			uint16_t gg = val >> 10;
			uint16_t bb = val - gg;
			uint16_t rr = (val >> 20) - gg;
			auto gmemo = gg;
			gg -= tt[0];
			tt[0] = gmemo;
			auto bmemo = bb;
			bb -= tt[1];
			tt[1] = bmemo;
			auto rmemo = rr;
			rr -= tt[2];
			tt[2] = rmemo;
			tt += 3;

			*g = (gg - gprevb) & 0x3ff;
			++pGCountTable[0][*g];
			*b = (bb - bprevb) & 0x3ff;
			++pBCountTable[0][*b];
			*r = (rr - rprevb) & 0x3ff;
			++pRCountTable[0][*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;

			b += 1;
			g += 1;
			r += 1;
		}
	}

	if (Pred != CYLINDRICAL_LEFT)
		_aligned_free(topbuf);
}

template<int F>
void tuned_ConvertR210ToUQRG_PredictCylindricalLeftAndCount(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024])
{
	tuned_ConvertR210ToUQRG_PredictAndCount<F, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin,  pSrcBegin, pSrcEnd, nWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
}

template<int F>
void tuned_ConvertR210ToUQRG_PredictPlanarGradientAndCount(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024])
{
	tuned_ConvertR210ToUQRG_PredictAndCount<F, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, nWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertR210ToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template void tuned_ConvertR210ToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertR210ToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template void tuned_ConvertR210ToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
#endif

//

template<int F, PREDICTION_TYPE Pred, bool NTSTORE>
static inline void tuned_ConvertUQRGToR210_RestoreImpl(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride)
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;

	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;

	uint16_t* linebuf = NULL;
	if (Pred != CYLINDRICAL_LEFT)
		linebuf = (uint16_t*)_aligned_malloc(nWidth * 6, 16);

	__m128i gprev = _mm_set1_epi16(gprevb);
	__m128i bprev = _mm_set1_epi16(bprevb);
	__m128i rprev = _mm_set1_epi16(rprevb);

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		uint8_t *pStrideEnd = p + nWidth * 4;
		auto pp = p;

		if (!NTSTORE)
		{
			gprev = _mm_set1_epi16(gprevb);
			bprev = _mm_set1_epi16(bprevb);
			rprev = _mm_set1_epi16(rprevb);
		}

		auto lb = linebuf;
		for (; pp <= pStrideEnd - 32; pp += 32)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft10Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft10Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft10Element<F>(rprev, rr);
			tuned_ConvertPlanarRGBXToR210Element10<F, __m128i, false, NTSTORE>(pp, gvalue.v0, bvalue.v0, rvalue.v0);
			if (Pred != CYLINDRICAL_LEFT)
			{
				_mm_store_si128((__m128i*)lb, gvalue.v0);
				_mm_store_si128((__m128i*)(lb + 8), bvalue.v0);
				_mm_store_si128((__m128i*)(lb + 16), rvalue.v0);
				lb += 24;
			}

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 8;
			b += 8;
			r += 8;
		}

		if (!NTSTORE)
		{
			gprevb = _mm_cvtsi128_si32(gprev);
			bprevb = _mm_cvtsi128_si32(bprev);
			rprevb = _mm_cvtsi128_si32(rprev);

			for (; pp < pStrideEnd; pp += 4)
			{
				uint32_t gg = (gprevb += g[0]) & 0x3ff;
				uint32_t bb = ((bprevb += b[0]) + gprevb) & 0x3ff;
				uint32_t rr = ((rprevb += r[0]) + gprevb) & 0x3ff;
				*(uint32_t*)pp = htob32((rr << 20) | (gg << 10) | bb);
				if (Pred != CYLINDRICAL_LEFT)
				{
					lb[0] = gg;
					lb[1] = bb;
					lb[2] = rr;
					lb += 3;
				}

				g += 1;
				b += 1;
				r += 1;
			}
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		uint8_t* pStrideEnd = p + nWidth * 4;
		auto pp = p;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();

		auto lb = linebuf;
		for (; pp <= pStrideEnd - 32; pp += 32)
		{
			__m128i gg = _mm_loadu_si128((const __m128i*)g);
			auto gvalue = tuned_RestoreLeft10Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i*)b);
			auto bvalue = tuned_RestoreLeft10Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i*)r);
			auto rvalue = tuned_RestoreLeft10Element<F>(rprev, rr);
			if (Pred != CYLINDRICAL_LEFT)
			{
				gvalue.v0 = _mm_add_epi16(gvalue.v0, _mm_load_si128((const __m128i*)lb));
				bvalue.v0 = _mm_add_epi16(bvalue.v0, _mm_load_si128((const __m128i*)(lb + 8)));
				rvalue.v0 = _mm_add_epi16(rvalue.v0, _mm_load_si128((const __m128i*)(lb + 16)));
			}
			tuned_ConvertPlanarRGBXToR210Element10<F, __m128i, false, NTSTORE>(pp, gvalue.v0, bvalue.v0, rvalue.v0);
			if (Pred != CYLINDRICAL_LEFT)
			{
				_mm_store_si128((__m128i*)lb, gvalue.v0);
				_mm_store_si128((__m128i*)(lb + 8), bvalue.v0);
				_mm_store_si128((__m128i*)(lb + 16), rvalue.v0);
				lb += 24;
			}

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 8;
			b += 8;
			r += 8;
		}

		if (!NTSTORE)
		{
			gprevb = _mm_cvtsi128_si32(gprev);
			bprevb = _mm_cvtsi128_si32(bprev);
			rprevb = _mm_cvtsi128_si32(rprev);

			for (; pp < pStrideEnd; pp += 4)
			{
				uint32_t gg = (gprevb += g[0]);
				uint32_t bb = ((bprevb += b[0]) + gprevb);
				uint32_t rr = ((rprevb += r[0]) + gprevb);
				if (Pred != CYLINDRICAL_LEFT)
				{
					gg += lb[0];
					bb += lb[1];
					rr += lb[2];
				}
				gg &= 0x3ff;
				bb &= 0x3ff;
				rr &= 0x3ff;
				*(uint32_t*)pp = htob32((rr << 20) | (gg << 10) | bb);
				if (Pred != CYLINDRICAL_LEFT)
				{
					lb[0] = gg;
					lb[1] = bb;
					lb[2] = rr;
					lb += 3;
				}

				g += 1;
				b += 1;
				r += 1;
			}
		}
	}

	if (Pred != CYLINDRICAL_LEFT)
		_aligned_free(linebuf);
}

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertUQRGToR210_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, unsigned int nWidth, ssize_t scbStride)
{
	if (IS_ALIGNED(pDstBegin, 16) && IS_MULTIPLE(nWidth, 8) && IS_MULTIPLE(scbStride, 16))
		tuned_ConvertUQRGToR210_RestoreImpl<F, Pred, true>(pDstBegin,  pDstEnd, pGBegin, pBBegin, pRBegin, nWidth, scbStride);
	else
		tuned_ConvertUQRGToR210_RestoreImpl<F, Pred, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, nWidth, scbStride);
}

template<int F>
void tuned_ConvertUQRGToR210_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, unsigned int nWidth, ssize_t scbStride)
{
	tuned_ConvertUQRGToR210_Restore<F, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, nWidth, scbStride);
}

template<int F>
void tuned_ConvertUQRGToR210_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, unsigned int nWidth, ssize_t scbStride)
{
	tuned_ConvertUQRGToR210_Restore<F, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, nWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertUQRGToR210_RestoreCylindricalLeft<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, unsigned int nWidth, ssize_t scbStride);
template void tuned_ConvertUQRGToR210_RestorePlanarGradient<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertUQRGToR210_RestoreCylindricalLeft<CODEFEATURE_AVX1>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
template void tuned_ConvertUQRGToR210_RestorePlanarGradient<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, unsigned int nWidth, ssize_t scbStride);
#endif

//

template<int F, VALUERANGE VR, PREDICTION_TYPE Pred>
static inline void tuned_ConvertLittleEndian16ToHostEndian10_PredictAndCount(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024])
{
	uint16_t prev = 0x200;

	auto q = (uint16_t*)pDstBegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pStrideEnd = (const uint16_t*)(p + cbWidth);
		auto pp = (const uint16_t*)p;

		__m128i prevv = _mm_slli_si128(_mm_cvtsi32_si128(prev), 14);
		for (; pp <= pStrideEnd - 8; pp += 8)
		{
			__m128i value = _mm_Convert16To10<VR>(_mm_loadu_si128((const __m128i*)pp));
			_mm_storeu_si128((__m128i*)q, tuned_PredictLeftAndCount10Element<F, true, 1, 0>(prevv, value, pCountTable));
			prevv = value;

			q += 8;
		}
		prev = _mm_cvtsi128_si32(_mm_srli_si128(prevv, 14));

		for (; pp < pStrideEnd; ++pp)
		{
			uint16_t cur = Convert16To10<VR>(ltoh16(*pp));
			*q = (cur - prev) & 0x3ff;
			++pCountTable[0][*q];
			prev = cur;

			++q;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		__m128i prevv = _mm_setzero_si128();

		for (; pp <= pStrideEnd - 8; pp += 8)
		{
			__m128i value = _mm_loadu_si128((const __m128i*)pp);
			if (VR == VALUERANGE::NOROUND)
				value = _mm_Convert16To10<VR>(_mm_sub_epi16(value, _mm_loadu_si128((const __m128i*)(pp - scbStride / 2))));
			else
				value = _mm_sub_epi16(_mm_Convert16To10<VR>(value), _mm_Convert16To10<VR>(_mm_loadu_si128((const __m128i*)(pp - scbStride / 2))));
			_mm_storeu_si128((__m128i*)q, tuned_PredictLeftAndCount10Element<F, true, 1, 0>(prevv, value, pCountTable));
			prevv = value;

			q += 8;
		}
		prev = _mm_cvtsi128_si32(_mm_srli_si128(prevv, 14));

		for (; pp < pStrideEnd; ++pp)
		{
			uint16_t cur = ltoh16(*pp);
			if (VR == VALUERANGE::NOROUND)
				cur = Convert16To10<VR>(cur - ltoh16(*(pp - scbStride / 2)));
			else
				cur = Convert16To10<VR>(cur) - Convert16To10<VR>(ltoh16(*(pp - scbStride / 2)));
			*q = (cur - prev) & 0x3ff;
			++pCountTable[0][*q];
			prev = cur;

			++q;
		}
	}
}

template<int F, VALUERANGE VR>
void tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024])
{
	tuned_ConvertLittleEndian16ToHostEndian10_PredictAndCount<F, VR, CYLINDRICAL_LEFT>(pDstBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pCountTable);
}

template<int F, VALUERANGE VR>
void tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024])
{
	tuned_ConvertLittleEndian16ToHostEndian10_PredictAndCount<F, VR, PLANAR_GRADIENT>(pDstBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pCountTable); 
}

#ifdef GENERATE_SSE41
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template void tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
#endif

//

template<int F, PREDICTION_TYPE Pred, bool NTSTORE>
static inline void tuned_ConvertHostEndian16ToLittleEndian16_RestoreImpl(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride)
{
	uint16_t prev = 0x8000;

	auto q = (const uint16_t*)pSrcBegin;

	uint16_t* linebuf = NULL;
	if (NTSTORE && Pred != CYLINDRICAL_LEFT)
		linebuf = (uint16_t*)_aligned_malloc(cbWidth, 16);

	__m128i prevv = _mm_set1_epi16(prev);

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		if (!NTSTORE)
			prevv = _mm_set1_epi16(prev);

		auto lb = linebuf;
		for (; pp <= pStrideEnd - 8; pp += 8, lb += 8)
		{
			__m128i s0 = _mm_loadu_si128((const __m128i*)q);
			auto value = tuned_RestoreLeft16Element<F>(prevv, s0);
			_mmt_store<__m128i, NTSTORE>((__m128i*)pp, value.v0);
			if (NTSTORE && Pred != CYLINDRICAL_LEFT)
				_mm_storeu_si128((__m128i*)lb, value.v0);
			prevv = value.v1;

			q += 8;
		}

		if (!NTSTORE)
		{
			prev = _mm_cvtsi128_si32(prevv);

			for (; pp < pStrideEnd; ++pp)
			{
				uint16_t cur = *q;
				*pp = htol16(prev += cur);

				++q;
			}
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		__m128i prevv = _mm_setzero_si128();

		auto lb = linebuf;
		for (; pp <= pStrideEnd - 8; pp += 8, lb += 8)
		{
			__m128i s0 = _mm_loadu_si128((const __m128i*)q);
			auto value = tuned_RestoreLeft16Element<F>(prevv, s0);
			if (!NTSTORE)
				_mm_storeu_si128((__m128i*)pp, _mm_add_epi16(value.v0, _mm_loadu_si128((const __m128i*)(pp - scbStride / 2))));
			else
			{
				auto restored = _mm_add_epi16(value.v0, _mm_loadu_si128((const __m128i*)lb));
				_mm_stream_si128((__m128i*)pp, restored);
				_mm_storeu_si128((__m128i*)lb, restored);
			}
			prevv = value.v1;

			q += 8;
		}

		if (!NTSTORE)
		{
			prev = _mm_cvtsi128_si32(prevv);

			for (; pp < pStrideEnd; ++pp)
			{
				uint16_t cur = *q;
				*pp = htol16((prev += cur) + ltoh16(*(pp - scbStride / 2)));

				++q;
			}
		}
	}

	if (NTSTORE && Pred != CYLINDRICAL_LEFT)
		_aligned_free(linebuf);
}

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertHostEndian16ToLittleEndian16_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride)
{
	if (IS_ALIGNED(pDstBegin, 16) && IS_MULTIPLE(cbWidth, 16) && IS_MULTIPLE(scbStride, 16))
		tuned_ConvertHostEndian16ToLittleEndian16_RestoreImpl<F, Pred, true>(pDstBegin, pDstEnd, pSrcBegin, cbWidth, scbStride);
	else
		tuned_ConvertHostEndian16ToLittleEndian16_RestoreImpl<F, Pred, false>(pDstBegin, pDstEnd, pSrcBegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertHostEndian16ToLittleEndian16_Restore<F, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pSrcBegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertHostEndian16ToLittleEndian16_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertHostEndian16ToLittleEndian16_Restore<F, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pSrcBegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertHostEndian16ToLittleEndian16_RestorePlanarGradient<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertHostEndian16ToLittleEndian16_RestorePlanarGradient<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, VALUERANGE VR, PREDICTION_TYPE Pred>
static inline void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024])
{
	uint16_t uprev = 0x200;
	uint16_t vprev = 0x200;

	auto u = (uint16_t*)pUBegin;
	auto v = (uint16_t*)pVBegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pStrideEnd = (const uint16_t*)(p + cbWidth);
		auto pp = (const uint16_t*)p;

		__m128i uprevv = _mm_slli_si128(_mm_cvtsi32_si128(uprev), 14);
		__m128i vprevv = _mm_slli_si128(_mm_cvtsi32_si128(vprev), 14);
		for (; pp <= pStrideEnd - 16; pp += 16)
		{
			auto m0 = _mm_loadu_si128((const __m128i*)pp);
			auto m1 = _mm_loadu_si128((const __m128i*)(pp + 8));
			m0 = _mm_Convert16To10<VR>(m0);
			m1 = _mm_Convert16To10<VR>(m1);
			m0 = _mm_shuffle_epi8(m0, _mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0));
			m1 = _mm_shuffle_epi8(m1, _mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0));
			auto uu = _mm_unpacklo_epi64(m0, m1);
			auto vv = _mm_unpackhi_epi64(m0, m1);
			_mm_storeu_si128((__m128i*)u, tuned_PredictLeftAndCount10Element<F, true, 2, 0>(uprevv, uu, pUCountTable));
			_mm_storeu_si128((__m128i*)v, tuned_PredictLeftAndCount10Element<F, true, 2, 1>(vprevv, vv, pVCountTable));
			uprevv = uu;
			vprevv = vv;

			u += 8;
			v += 8;
		}
		uprev = _mm_cvtsi128_si32(_mm_srli_si128(uprevv, 14));
		vprev = _mm_cvtsi128_si32(_mm_srli_si128(vprevv, 14));

		for (; pp < pStrideEnd; pp += 2, ++u, ++v)
		{
			auto uu = Convert16To10<VR>(ltoh16(pp[0]));
			*u = (uu - uprev) & 0x3ff;
			++pUCountTable[0][*u];
			uprev = uu;
			auto vv = Convert16To10<VR>(ltoh16(pp[1]));
			*v = (vv - vprev) & 0x3ff;
			++pVCountTable[0][*v];
			vprev = vv;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		__m128i uprevv = _mm_setzero_si128();
		__m128i vprevv = _mm_setzero_si128();
		for (; pp <= pStrideEnd - 16; pp += 16)
		{
			auto m0 = _mm_loadu_si128((const __m128i*)pp);
			auto m1 = _mm_loadu_si128((const __m128i*)(pp + 8));
			if (VR == VALUERANGE::NOROUND)
			{
				m0 = _mm_Convert16To10<VR>(_mm_sub_epi16(m0, _mm_loadu_si128((const __m128i*)(pp - scbStride / 2))));
				m1 = _mm_Convert16To10<VR>(_mm_sub_epi16(m1, _mm_loadu_si128((const __m128i*)(pp + 8 - scbStride / 2))));
			}
			else
			{
				m0 = _mm_sub_epi16(_mm_Convert16To10<VR>(m0), _mm_Convert16To10<VR>(_mm_loadu_si128((const __m128i*)(pp - scbStride / 2))));
				m1 = _mm_sub_epi16(_mm_Convert16To10<VR>(m1), _mm_Convert16To10<VR>(_mm_loadu_si128((const __m128i*)(pp + 8 - scbStride / 2))));
			}
			m0 = _mm_shuffle_epi8(m0, _mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0));
			m1 = _mm_shuffle_epi8(m1, _mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0));
			auto uu = _mm_unpacklo_epi64(m0, m1);
			auto vv = _mm_unpackhi_epi64(m0, m1);
			_mm_storeu_si128((__m128i*)u, tuned_PredictLeftAndCount10Element<F, true, 2, 0>(uprevv, uu, pUCountTable));
			_mm_storeu_si128((__m128i*)v, tuned_PredictLeftAndCount10Element<F, true, 2, 1>(vprevv, vv, pVCountTable));
			uprevv = uu;
			vprevv = vv;

			u += 8;
			v += 8;
		}
		uprev = _mm_cvtsi128_si32(_mm_srli_si128(uprevv, 14));
		vprev = _mm_cvtsi128_si32(_mm_srli_si128(vprevv, 14));

		for (; pp < pStrideEnd; pp += 2, ++u, ++v)
		{
			auto uu = ltoh16(pp[0]);
			if (VR == VALUERANGE::NOROUND)
				uu = Convert16To10<VR>(uu - ltoh16(pp[-scbStride / 2]));
			else
				uu = Convert16To10<VR>(uu) - Convert16To10<VR>(ltoh16(pp[-scbStride / 2]));
			*u = (uu - uprev) & 0x3ff;
			++pUCountTable[0][*u];
			uprev = uu;
			auto vv = ltoh16(pp[1]);
			if (VR == VALUERANGE::NOROUND)
				vv = Convert16To10<VR>(vv - ltoh16(pp[1 - scbStride / 2]));
			else
				vv = Convert16To10<VR>(vv) - Convert16To10<VR>(ltoh16(pp[1 - scbStride / 2]));
			*v = (vv - vprev) & 0x3ff;
			++pVCountTable[0][*v];
			vprev = vv;
		}
	}
}

template<int F, VALUERANGE VR>
void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024])
{
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictAndCount<F, VR, CYLINDRICAL_LEFT>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

template<int F, VALUERANGE VR>
void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024])
{
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictAndCount<F, VR, PLANAR_GRADIENT>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
#endif

//

template<int F, PREDICTION_TYPE Pred, bool NTSTORE>
static inline void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreImpl(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint16_t uprev = 0x8000;
	uint16_t vprev = 0x8000;

	auto u = (const uint16_t*)pUBegin;
	auto v = (const uint16_t*)pVBegin;

	uint16_t* linebuf = NULL;
	if (NTSTORE && Pred != CYLINDRICAL_LEFT)
		linebuf = (uint16_t*)_aligned_malloc(cbWidth, 16);

	__m128i uprevv = _mm_set1_epi16(uprev);
	__m128i vprevv = _mm_set1_epi16(vprev);

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		if (!NTSTORE)
		{
			uprevv = _mm_set1_epi16(uprev);
			vprevv = _mm_set1_epi16(vprev);
		}

		auto lb = linebuf;
		for (; pp <= pStrideEnd - 16; pp += 16, lb += 16)
		{
			__m128i uu = _mm_loadu_si128((const __m128i*)u);
			auto uvalue = tuned_RestoreLeft16Element<F>(uprevv, uu);
			__m128i vv = _mm_loadu_si128((const __m128i*)v);
			auto vvalue = tuned_RestoreLeft16Element<F>(vprevv, vv);
			auto m0 = _mm_unpacklo_epi16(uvalue.v0, vvalue.v0);
			auto m1 = _mm_unpackhi_epi16(uvalue.v0, vvalue.v0);
			_mmt_store<__m128i, NTSTORE>((__m128i*)pp, m0);
			_mmt_store<__m128i, NTSTORE>((__m128i*)(pp + 8), m1);
			if (NTSTORE && Pred != CYLINDRICAL_LEFT)
			{
				_mm_storeu_si128((__m128i*)lb, m0);
				_mm_storeu_si128((__m128i*)(lb + 8), m1);
			}
			uprevv = uvalue.v1;
			vprevv = vvalue.v1;

			u += 8;
			v += 8;
		}

		if (!NTSTORE)
		{
			uprev = _mm_cvtsi128_si32(uprevv);
			vprev = _mm_cvtsi128_si32(vprevv);

			for (; pp < pStrideEnd; pp += 2)
			{
				pp[0] = htol16(uprev += *u);
				pp[1] = htol16(vprev += *v);

				++u;
				++v;
			}
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		__m128i uprevv = _mm_setzero_si128();
		__m128i vprevv = _mm_setzero_si128();

		auto lb = linebuf;
		for (; pp <= pStrideEnd - 16; pp += 16, lb += 16)
		{
			__m128i uu = _mm_loadu_si128((const __m128i*)u);
			auto uvalue = tuned_RestoreLeft16Element<F>(uprevv, uu);
			__m128i vv = _mm_loadu_si128((const __m128i*)v);
			auto vvalue = tuned_RestoreLeft16Element<F>(vprevv, vv);
			auto m0 = _mm_unpacklo_epi16(uvalue.v0, vvalue.v0);
			auto m1 = _mm_unpackhi_epi16(uvalue.v0, vvalue.v0);
			if (!NTSTORE)
			{
				_mm_storeu_si128((__m128i*)pp, _mm_add_epi16(m0, _mm_loadu_si128((const __m128i*)(pp - scbStride / 2))));
				_mm_storeu_si128((__m128i*)(pp + 8), _mm_add_epi16(m1, _mm_loadu_si128((const __m128i*)(pp + 8 - scbStride / 2))));
			}
			else
			{
				auto restored0 = _mm_add_epi16(m0, _mm_loadu_si128((const __m128i*)(lb)));
				auto restored1 = _mm_add_epi16(m1, _mm_loadu_si128((const __m128i*)(lb + 8)));
				_mm_stream_si128((__m128i*)pp, restored0);
				_mm_stream_si128((__m128i*)(pp + 8), restored1);
				_mm_storeu_si128((__m128i*)lb, restored0);
				_mm_storeu_si128((__m128i*)(lb + 8), restored1);
			}
			uprevv = uvalue.v1;
			vprevv = vvalue.v1;

			u += 8;
			v += 8;
		}

		if (!NTSTORE)
		{
			uprev = _mm_cvtsi128_si32(uprevv);
			vprev = _mm_cvtsi128_si32(vprevv);

			for (; pp < pStrideEnd; pp += 2)
			{
				pp[0] = htol16((uprev += *u) + ltoh16(pp[0 - scbStride / 2]));
				pp[1] = htol16((vprev += *v) + ltoh16(pp[1 - scbStride / 2]));

				++u;
				++v;
			}
		}
	}

	if (NTSTORE && Pred != CYLINDRICAL_LEFT)
		_aligned_free(linebuf);
}

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	if (IS_ALIGNED(pDstBegin, 16) && IS_MULTIPLE(cbWidth, 32) && IS_MULTIPLE(scbStride, 16))
		tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreImpl<F, Pred, true>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
	else
		tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreImpl<F, Pred, false>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_Restore<F, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F>
void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_Restore<F, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient<CODEFEATURE_SSE41>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient<CODEFEATURE_AVX1>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
#endif
