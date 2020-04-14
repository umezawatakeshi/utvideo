/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQY0Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQY0Codec::m_utvfEncoderInput[] = {
	UTVF_P010,
	UTVF_P016,
	UTVF_YUV420P16LE,
	UTVF_INVALID,
};

const utvf_t CUQY0Codec::m_utvfDecoderOutput[] = {
	UTVF_P010,
	UTVF_P016,
	UTVF_YUV420P16LE,
	UTVF_INVALID,
};

const utvf_t CUQY0Codec::m_utvfCompressed[] = {
	UTVF_UQY0,
	UTVF_INVALID,
};

CUQY0Codec::CUQY0Codec(const char *pszInterfaceName) : CUQ00Codec("UQY0", pszInterfaceName)
{
}

void CUQY0Codec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height * 2;
	m_cbPlaneSize[1]          = width * height / 2;
	m_cbPlaneSize[2]          = width * height / 2;

	m_cbPlaneWidth[0]         = width * 2;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;

	m_cbPlaneStripeSize[0]    = width * 4;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;

	m_cbPlanePredictStride[0] = width * 2;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
}

void CUQY0Codec::ConvertToPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
		ConvertLittleEndian16ToHostEndian10Limited(y, pRawBegin[0], pRawEnd[0]);
		ConvertLittleEndian16ToHostEndian10Limited(u, pRawBegin[1], pRawEnd[1]);
		ConvertLittleEndian16ToHostEndian10Limited(v, pRawBegin[2], pRawEnd[2]);
		return;

	case UTVF_P010:
		ConvertLittleEndian16ToHostEndian10Noround(y, pRawBegin[0], pRawEnd[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround(u, v, pRawBegin[1], pRawEnd[1]);
		return;

	case UTVF_P016:
		ConvertLittleEndian16ToHostEndian10Limited(y, pRawBegin[0], pRawEnd[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited(u, v, pRawBegin[1], pRawEnd[1]);
		return;
	}
}

void CUQY0Codec::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[0], pRawEnd[0], y);
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[1], pRawEnd[1], u);
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[2], pRawEnd[2], v);
		return;

	case UTVF_P010:
	case UTVF_P016:
		ConvertHostEndian10ToLittleEndian16Limited(pRawBegin[0], pRawEnd[0], y);
		ConvertPlanarHostEndian10ToPackedUVLittleEndian16Limited(pRawBegin[1], pRawEnd[1], u, v);
		return;
	}
}

bool CUQY0Codec::PredictDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(y, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(u, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1]);
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(v, pRawBegin[2], pRawEnd[2], m_fmRaw.cbLineWidth[2], m_fmRaw.scbLineStride[2], m_counts[nBandIndex].dwCount[2]);
		return true;

	case UTVF_P010:
		ConvertLittleEndian16ToHostEndian10Noround_PredictCylindricalLeftAndCount(y, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictCylindricalLeftAndCount(u, v, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		return true;

	case UTVF_P016:
		ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(y, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
		ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictCylindricalLeftAndCount(u, v, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		return true;
	}

	return false;
}

void CUQY0Codec::GenerateDecodeTable(uint32_t nPlaneIndex)
{
	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
	case UTVF_P010:
	case UTVF_P016:
		GenerateHuffmanDecodeTable<10, 6>(m_hdt[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		break;

	default:
		CUQ00Codec::GenerateDecodeTable(nPlaneIndex);
	}
}

bool CUQY0Codec::DecodeDirect(uint32_t nBandIndex)
{
	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
	case UTVF_P010:
	case UTVF_P016:
		DecodeAndRestoreCustomToPlanar(nBandIndex);
		return true;
	}

	return false;
}

void CUQY0Codec::RestoreCustom(uint32_t nBandIndex, int nPlaneIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
		ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(pRawBegin[nPlaneIndex], pRawEnd[nPlaneIndex], pPlaneBegin[nPlaneIndex], m_fmRaw.cbLineWidth[nPlaneIndex], m_fmRaw.scbLineStride[nPlaneIndex]);
		break;

	case UTVF_P010:
	case UTVF_P016:
		if (nPlaneIndex == 0)
			ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], y, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
		else if (nPlaneIndex == 2)
			ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft(pRawBegin[1], pRawEnd[1], u, v, m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1]);
		break;
	}
}
