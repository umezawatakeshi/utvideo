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

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRAToULRA(g, b, r, a, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRA_TD:
		ConvertBGRAToULRA(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertARGBToULRA(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	}
}

void CUMRACodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r, *a;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
		ConvertULRAToBGRA(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, a, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRA_TD:
		ConvertULRAToBGRA(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRAToARGB(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	}
}

bool CUMRACodec::EncodeDirect(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r, *a;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	size_t cbPlane = (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[0];

	if (m_nKeyFrameInterval <= 1)
	{
		m_cbControlStream[0][nBandIndex] = cbPlane / 64 * 3;
		m_cbControlStream[1][nBandIndex] = cbPlane / 64 * 3;
		m_cbControlStream[2][nBandIndex] = cbPlane / 64 * 3;
		m_cbControlStream[3][nBandIndex] = cbPlane / 64 * 3;

		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRAToULRA_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				m_pPackedStream[3][nBandIndex], &m_cbPackedStream[3][nBandIndex],
				m_pControlStream[3][nBandIndex],
				pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRA_TD:
			ConvertBGRAToULRA_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				m_pPackedStream[3][nBandIndex], &m_cbPackedStream[3][nBandIndex],
				m_pControlStream[3][nBandIndex],
				pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertARGBToULRA_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				m_pPackedStream[3][nBandIndex], &m_cbPackedStream[3][nBandIndex],
				m_pControlStream[3][nBandIndex],
				pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
	}

	return false;
}

bool CUMRACodec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
