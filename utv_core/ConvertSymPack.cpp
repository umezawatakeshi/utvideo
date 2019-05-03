/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "SymPack.h"
#include "ConvertSymPack.h"
#include "Predict.h"
#include "ColorOrder.h"
#include "Coefficient.h"
#include "ByteOrder.h"

template<typename T>
void cpp_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	auto gp = pGPacked;
	auto bp = pBPacked;
	auto rp = pRPacked;

	auto gc = pGControl;
	auto bc = pBControl;
	auto rc = pRControl;

	size_t cbControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + T::BYPP * 64 - 1) / (T::BYPP * 64)) * 3;
	memset(pGControl, 0, cbControl);
	memset(pBControl, 0, cbControl);
	memset(pRControl, 0, cbControl);

	int gshift = 0;
	int bshift = 0;
	int rshift = 0;

	union padsolve
	{
		uint8_t b[8];
		uint64_t w;
	};

	{
		const auto p = pSrcBegin;
		auto pp = p;

		uint8_t gprevb = 0x80;
		uint8_t bprevb = 0;
		uint8_t rprevb = 0;

		padsolve gps, bps, rps;

		while (pp < p + cbWidth)
		{
			int n = 0;
			for (; pp < p + cbWidth && n < 8; pp += T::BYPP, ++n)
			{
				uint8_t gg = pp[T::G];
				gps.b[n] = gg - gprevb;
				uint8_t bb = pp[T::B] - gg;
				bps.b[n] = bb - bprevb;
				uint8_t rr = pp[T::R] - gg;
				rps.b[n] = rr - rprevb;

				gprevb = gg;
				bprevb = bb;
				rprevb = rr;
			}
			for (; n < 8; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
			}

			cpp_PackForIntra(gp, gc, gshift, gps.w);
			cpp_PackForIntra(bp, bc, bshift, bps.w);
			cpp_PackForIntra(rp, rc, rshift, rps.w);
		}

		if (gshift != 0)
		{
			gshift = 0;
			bshift = 0;
			rshift = 0;

			gc += 3;
			bc += 3;
			rc += 3;
		}
	}

	for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		uint8_t gprevb = 0;
		uint8_t bprevb = 0;
		uint8_t rprevb = 0;

		padsolve gps, bps, rps;

		while (pp < p + cbWidth)
		{
			int n = 0;
			for (; pp < p + cbWidth && n < 8; pp += T::BYPP, ++n)
			{
				uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
				gps.b[n] = gg - gprevb;
				uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
				bps.b[n] = bb - bprevb;
				uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
				rps.b[n] = rr - rprevb;

				gprevb = gg;
				bprevb = bb;
				rprevb = rr;
			}
			for (; n < 8; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
			}

			cpp_PackForIntra(gp, gc, gshift, gps.w);
			cpp_PackForIntra(bp, bc, bshift, bps.w);
			cpp_PackForIntra(rp, rc, rshift, rps.w);
		}

		if (gshift != 0)
		{
			gshift = 0;
			bshift = 0;
			rshift = 0;

			gc += 3;
			bc += 3;
			rc += 3;
		}
	}

	*cbGPacked = gp - pGPacked;
	*cbBPacked = bp - pBPacked;
	*cbRPacked = rp - pRPacked;
}

template void cpp_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CBGRColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

//

template<class T>
void cpp_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	auto gp = pGPacked;
	auto bp = pBPacked;
	auto rp = pRPacked;
	auto ap = pAPacked;

	auto gc = pGControl;
	auto bc = pBControl;
	auto rc = pRControl;
	auto ac = pAControl;

	size_t cbControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + 255) / 256) * 3;
	memset(pGControl, 0, cbControl);
	memset(pBControl, 0, cbControl);
	memset(pRControl, 0, cbControl);
	memset(pAControl, 0, cbControl);

	int gshift = 0;
	int bshift = 0;
	int rshift = 0;
	int ashift = 0;

	union padsolve
	{
		uint8_t b[8];
		uint64_t w;
	};

	{
		const auto p = pSrcBegin;
		auto pp = p;

		uint8_t gprevb = 0x80;
		uint8_t bprevb = 0;
		uint8_t rprevb = 0;
		uint8_t aprevb = 0x80;

		padsolve gps, bps, rps, aps;

		while (pp < p + cbWidth)
		{
			int n = 0;
			for (; pp < p + cbWidth && n < 8; pp += 4, ++n)
			{
				uint8_t gg = pp[T::G];
				gps.b[n] = gg - gprevb;
				uint8_t bb = pp[T::B] - gg;
				bps.b[n] = bb - bprevb;
				uint8_t rr = pp[T::R] - gg;
				rps.b[n] = rr - rprevb;
				uint8_t aa = pp[T::A];
				aps.b[n] = aa - aprevb;

				gprevb = gg;
				bprevb = bb;
				rprevb = rr;
				aprevb = aa;
			}
			for (; n < 8; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
				aps.b[n] = 0;
			}

			cpp_PackForIntra(gp, gc, gshift, gps.w);
			cpp_PackForIntra(bp, bc, bshift, bps.w);
			cpp_PackForIntra(rp, rc, rshift, rps.w);
			cpp_PackForIntra(ap, ac, ashift, aps.w);
		}

		if (gshift != 0)
		{
			gshift = 0;
			bshift = 0;
			rshift = 0;
			ashift = 0;

			gc += 3;
			bc += 3;
			rc += 3;
			ac += 3;
		}
	}

	for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		uint8_t gprevb = 0;
		uint8_t bprevb = 0;
		uint8_t rprevb = 0;
		uint8_t aprevb = 0;

		padsolve gps, bps, rps, aps;

		while (pp < p + cbWidth)
		{
			int n = 0;
			for (; pp < p + cbWidth && n < 8; pp += 4, ++n)
			{
				uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
				gps.b[n] = gg - gprevb;
				uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
				bps.b[n] = bb - bprevb;
				uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
				rps.b[n] = rr - rprevb;
				uint8_t aa = pp[T::A] - (pp - scbStride)[T::A];
				aps.b[n] = aa - aprevb;

				gprevb = gg;
				bprevb = bb;
				rprevb = rr;
				aprevb = aa;
			}
			for (; n < 8; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
				aps.b[n] = 0;
			}

			cpp_PackForIntra(gp, gc, gshift, gps.w);
			cpp_PackForIntra(bp, bc, bshift, bps.w);
			cpp_PackForIntra(rp, rc, rshift, rps.w);
			cpp_PackForIntra(ap, ac, ashift, aps.w);
		}

		if (gshift != 0)
		{
			gshift = 0;
			bshift = 0;
			rshift = 0;
			ashift = 0;

			gc += 3;
			bc += 3;
			rc += 3;
			ac += 3;
		}
	}

	*cbGPacked = gp - pGPacked;
	*cbBPacked = bp - pBPacked;
	*cbRPacked = rp - pRPacked;
	*cbAPacked = ap - pAPacked;
}

template void cpp_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
