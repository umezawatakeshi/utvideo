/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
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
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_b48r:
		ConvertB48rToUQRG(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		break;
	case UTVF_b64a:
		ConvertB64aToUQRG(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		break;
	case UTVF_r210:
		ConvertR210ToUQRG(g, b, r, pRawBegin[0], pRawEnd[0], m_nWidth, m_fmRaw.scbLineStride[0]);
		break;
	}
}

void CUQRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_b48r:
		ConvertUQRGToB48r(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		break;
	case UTVF_b64a:
		ConvertUQRGToB64a(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		break;
	case UTVF_r210:
		ConvertUQRGToR210(pRawBegin[0], pRawEnd[0], g, b, r, m_nWidth, m_fmRaw.scbLineStride[0]);
		break;
	}
}

bool CUQRGCodec::PredictDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertB64aToUQRG_PredictCylindricalLeftAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		return true;
	case UTVF_r210:
		ConvertR210ToUQRG_PredictCylindricalLeftAndCount(g, b, r, pRawBegin[0], pRawEnd[0], m_nWidth, m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		return true;
	}

	return false;
}

bool CUQRGCodec::RestoreDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertUQRGToB64a_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		return true;
	case UTVF_r210:
		ConvertUQRGToR210_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, m_nWidth, m_fmRaw.scbLineStride[0]);
		return true;
	}

	return false;
}

bool CUQRGCodec::IsDirectRestorable()
{
	switch (m_utvfRaw)
	{
	case UTVF_b64a:
	case UTVF_r210:
		return true;
	}

	return false;
}
