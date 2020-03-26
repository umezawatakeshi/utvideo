/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
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

void CUMRACodec::ConvertFromPlanar(uint32_t nBandIndex)
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

bool CUMRACodec::EncodeDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, _] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);

	size_t cbPlane = (m_dwStripeEnd[nBandIndex] - m_dwStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[0];

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
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
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
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
	}

	return false;
}

bool CUMRACodec::DecodeDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, _] = CalcBandPosition<false>(nBandIndex);

	if (!(m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION))
	{
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRA_TD:
			ConvertULRAToBGRA_Unpack8SymAndRestorePlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_pPackedStream[3][nBandIndex], m_pControlStream[3][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_NFCC_ARGB_TD:
			ConvertULRAToARGB_Unpack8SymAndRestorePlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_pPackedStream[3][nBandIndex], m_pControlStream[3][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
	}

	return false;
}
