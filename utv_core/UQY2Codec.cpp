/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQY2Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQY2Codec::m_utvfEncoderInput[] = {
	UTVF_P210,
	UTVF_P216,
	UTVF_YUV422P16LE,
	UTVF_v210,
	UTVF_INVALID,
};

const utvf_t CUQY2Codec::m_utvfDecoderOutput[] = {
	UTVF_P210,
	UTVF_P216,
	UTVF_YUV422P16LE,
	UTVF_v210,
	UTVF_INVALID,
};

const utvf_t CUQY2Codec::m_utvfCompressed[] = {
	UTVF_UQY2,
	UTVF_INVALID,
};

CUQY2Codec::CUQY2Codec(const char *pszInterfaceName) : CUQ00Codec("UQY2", pszInterfaceName)
{
}

void CUQY2Codec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height * 2;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;

	m_cbPlaneWidth[0]         = width * 2;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;

	m_cbPlaneStripeSize[0]    = width * 2;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;

	m_cbPlanePredictStride[0] = width * 2;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
}

void CUQY2Codec::ConvertToPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
		ConvertLittleEndian16ToHostEndian10Limited(y, pRawBegin[0], pRawEnd[0]);
		ConvertLittleEndian16ToHostEndian10Limited(u, pRawBegin[1], pRawEnd[1]);
		ConvertLittleEndian16ToHostEndian10Limited(v, pRawBegin[2], pRawEnd[2]);
		return;

	case UTVF_P210:
		ConvertLittleEndian16ToHostEndian10Noround(y, pRawBegin[0], pRawEnd[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround(u, v, pRawBegin[1], pRawEnd[1]);
		return;

	case UTVF_P216:
		ConvertLittleEndian16ToHostEndian10Limited(y, pRawBegin[0], pRawEnd[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited(u, v, pRawBegin[1], pRawEnd[1]);
		return;

	case UTVF_v210:
		ConvertV210ToUQY2(y, u, v, pRawBegin[0], pRawEnd[0], m_nWidth, m_fmRaw.scbLineStride[0]);
		break;
	}
}

void CUQY2Codec::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[0], pRawEnd[0], y);
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[1], pRawEnd[1], u);
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[2], pRawEnd[2], v);
		return;

	case UTVF_P210:
	case UTVF_P216:
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[0], pRawEnd[0], y);
		ConvertPlanarHostEndian10ToPackedUVLittleEndian16Limited(pRawBegin[1], pRawEnd[1], u, v);
		return;

	case UTVF_v210:
		ConvertUQY2ToV210(pRawBegin[0], pRawEnd[0], y, u, v, m_nWidth, m_fmRaw.scbLineStride[0]);
		break;
	}
}

bool CUQY2Codec::PredictDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(y, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(u, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1]);
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(v, pRawBegin[2], pRawEnd[2], m_fmRaw.cbLineWidth[2], m_fmRaw.scbLineStride[2], m_counts[nBandIndex].dwCount[2]);
		return true;

	case UTVF_P210:
		ConvertLittleEndian16ToHostEndian10Noround_PredictCylindricalLeftAndCount(y, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictCylindricalLeftAndCount(u, v, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		return true;

	case UTVF_P216:
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(y, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictCylindricalLeftAndCount(u, v, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		return true;
	}

	return false;
}

void CUQY2Codec::GenerateDecodeTable(uint32_t nPlaneIndex)
{
	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
	case UTVF_P210:
	case UTVF_P216:
		GenerateHuffmanDecodeTable<10, 6>(m_hdt[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		break;

	default:
		CUQ00Codec::GenerateDecodeTable(nPlaneIndex);
	}
}

bool CUQY2Codec::DecodeDirect(uint32_t nBandIndex)
{
	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
	case UTVF_P210:
	case UTVF_P216:
		DecodeAndRestoreCustomToPlanar(nBandIndex);
		return true;
	}

	return false;
}

void CUQY2Codec::RestoreCustom(uint32_t nBandIndex, int nPlaneIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
		ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(pRawBegin[nPlaneIndex], pRawEnd[nPlaneIndex], pPlaneBegin[nPlaneIndex], m_fmRaw.cbLineWidth[nPlaneIndex], m_fmRaw.scbLineStride[nPlaneIndex]);
		break;

	case UTVF_P210:
	case UTVF_P216:
		if (nPlaneIndex == 0)
			ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], y, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		else if (nPlaneIndex == 2)
			ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft(pRawBegin[1], pRawEnd[1], u, v, m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1]);
		break;
	}
}
