/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQRACodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQRACodec::m_utvfEncoderInput[] = {
	UTVF_b64a,
	UTVF_INVALID,
};

const utvf_t CUQRACodec::m_utvfDecoderOutput[] = {
	UTVF_b64a,
	UTVF_INVALID,
};

const utvf_t CUQRACodec::m_utvfCompressed[] = {
	UTVF_UQRA,
	UTVF_INVALID,
};

CUQRACodec::CUQRACodec(const char *pszInterfaceName) : CUQ00Codec("UQRA", pszInterfaceName)
{
}

void CUQRACodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height * 2;
	m_cbPlaneSize[1]          = width * height * 2;
	m_cbPlaneSize[2]          = width * height * 2;
	m_cbPlaneSize[3]          = width * height * 2;

	m_cbPlaneWidth[0]         = width * 2;
	m_cbPlaneWidth[1]         = width * 2;
	m_cbPlaneWidth[2]         = width * 2;
	m_cbPlaneWidth[3]         = width * 2;

	m_cbPlaneStripeSize[0]    = width * 2;
	m_cbPlaneStripeSize[1]    = width * 2;
	m_cbPlaneStripeSize[2]    = width * 2;
	m_cbPlaneStripeSize[3]    = width * 2;

	m_cbPlanePredictStride[0] = width * 2;
	m_cbPlanePredictStride[1] = width * 2;
	m_cbPlanePredictStride[2] = width * 2;
	m_cbPlanePredictStride[3] = width * 2;
}

void CUQRACodec::ConvertToPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertB64aToUQRA(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		break;
	}
}

void CUQRACodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertUQRAToB64a(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		break;
	}
}

bool CUQRACodec::PredictDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_ec.ecFlags & EC_FLAGS_PREDICT_MASK)
	{
	case EC_FLAGS_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_b64a:
			ConvertB64aToUQRA_PredictCylindricalLeftAndCount(g, b, r, a, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
			return true;
		}
		break;
	}

	return false;
}

bool CUQRACodec::RestoreDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
	auto& [g, b, r, a] = pPlaneBegin;

	switch (m_byPredictionType)
	{
	case PREDICT_CYLINDRICAL_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_b64a:
			ConvertUQRAToB64a_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], g, b, r, a, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	}

	return false;
}

bool CUQRACodec::IsDirectRestorable()
{
	switch (m_byPredictionType)
	{
	case PREDICT_CYLINDRICAL_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_b64a:
			return true;
		}
		break;
	}

	return false;
}
