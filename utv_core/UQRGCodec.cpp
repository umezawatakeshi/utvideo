/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQRGCodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQRGCodec::m_utvfEncoderInput[] = {
	UTVF_b48r,
	UTVF_b64a,
	UTVF_INVALID,
};

const utvf_t CUQRGCodec::m_utvfDecoderOutput[] = {
	UTVF_b48r,
	UTVF_b64a,
	UTVF_INVALID,
};

const utvf_t CUQRGCodec::m_utvfCompressed[] = {
	UTVF_UQRG,
	UTVF_INVALID,
};

CUQRGCodec::CUQRGCodec(const char *pszInterfaceName) : CUQ00Codec("UQRG", pszInterfaceName)
{
	m_syInitialPredict[0] = 0x200;
	m_syInitialPredict[1] = 0x200;
	m_syInitialPredict[2] = 0x200;
}

void CUQRGCodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height * 2;
	m_cbPlaneSize[1]          = width * height * 2;
	m_cbPlaneSize[2]          = width * height * 2;

	m_cbPlaneWidth[0]         = width * 2;
	m_cbPlaneWidth[1]         = width * 2;
	m_cbPlaneWidth[2]         = width * 2;

	m_cbPlaneStripeSize[0]    = width * 2;
	m_cbPlaneStripeSize[1]    = width * 2;
	m_cbPlaneStripeSize[2]    = width * 2;

	m_cbPlanePredictStride[0] = width * 2;
	m_cbPlanePredictStride[1] = width * 2;
	m_cbPlanePredictStride[2] = width * 2;
}

void CUQRGCodec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint16_t *g, *b, *r;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = (uint16_t *)(m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0]);
	b = (uint16_t *)(m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1]);
	r = (uint16_t *)(m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2]);

	switch (m_utvfRaw)
	{
	case UTVF_b48r:
		for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += m_cbRawGrossWidth)
		{
			const uint8_t *pStrideEnd = pStrideBegin + m_cbRawNetWidth;
			for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += 6)
			{
				const uint16_t *pp = (const uint16_t *)p;
				uint16_t gg = Convert16To10Fullrange(btoh16(pp[1]));
				uint16_t bb = Convert16To10Fullrange(btoh16(pp[2]));
				uint16_t rr = Convert16To10Fullrange(btoh16(pp[0]));

				*g++ = gg;
				*b++ = (bb - gg + 0x200) & 0x3ff;
				*r++ = (rr - gg + 0x200) & 0x3ff;
			}
		}
		break;
	case UTVF_b64a:
		for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += m_cbRawGrossWidth)
		{
			const uint8_t *pStrideEnd = pStrideBegin + m_cbRawNetWidth;
			for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += 8)
			{
				const uint16_t *pp = (const uint16_t *)p;
				uint16_t gg = Convert16To10Fullrange(btoh16(pp[2]));
				uint16_t bb = Convert16To10Fullrange(btoh16(pp[3]));
				uint16_t rr = Convert16To10Fullrange(btoh16(pp[1]));

				*g++ = gg;
				*b++ = (bb - gg + 0x200) & 0x3ff;
				*r++ = (rr - gg + 0x200) & 0x3ff;
			}
		}
		break;
	}
}

void CUQRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint16_t *g, *b, *r;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = (uint16_t *)(m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0]);
	b = (uint16_t *)(m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1]);
	r = (uint16_t *)(m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2]);

	switch (m_utvfRaw)
	{
	case UTVF_b48r:
		for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += m_cbRawGrossWidth)
		{
			uint8_t *pStrideEnd = pStrideBegin + m_cbRawNetWidth;
			for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += 6)
			{
				uint16_t *pp = (uint16_t *)p;

				pp[1] = htob16(Convert10To16Fullrange(*g));
				pp[2] = htob16(Convert10To16Fullrange(*b + *g - 0x200));
				pp[0] = htob16(Convert10To16Fullrange(*r + *g - 0x200));
				g++; b++; r++;
			}
		}
		break;
	case UTVF_b64a:
		for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += m_cbRawGrossWidth)
		{
			uint8_t *pStrideEnd = pStrideBegin + m_cbRawNetWidth;
			for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += 8)
			{
				uint16_t *pp = (uint16_t *)p;

				pp[2] = htob16(Convert10To16Fullrange(*g));
				pp[3] = htob16(Convert10To16Fullrange(*b + *g - 0x200));
				pp[1] = htob16(Convert10To16Fullrange(*r + *g - 0x200));
				pp[0] = 0xffff;
				g++; b++; r++;
			}
		}
		break;
	}
}

bool CUQRGCodec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
