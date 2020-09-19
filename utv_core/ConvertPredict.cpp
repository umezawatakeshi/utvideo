/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Convert.h"
#include "Predict.h"
#include "ConvertPredict.h"
#include "ColorOrder.h"
#include "Coefficient.h"
#include "ByteOrder.h"

template<class T, bool A, PREDICTION_TYPE Pred>
static inline void cpp_ConvertRGBXToULRX_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
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

		gprevb = 0;
		bprevb = 0;
		rprevb = 0;
		if (A)
			aprevb = 0;

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

template<class T>
void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256])
{
	cpp_ConvertRGBXToULRX_PredictAndCount<T, false, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<class T>
void cpp_ConvertRGBAToULRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
{
	cpp_ConvertRGBXToULRX_PredictAndCount<T, true, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<class T>
void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256])
{
	cpp_ConvertRGBXToULRX_PredictAndCount<T, false, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<class T>
void cpp_ConvertRGBAToULRA_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
{
	cpp_ConvertRGBXToULRX_PredictAndCount<T, true, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<class T>
void cpp_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256])
{
	cpp_ConvertRGBXToULRX_PredictAndCount<T, false, CYLINDRICAL_WRONG_MEDIAN>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<class T>
void cpp_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256])
{
	cpp_ConvertRGBXToULRX_PredictAndCount<T, true, CYLINDRICAL_WRONG_MEDIAN>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void cpp_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount<CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void cpp_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template void cpp_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template void cpp_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);

//

template<class T, bool A, PREDICTION_TYPE Pred>
void cpp_ConvertULRXToRGBX_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;
	uint8_t aprevb = 0x80;

	auto g = pGBegin;
	auto b = pBBegin;
	auto r = pRBegin;
	auto a = pABegin;

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

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

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		gprevb = 0;
		bprevb = 0;
		rprevb = 0;
		if (A)
			aprevb = 0;

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
}

template<class T>
void cpp_ConvertULRGToRGB_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRXToRGBX_Restore<T, false, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULRAToRGBA_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRXToRGBX_Restore<T, true, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULRGToRGB_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRXToRGBX_Restore<T, false, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULRAToRGBA_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRXToRGBX_Restore<T, true, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULRGToRGB_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRXToRGBX_Restore<T, false, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULRAToRGBA_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRXToRGBX_Restore<T, true, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template void cpp_ConvertULRGToRGB_RestoreCylindricalLeft<CBGRColorOrder>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestoreCylindricalLeft<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestoreCylindricalLeft<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestoreCylindricalLeft<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestoreCylindricalLeft<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestorePlanarGradient<CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestorePlanarGradient<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestorePlanarGradient<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestorePlanarGradient<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestorePlanarGradient<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);

//

template<class T, PREDICTION_TYPE Pred>
static inline void cpp_ConvertPackedYUV422ToULY2_PredictAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
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

		yprevb = 0;
		uprevb = 0;
		vprevb = 0;

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

template<class T>
void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	cpp_ConvertPackedYUV422ToULY2_PredictAndCount<T, CYLINDRICAL_LEFT>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<class T>
void cpp_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	cpp_ConvertPackedYUV422ToULY2_PredictAndCount<T, PLANAR_GRADIENT>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<class T>
void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	cpp_ConvertPackedYUV422ToULY2_PredictAndCount<T, CYLINDRICAL_WRONG_MEDIAN>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void cpp_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void cpp_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);

//

template<class T, PREDICTION_TYPE Pred>
static inline void cpp_ConvertULY2ToPackedYUV422_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t yprevb = 0x80;
	uint8_t uprevb = 0x80;
	uint8_t vprevb = 0x80;

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

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

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		yprevb = 0;
		uprevb = 0;
		vprevb = 0;

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

	yprevb = 0;
	uprevb = 0;
	vprevb = 0;

	uint8_t ytopprevb = 0;
	uint8_t utopprevb = 0;
	uint8_t vtopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t ytop0 = (pp - scbStride)[T::Y0];
			uint8_t yy0 = *y + median<uint8_t>(yprevb, ytop0, yprevb + ytop0 - ytopprevb);
			pp[T::Y0] = yy0;
			uint8_t ytop1 = (pp - scbStride)[T::Y1];
			uint8_t yy1 = *(y + 1) + median<uint8_t>(yy0, ytop1, yy0 + ytop1 - ytop0);
			pp[T::Y1] = yy1;
			uint8_t utop = (pp - scbStride)[T::U];
			uint8_t uu = *u + median<uint8_t>(uprevb, utop, uprevb + utop - utopprevb);
			pp[T::U] = uu;
			uint8_t vtop = (pp - scbStride)[T::V];
			uint8_t vv = *v + median<uint8_t>(vprevb, vtop, vprevb + vtop - vtopprevb);
			pp[T::V] = vv;

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

