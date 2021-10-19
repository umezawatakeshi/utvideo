/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
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
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRA_TD:
		ConvertBGRAToULRA(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertARGBToULRA(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	}
}

void CULRACodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRA_TD:
		ConvertULRAToBGRA(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRAToARGB(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0]);
		break;
	}
}

bool CULRACodec::PredictDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRA_TD:
			ConvertBGRAToULRA_PredictCylindricalLeftAndCount(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertARGBToULRA_PredictCylindricalLeftAndCount(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
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
		case UTVF_NFCC_BGRA_TD:
			ConvertBGRAToULRA_PredictPlanarGradientAndCount(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertARGBToULRA_PredictPlanarGradientAndCount(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
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
		case UTVF_NFCC_BGRA_TD:
			if (m_bRequirePreCounting)
				ConvertBGRAToULRA_PredictCylindricalWrongMedianAndCount(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			else
				ConvertBGRAToULRA_PredictCylindricalWrongMedian(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			if (m_bRequirePreCounting)
				ConvertARGBToULRA_PredictCylindricalWrongMedianAndCount(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			else
				ConvertARGBToULRA_PredictCylindricalWrongMedian(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	}

	return false;
}

bool CULRACodec::RestoreDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRA_TD:
			ConvertULRAToBGRA_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRAToARGB_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
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
		case UTVF_NFCC_BGRA_TD:
			ConvertULRAToBGRA_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRAToARGB_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
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
		case UTVF_NFCC_BGRA_TD:
			ConvertULRAToBGRA_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRAToARGB_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	}
	return false;
}

bool CULRACodec::IsDirectRestorable()
{
	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRA_TD:
		case UTVF_NFCC_ARGB_TD:
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
		case UTVF_NFCC_BGRA_TD:
		case UTVF_NFCC_ARGB_TD:
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
		case UTVF_NFCC_BGRA_TD:
		case UTVF_NFCC_ARGB_TD:
			return true;
		}
		break;
	}
	return false;
}
