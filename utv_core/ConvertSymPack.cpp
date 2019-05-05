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

//

template<typename T>
void cpp_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	auto yp = pYPacked;
	auto up = pUPacked;
	auto vp = pVPacked;

	auto yc = pYControl;
	auto uc = pUControl;
	auto vc = pVControl;

	size_t cbYControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + 127) / 128) * 3;
	size_t cbCControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + 255) / 256) * 3;
	memset(pYControl, 0, cbYControl);
	memset(pUControl, 0, cbCControl);
	memset(pVControl, 0, cbCControl);

	int yshift = 0;
	int ushift = 0;
	int vshift = 0;

	union padsolve
	{
		uint8_t b[8];
		uint64_t w;
	};

	{
		const auto p = pSrcBegin;
		auto pp = p;

		uint8_t yprevb = 0x80;
		uint8_t uprevb = 0x80;
		uint8_t vprevb = 0x80;

		while (pp < p + cbWidth)
		{
			int n = 0;
			padsolve yps[2], ups, vps;

			for (; pp < p + cbWidth && n < 8; pp += 4, ++n)
			{
				uint8_t yy0 = pp[T::Y0];
				yps[0].b[n * 2] = yy0 - yprevb;
				uint8_t yy1 = pp[T::Y1];
				yps[0].b[n * 2 + 1] = yy1 - yy0;
				uint8_t uu = pp[T::U];
				ups.b[n] = uu - uprevb;
				uint8_t vv = pp[T::V];
				vps.b[n] = vv - vprevb;

				yprevb = yy1;
				uprevb = uu;
				vprevb = vv;
			}
			for (; n < 8; ++n)
			{
				yps[0].b[n * 2] = 0;
				yps[0].b[n * 2 + 1] = 0;
				ups.b[n] = 0;
				vps.b[n] = 0;
			}

			cpp_PackForIntra(yp, yc, yshift, yps[0].w);
			cpp_PackForIntra(yp, yc, yshift, yps[1].w);
			cpp_PackForIntra(up, uc, ushift, ups.w);
			cpp_PackForIntra(vp, vc, vshift, vps.w);
		}

		if (yshift != 0)
		{
			yshift = 0;
			yc += 3;
		}

		if (ushift != 0)
		{
			ushift = 0;
			vshift = 0;

			uc += 3;
			vc += 3;
		}
	}

	for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		uint8_t yprevb = 0;
		uint8_t uprevb = 0;
		uint8_t vprevb = 0;

		while (pp < p + cbWidth)
		{
			int n = 0;
			padsolve yps[2], ups, vps;

			for (; pp < p + cbWidth && n < 8; pp += 4, ++n)
			{
				uint8_t yy0 = pp[T::Y0] - (pp - scbStride)[T::Y0];
				yps[0].b[n * 2] = yy0 - yprevb;
				uint8_t yy1 = pp[T::Y1] - (pp - scbStride)[T::Y1];
				yps[0].b[n * 2 + 1] = yy1 - yy0;
				uint8_t uu = pp[T::U] - (pp - scbStride)[T::U];
				ups.b[n] = uu - uprevb;
				uint8_t vv = pp[T::V] - (pp - scbStride)[T::V];
				vps.b[n] = vv - vprevb;

				yprevb = yy1;
				uprevb = uu;
				vprevb = vv;
			}
			for (; n < 8; ++n)
			{
				yps[0].b[n * 2] = 0;
				yps[0].b[n * 2 + 1] = 0;
				ups.b[n] = 0;
				vps.b[n] = 0;
			}

			cpp_PackForIntra(yp, yc, yshift, yps[0].w);
			cpp_PackForIntra(yp, yc, yshift, yps[1].w);
			cpp_PackForIntra(up, uc, ushift, ups.w);
			cpp_PackForIntra(vp, vc, vshift, vps.w);
		}

		if (yshift != 0)
		{
			yshift = 0;
			yc += 3;
		}

		if (ushift != 0)
		{
			ushift = 0;
			vshift = 0;

			uc += 3;
			vc += 3;
		}
	}

	*cbYPacked = yp - pYPacked;
	*cbUPacked = up - pUPacked;
	*cbVPacked = vp - pVPacked;
}

template void cpp_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CYUYVColorOrder>(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CUYVYColorOrder>(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