template<class T>
void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToPackedYUV422_Restore<T, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULY2ToPackedYUV422_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToPackedYUV422_Restore<T, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToPackedYUV422_Restore<T, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);

//

template<PREDICTION_TYPE Pred>
static inline void cpp_ConvertPackedUVToPlanar_PredictAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	uint8_t uprev = 0x80;
	uint8_t vprev = 0x80;

	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

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

		uprev = 0;
		vprev = 0;

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

void cpp_ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	cpp_ConvertPackedUVToPlanar_PredictAndCount<CYLINDRICAL_LEFT>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

void cpp_ConvertPackedUVToPlanar_PredictPlanarGradientAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	cpp_ConvertPackedUVToPlanar_PredictAndCount<PLANAR_GRADIENT>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

void cpp_ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256])
{
	cpp_ConvertPackedUVToPlanar_PredictAndCount<CYLINDRICAL_WRONG_MEDIAN>(pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pUCountTable, pVCountTable);
}

//

template<PREDICTION_TYPE Pred>
static inline void cpp_ConvertPlanarToPackedUV_Restore(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t uprev = 0x80;
	uint8_t vprev = 0x80;

	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		for (; pp < p + cbWidth; pp += 2)
		{
			pp[0] = uprev += *u;
			pp[1] = vprev += *v;

			++u;
			++v;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		uprev = 0;
		vprev = 0;

		for (; pp < p + cbWidth; pp += 2)
		{
			pp[0] = (uprev += *u) + (pp - scbStride)[0];
			pp[1] = (vprev += *v) + (pp - scbStride)[1];

			++u;
			++v;
		}
	}

	uprev = 0;
	vprev = 0;

	uint8_t utopprev = 0;
	uint8_t vtopprev = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

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
}

void cpp_ConvertPlanarToPackedUV_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertPlanarToPackedUV_Restore<CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

void cpp_ConvertPlanarToPackedUV_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertPlanarToPackedUV_Restore<PLANAR_GRADIENT>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

void cpp_ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertPlanarToPackedUV_Restore<CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pUBegin, pVBegin, cbWidth, scbStride);
}

//

template<bool A>
static inline void cpp_ConvertB64aToUQRX_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024])
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;
	uint16_t aprevb = 0x200;

	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *a = (uint16_t *)pABegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		for (; pp < p + cbWidth; pp += 8)
		{
			uint16_t gg = Convert16To10Fullrange(btoh16(((uint16_t *)pp)[2]));
			*g = (gg - gprevb) & 0x3ff;
			++pGCountTable[0][*g];
			uint16_t bb = Convert16To10Fullrange(btoh16(((uint16_t *)pp)[3])) - gg;
			*b = (bb - bprevb) & 0x3ff;
			++pBCountTable[0][*b];
			uint16_t rr = Convert16To10Fullrange(btoh16(((uint16_t *)pp)[1])) - gg;
			*r = (rr - rprevb) & 0x3ff;
			++pRCountTable[0][*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A) {
				uint16_t aa = Convert16To10Fullrange(btoh16(((uint16_t *)pp)[0]));
				*a = (aa - aprevb) & 0x3ff;
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
}

void cpp_ConvertB64aToUQRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024])
{
	cpp_ConvertB64aToUQRX_PredictCylindricalLeftAndCount<false>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

void cpp_ConvertB64aToUQRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024])
{
	cpp_ConvertB64aToUQRX_PredictCylindricalLeftAndCount<true>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

//

template<bool A>
static inline void cpp_ConvertUQRXToB64a_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;
	uint16_t aprevb = 0x200;

	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *a = (uint16_t *)pABegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		for (; pp < p + cbWidth; pp += 8)
		{
			((uint16_t *)pp)[2] = htob16(Convert10To16Fullrange(gprevb += g[0]));
			((uint16_t *)pp)[3] = htob16(Convert10To16Fullrange((bprevb += b[0]) + gprevb));
			((uint16_t *)pp)[1] = htob16(Convert10To16Fullrange((rprevb += r[0]) + gprevb));
			if (A)
				((uint16_t *)pp)[0] = htob16(Convert10To16Fullrange(aprevb += a[0]));
			else
				((uint16_t *)pp)[0] = 0xffff;

			g += 1;
			b += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}
}

