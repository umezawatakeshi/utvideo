/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ConvertPredict.h"
#include "ColorOrder.h"
#include "Coefficient.h"
#include "ByteOrder.h"

template<class T, bool Gradient>
static inline void cpp_ConvertRGBToULRG_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;

	for (auto p = pSrcBegin; p != (Gradient ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

		for (; pp < p + cbWidth; pp += T::BYPP)
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

		gprevb = 0;
		bprevb = 0;
		rprevb = 0;

		for (; pp < p + cbWidth; pp += T::BYPP)
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

template<class T>
void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	cpp_ConvertRGBToULRG_PredictAndCount<T, false>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
}

template<class T>
void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	cpp_ConvertRGBToULRG_PredictAndCount<T, true>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
}

template void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void cpp_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount<CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void cpp_ConvertRGBToULRG_PredictPlanarGradientAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);

//

template<class T, bool Gradient>
static inline void cpp_ConvertRGBAToULRA_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
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

		for (; pp < p + cbWidth; pp += T::BYPP)
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
			uint8_t aa = pp[T::A];
			*a = aa - aprevb;
			++pACountTable[*a];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			aprevb = aa;

			b += 1;
			g += 1;
			r += 1;
			a += 1;
		}
	}

	if (Gradient) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		gprevb = 0;
		bprevb = 0;
		rprevb = 0;
		aprevb = 0;

		for (; pp < p + cbWidth; pp += T::BYPP)
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
			uint8_t aa = pp[T::A] - (pp - scbStride)[T::A];
			*a = aa - aprevb;
			++pACountTable[*a];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			aprevb = aa;

			b += 1;
			g += 1;
			r += 1;
			a += 1;
		}
	}
}

template<class T>
void cpp_ConvertRGBAToULRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	cpp_ConvertRGBAToULRA_PredictAndCount<T, false>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<class T>
void cpp_ConvertRGBAToULRA_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	cpp_ConvertRGBAToULRA_PredictAndCount<T, true>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template void cpp_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void cpp_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void cpp_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void cpp_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);

//

template<class T, bool Gradient>
static inline void cpp_ConvertULRGToRGB_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;

	auto g = pGBegin;
	auto b = pBBegin;
	auto r = pRBegin;

	for (auto p = pDstBegin; p != (Gradient ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			pp[T::G] = gprevb += g[0];
			pp[T::B] = (bprevb += b[0]) + gprevb;
			pp[T::R] = (rprevb += r[0]) + gprevb;
			if (T::BYPP == 4)
				pp[T::A] = 0xff;

			g += 1;
			b += 1;
			r += 1;
		}
	}

	if (Gradient) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		gprevb = 0;
		bprevb = 0;
		rprevb = 0;

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			pp[T::G] = (gprevb += g[0]) + (pp - scbStride)[T::G];
			pp[T::B] = (bprevb += b[0]) + (pp - scbStride)[T::B] + gprevb;
			pp[T::R] = (rprevb += r[0]) + (pp - scbStride)[T::R] + gprevb;
			if (T::BYPP == 4)
				pp[T::A] = 0xff;

			g += 1;
			b += 1;
			r += 1;
		}
	}
}

template<class T>
void cpp_ConvertULRGToRGB_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRGToRGB_Restore<T, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULRGToRGB_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRGToRGB_Restore<T, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
}

template void cpp_ConvertULRGToRGB_RestoreCylindricalLeft<CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestoreCylindricalLeft<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestoreCylindricalLeft<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestorePlanarGradient<CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestorePlanarGradient<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB_RestorePlanarGradient<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);

//

template<class T, bool Gradient>
void cpp_ConvertULRAToRGBA_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
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

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			pp[T::G] = gprevb += g[0];
			pp[T::B] = (bprevb += b[0]) + gprevb;
			pp[T::R] = (rprevb += r[0]) + gprevb;
			pp[T::A] = aprevb += a[0];

			g += 1;
			b += 1;
			r += 1;
			a += 1;
		}
	}

	if (Gradient) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		gprevb = 0;
		bprevb = 0;
		rprevb = 0;
		aprevb = 0;

		for (; pp < p + cbWidth; pp += T::BYPP)
		{
			pp[T::G] = (gprevb += g[0]) + (pp - scbStride)[T::G];
			pp[T::B] = (bprevb += b[0]) + (pp - scbStride)[T::B] + gprevb;
			pp[T::R] = (rprevb += r[0]) + (pp - scbStride)[T::R] + gprevb;
			pp[T::A] = (aprevb += a[0]) + (pp - scbStride)[T::A];

			g += 1;
			b += 1;
			r += 1;
			a += 1;
		}
	}
}

template<class T>
void cpp_ConvertULRAToRGBA_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRAToRGBA_Restore<T, false>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULRAToRGBA_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULRAToRGBA_Restore<T, true>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template void cpp_ConvertULRAToRGBA_RestoreCylindricalLeft<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestoreCylindricalLeft<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestorePlanarGradient<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToRGBA_RestorePlanarGradient<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);

//

template<class T, bool Gradient>
static inline void cpp_ConvertPackedYUV422ToULY2_PredictAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
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

		yprevb = 0;
		uprevb = 0;
		vprevb = 0;

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

template<class T>
void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	cpp_ConvertPackedYUV422ToULY2_PredictAndCount<T, false>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<class T>
void cpp_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	cpp_ConvertPackedYUV422ToULY2_PredictAndCount<T, true>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void cpp_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void cpp_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void cpp_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);

//

template<class T, bool Gradient>
static inline void cpp_ConvertULY2ToPackedYUV422_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
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
}

template<class T>
void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToPackedYUV422_Restore<T, false>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<class T>
void cpp_ConvertULY2ToPackedYUV422_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToPackedYUV422_Restore<T, true>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
