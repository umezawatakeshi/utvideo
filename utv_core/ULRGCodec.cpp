/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
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
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULRG(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULRG(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULRG(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULRG(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	}
}

void CULRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertULRGToBGR(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertULRGToBGRX(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULRGToRGB(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRGToXRGB(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	}
}

bool CULRGCodec::PredictDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_PredictCylindricalLeftAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_PredictCylindricalLeftAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_PredictCylindricalLeftAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_PredictCylindricalLeftAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_PredictPlanarGradientAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_PredictPlanarGradientAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_PredictPlanarGradientAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_PredictPlanarGradientAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	}

	return false;
}

bool CULRGCodec::RestoreDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	}

	return false;
}

bool CULRGCodec::IsDirectRestorable()
{
	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGR_TD:
		case UTVF_NFCC_BGRX_TD:
		case UTVF_NFCC_RGB_TD:
		case UTVF_NFCC_ARGB_TD:
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGR_TD:
		case UTVF_NFCC_BGRX_TD:
		case UTVF_NFCC_RGB_TD:
		case UTVF_NFCC_ARGB_TD:
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGR_TD:
		case UTVF_NFCC_BGRX_TD:
		case UTVF_NFCC_RGB_TD:
		case UTVF_NFCC_ARGB_TD:
			return true;
		}
		break;
	}

	return false;
}