void cpp_ConvertUQRGToB64a_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertUQRXToB64a_RestoreCylindricalLeft<false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

void cpp_ConvertUQRAToB64a_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertUQRXToB64a_RestoreCylindricalLeft<true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

//

void cpp_ConvertR210ToUQRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024])
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;

	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		const uint8_t *pStrideEnd = p + nWidth * 4;
		auto pp = p;

		for (; pp < pStrideEnd; pp += 4)
		{
			uint32_t val = btoh32(*(const uint32_t *)pp);

			uint16_t gg = val >> 10;
			*g = (gg - gprevb) & 0x3ff;
			++pGCountTable[0][*g];
			uint16_t bb = val - gg;
			*b = (bb - bprevb) & 0x3ff;
			++pBCountTable[0][*b];
			uint16_t rr = (val >> 20) - gg;
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
}

//

void cpp_ConvertUQRGToR210_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride)
{
	uint16_t gprevb = 0x200;
	uint16_t bprevb = 0;
	uint16_t rprevb = 0;

	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		uint8_t *pStrideEnd = p + nWidth * 4;
		auto pp = p;

		for (; pp < pStrideEnd; pp += 4)
		{
			uint32_t gg = (gprevb += g[0]) & 0x3ff;
			uint32_t bb = ((bprevb += b[0]) + gprevb) & 0x3ff;
			uint32_t rr = ((rprevb += r[0]) + gprevb) & 0x3ff;
			*(uint32_t *)pp = htob32((rr << 20) | (gg << 10) | bb);

			g += 1;
			b += 1;
			r += 1;
		}
	}
}

//

template<VALUERANGE VR>
void cpp_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024])
{
	uint16_t prev = 0x200;

	auto q = (uint16_t*)pDstBegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pStrideEnd = (const uint16_t*)(p + cbWidth);
		auto pp = (const uint16_t*)p;

		for (; pp < pStrideEnd; ++pp)
		{
			uint16_t cur = Convert16To10<VR>(ltoh16(*pp));
			*q = (cur - prev) & 0x3ff;
			++pCountTable[0][*q];
			prev = cur;

			++q;
		}
	}
}

template void cpp_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<VALUERANGE::LIMITED>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template void cpp_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<VALUERANGE::NOROUND>(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);

//

void cpp_ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride)
{
	uint16_t prev = 0x8000;

	auto q = (const uint16_t*)pSrcBegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		for (; pp < pStrideEnd; ++pp)
		{
			uint16_t cur = *q;
			*pp = htol16(prev += cur);

			++q;
		}
	}
}

//

template<VALUERANGE VR>
void cpp_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024])
{
	uint16_t uprev = 0x200;
	uint16_t vprev = 0x200;

	auto u = (uint16_t*)pUBegin;
	auto v = (uint16_t*)pVBegin;

	for (auto p = pSrcBegin; p != pSrcEnd; p += scbStride)
	{
		auto pStrideEnd = (const uint16_t*)(p + cbWidth);
		auto pp = (const uint16_t*)p;

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
}

template void cpp_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template void cpp_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);

//

void cpp_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint16_t uprev = 0x8000;
	uint16_t vprev = 0x8000;

	auto u = (const uint16_t*)pUBegin;
	auto v = (const uint16_t*)pVBegin;

	for (auto p = pDstBegin; p != pDstEnd; p += scbStride)
	{
		auto pStrideEnd = (uint16_t*)(p + cbWidth);
		auto pp = (uint16_t*)p;

		for (; pp < pStrideEnd; pp += 2)
		{
			pp[0] = htol16(uprev += *u);
			pp[1] = htol16(vprev += *v);

			++u;
			++v;
		}
	}
}
