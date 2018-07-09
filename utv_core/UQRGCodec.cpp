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
	UTVF_r210,
	UTVF_b48r,
	UTVF_b64a,
	UTVF_INVALID,
};

const utvf_t CUQRGCodec::m_utvfDecoderOutput[] = {
	UTVF_r210,
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
	uint8_t *g, *b, *r;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_b48r:
		ConvertB48rToUQRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_b64a:
		ConvertB64aToUQRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_r210:
		cpp_ConvertR210ToUQRG(g, b, r, pSrcBegin, pSrcEnd, m_nWidth, m_cbRawGrossWidth);
		break;
	}
}

void CUQRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_b48r:
		ConvertUQRGToB48r(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_b64a:
		ConvertUQRGToB64a(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_r210:
		cpp_ConvertUQRGToR210(pDstBegin, pDstEnd, g, b, r, m_nWidth, m_cbRawGrossWidth);
		break;
	}
}

bool CUQRGCodec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
