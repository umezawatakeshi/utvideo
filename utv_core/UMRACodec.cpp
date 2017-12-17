/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UMRACodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

const utvf_t CUMRACodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGRA_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGRA_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CUMRACodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGRA_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGRA_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CUMRACodec::m_utvfCompressed[] = {
	UTVF_UMRA,
	UTVF_INVALID,
};

CUMRACodec::CUMRACodec(const char *pszInterfaceName) : CUM00Codec("UMRA", pszInterfaceName)
{
}

void CUMRACodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	width = ROUNDUP(width, 64);

	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;
	m_cbPlaneSize[3]          = width * height;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;
	m_cbPlaneWidth[3]         = width;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;
	m_cbPlaneStripeSize[3]    = width;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
	m_cbPlanePredictStride[3] = width;
}

void CUMRACodec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r, *a;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRAToULRA(g, b, r, a, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRA_TD:
		ConvertBGRAToULRA(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertARGBToULRA(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

void CUMRACodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r, *a;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
		ConvertULRAToBGRA(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, a, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRA_TD:
		ConvertULRAToBGRA(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRAToARGB(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

bool CUMRACodec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
