/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULRACodec.h"
#include "Predict.h"
#include "Convert.h"
#include "ConvertPredict.h"
#include "TunedFunc.h"

const utvf_t CULRACodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGRA_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGRA_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRACodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGRA_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGRA_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRACodec::m_utvfCompressed[] = {
	UTVF_ULRA,
	UTVF_INVALID,
};

CULRACodec::CULRACodec(const char *pszInterfaceName) : CUL00Codec("ULRA", pszInterfaceName)
{
}

void CULRACodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
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

void CULRACodec::ConvertToPlanar(uint32_t nBandIndex)
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

void CULRACodec::ConvertFromPlanar(uint32_t nBandIndex)
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

bool CULRACodec::PredictDirect(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r, *a;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pPredicted->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pPredicted->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pPredicted->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pPredicted->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRAToULRA_PredictCylindricalLeftAndCount(g, b, r, a, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_BGRA_TD:
			ConvertBGRAToULRA_PredictCylindricalLeftAndCount(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertARGBToULRA_PredictCylindricalLeftAndCount(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRAToULRA_PredictPlanarGradientAndCount(g, b, r, a, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_BGRA_TD:
			ConvertBGRAToULRA_PredictPlanarGradientAndCount(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertARGBToULRA_PredictPlanarGradientAndCount(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRAToULRA_PredictCylindricalWrongMedianAndCount(g, b, r, a, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_BGRA_TD:
			ConvertBGRAToULRA_PredictCylindricalWrongMedianAndCount(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertARGBToULRA_PredictCylindricalWrongMedianAndCount(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		}
		break;
	}

	return false;
}

bool CULRACodec::RestoreDirect(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r, *a;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pPredicted->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pPredicted->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pPredicted->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pPredicted->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			ConvertULRAToBGRA_RestoreCylindricalLeft(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, a, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRA_TD:
			ConvertULRAToBGRA_RestoreCylindricalLeft(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRAToARGB_RestoreCylindricalLeft(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			ConvertULRAToBGRA_RestorePlanarGradient(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, a, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRA_TD:
			ConvertULRAToBGRA_RestorePlanarGradient(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRAToARGB_RestorePlanarGradient(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			ConvertULRAToBGRA_RestoreCylindricalWrongMedian(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, a, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRA_TD:
			ConvertULRAToBGRA_RestoreCylindricalWrongMedian(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRAToARGB_RestoreCylindricalWrongMedian(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	}
	return false;
}
