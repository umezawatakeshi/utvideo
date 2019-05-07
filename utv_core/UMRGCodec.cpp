/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UMRGCodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

const utvf_t CUMRGCodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CUMRGCodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CUMRGCodec::m_utvfCompressed[] = {
	UTVF_UMRG,
	UTVF_INVALID,
};

CUMRGCodec::CUMRGCodec(const char *pszInterfaceName) : CUM00Codec("UMRG", pszInterfaceName)
{
}

void CUMRGCodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	width = ROUNDUP(width, 64);

	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
}

void CUMRGCodec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertBGRToULRG(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRXToULRG(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULRG(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	}
}

void CUMRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertULRGToBGR(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULRGToBGRX(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULRGToBGR(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULRGToBGRX(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULRGToRGB(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRGToXRGB(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0]);
		break;
	}
}

bool CUMRGCodec::EncodeDirect(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	size_t cbPlane = (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[0];

	if (m_nKeyFrameInterval <= 1)
	{
		m_cbControlStream[0][nBandIndex] = cbPlane / 64 * 3;
		m_cbControlStream[1][nBandIndex] = cbPlane / 64 * 3;
		m_cbControlStream[2][nBandIndex] = cbPlane / 64 * 3;

		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRXToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
	}

	return false;
}

bool CUMRGCodec::DecodeDirect(uint32_t nBandIndex)
{
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex] * m_cbRawStripeSize;

	if (!(m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION))
	{
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertULRGToBGR_Unpack8SymAndRestorePlanarGradient8(
				pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth,
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertULRGToBGRX_Unpack8SymAndRestorePlanarGradient8(
				pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth,
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_Unpack8SymAndRestorePlanarGradient8(
				pDstBegin, pDstEnd,
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_Unpack8SymAndRestorePlanarGradient8(
				pDstBegin, pDstEnd,
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8(
				pDstBegin, pDstEnd,
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_Unpack8SymAndRestorePlanarGradient8(
				pDstBegin, pDstEnd,
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
	}

	return false;
}
