/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULRGCodec.h"
#include "Predict.h"
#include "Convert.h"
#include "ConvertPredict.h"
#include "TunedFunc.h"

const utvf_t CULRGCodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRGCodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRGCodec::m_utvfCompressed[] = {
	UTVF_ULRG,
	UTVF_INVALID,
};

CULRGCodec::CULRGCodec(const char *pszInterfaceName) : CUL00Codec("ULRG", pszInterfaceName)
{
}

void CULRGCodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
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

void CULRGCodec::ConvertToPlanar(uint32_t nBandIndex)
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

void CULRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
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

bool CULRGCodec::PredictDirect(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pPredicted->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pPredicted->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pPredicted->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertBGRToULRG_PredictCylindricalLeftAndCount(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRXToULRG_PredictCylindricalLeftAndCount(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_PredictCylindricalLeftAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_PredictCylindricalLeftAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_PredictCylindricalLeftAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_PredictCylindricalLeftAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertBGRToULRG_PredictPlanarGradientAndCount(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRXToULRG_PredictPlanarGradientAndCount(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_PredictPlanarGradientAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_PredictPlanarGradientAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_PredictPlanarGradientAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_PredictPlanarGradientAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertBGRToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertBGRXToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	}

	return false;
}

bool CULRGCodec::RestoreDirect(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pPredicted->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pPredicted->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pPredicted->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertULRGToBGR_RestoreCylindricalLeft(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertULRGToBGRX_RestoreCylindricalLeft(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_RestoreCylindricalLeft(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_RestoreCylindricalLeft(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_RestoreCylindricalLeft(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_RestoreCylindricalLeft(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertULRGToBGR_RestorePlanarGradient(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertULRGToBGRX_RestorePlanarGradient(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_RestorePlanarGradient(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_RestorePlanarGradient(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_RestorePlanarGradient(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_RestorePlanarGradient(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			ConvertULRGToBGR_RestoreCylindricalWrongMedian(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_BU:
			ConvertULRGToBGRX_RestoreCylindricalWrongMedian(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_RestoreCylindricalWrongMedian(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_RestoreCylindricalWrongMedian(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_RestoreCylindricalWrongMedian(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_RestoreCylindricalWrongMedian(pDstBegin, pDstEnd, g, b, r, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	}

	return false;
}
