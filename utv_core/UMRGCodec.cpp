/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
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

void CUMRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
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

bool CUMRGCodec::EncodeDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, _] = CalcBandPosition<true>(nBandIndex);

	size_t cbPlane = (m_dwStripeEnd[nBandIndex] - m_dwStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[0];

	if (m_nKeyFrameInterval <= 1)
	{
		m_cbControlStream[0][nBandIndex] = cbPlane / 64 * 3;
		m_cbControlStream[1][nBandIndex] = cbPlane / 64 * 3;
		m_cbControlStream[2][nBandIndex] = cbPlane / 64 * 3;

		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertBGRXToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertXRGBToULRG_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
	}

	return false;
}

bool CUMRGCodec::DecodeDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, _] = CalcBandPosition<false>(nBandIndex);

	if (!(m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION))
	{
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
			ConvertULRGToBGR_Unpack8SymAndRestorePlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRX_TD:
			ConvertULRGToBGRX_Unpack8SymAndRestorePlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_RGB_TD:
			ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRGToXRGB_Unpack8SymAndRestorePlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
	}

	return false;
}
