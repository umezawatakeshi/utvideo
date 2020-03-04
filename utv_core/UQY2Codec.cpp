/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQY2Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQY2Codec::m_utvfEncoderInput[] = {
	UTVF_v210,
	UTVF_P210,
	UTVF_P216,
	UTVF_YUV422P16LE,
	UTVF_INVALID,
};

const utvf_t CUQY2Codec::m_utvfDecoderOutput[] = {
	UTVF_v210,
	UTVF_P210,
	UTVF_P216,
	UTVF_YUV422P16LE,
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
	uint8_t *y, *u, *v;

	y = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
		{
			const uint8_t *pSrcYBegin, *pSrcUBegin, *pSrcVBegin;
			const uint8_t *pSrcYEnd, *pSrcUEnd, *pSrcVEnd;

			pSrcYBegin = ((const uint8_t*)m_pInput);
			pSrcUBegin = pSrcYBegin + m_nWidth * m_nHeight * 2;
			pSrcVBegin = pSrcUBegin + m_nWidth * m_nHeight;

			pSrcYEnd = pSrcYBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUEnd = pSrcUBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[1];
			pSrcVEnd = pSrcVBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[2];

			pSrcYBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
			pSrcVBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

			ConvertLittleEndian16ToHostEndian10Limited(y, pSrcYBegin, pSrcYEnd);
			ConvertLittleEndian16ToHostEndian10Limited(u, pSrcUBegin, pSrcUEnd);
			ConvertLittleEndian16ToHostEndian10Limited(v, pSrcVBegin, pSrcVEnd);
		}
		return;

	case UTVF_P210:
		{
			const uint8_t *pSrcYBegin, *pSrcUVBegin;
			const uint8_t *pSrcYEnd, *pSrcUVEnd;

			pSrcYBegin = ((const uint8_t*)m_pInput);
			pSrcUVBegin = pSrcYBegin + m_nWidth * m_nHeight * 2;

			pSrcYEnd  = pSrcYBegin  + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVEnd = pSrcUVBegin + m_dwStripeEnd[nBandIndex] * m_nWidth * 2;

			pSrcYBegin  += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVBegin += m_dwStripeBegin[nBandIndex] * m_nWidth * 2;

			ConvertLittleEndian16ToHostEndian10Noround(y, pSrcYBegin, pSrcYEnd);
			ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround(u, v, pSrcUVBegin, pSrcUVEnd);
		}
		return;

	case UTVF_P216:
		{
			const uint8_t *pSrcYBegin, *pSrcUVBegin;
			const uint8_t *pSrcYEnd, *pSrcUVEnd;

			pSrcYBegin = ((const uint8_t*)m_pInput);
			pSrcUVBegin = pSrcYBegin + m_nWidth * m_nHeight * 2;

			pSrcYEnd  = pSrcYBegin  + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVEnd = pSrcUVBegin + m_dwStripeEnd[nBandIndex] * m_nWidth * 2;

			pSrcYBegin  += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVBegin += m_dwStripeBegin[nBandIndex] * m_nWidth * 2;

			ConvertLittleEndian16ToHostEndian10Limited(y, pSrcYBegin, pSrcYEnd);
			ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited(u, v, pSrcUVBegin, pSrcUVEnd);
		}
		return;
	}

	const uint8_t* pSrcBegin, * pSrcEnd;

	pSrcBegin = ((uint8_t*)m_pInput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t*)m_pInput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_v210:
		ConvertV210ToUQY2(y, u, v, pSrcBegin, pSrcEnd, m_nWidth, m_cbRawGrossWidth);
		break;
	}
}

void CUQY2Codec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *y, *u, *v;

	y = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
		{
			uint8_t *pDstYBegin, *pDstUBegin, *pDstVBegin;
			uint8_t *pDstYEnd, *pDstUEnd, *pDstVEnd;

			pDstYBegin = ((uint8_t*)m_pOutput);
			pDstUBegin = pDstYBegin + m_nWidth * m_nHeight * 2;
			pDstVBegin = pDstUBegin + m_nWidth * m_nHeight;

			pDstYEnd = pDstYBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pDstUEnd = pDstUBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[1];
			pDstVEnd = pDstVBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[2];

			pDstYBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pDstUBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
			pDstVBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

			ConvertHostEndian10ToLittleEndian16Limited(pDstYBegin, pDstYEnd, y);
			ConvertHostEndian10ToLittleEndian16Limited(pDstUBegin, pDstUEnd, u);
			ConvertHostEndian10ToLittleEndian16Limited(pDstVBegin, pDstVEnd, v);
		}
		return;

	case UTVF_P210:
	case UTVF_P216:
		{
			uint8_t *pDstYBegin, *pDstUVBegin;
			uint8_t *pDstYEnd, *pDstUVEnd;

			pDstYBegin  = ((uint8_t*)m_pOutput);
			pDstUVBegin = pDstYBegin + m_nWidth * m_nHeight * 2;

			pDstYEnd  = pDstYBegin  + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pDstUVEnd = pDstUVBegin + m_dwStripeEnd[nBandIndex] * m_nWidth * 2;

			pDstYBegin  += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pDstUVBegin += m_dwStripeBegin[nBandIndex] * m_nWidth * 2;

			ConvertHostEndian10ToLittleEndian16Limited(pDstYBegin, pDstYEnd, y);
			ConvertPlanarHostEndian10ToPackedUVLittleEndian16Limited(pDstUVBegin, pDstUVEnd, u, v);
		}
		return;
	}

	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_v210:
		ConvertUQY2ToV210(pDstBegin, pDstEnd, y, u, v, m_nWidth, m_cbRawGrossWidth);
		break;
	}
}

bool CUQY2Codec::PredictDirect(uint32_t nBandIndex)
{
	uint8_t* y, * u, * v;

	y = m_pPredicted->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	u = m_pPredicted->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	v = m_pPredicted->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUV422P16LE:
		{
			const uint8_t *pSrcYBegin, *pSrcUBegin, *pSrcVBegin;
			const uint8_t *pSrcYEnd, *pSrcUEnd, *pSrcVEnd;

			pSrcYBegin = ((const uint8_t*)m_pInput);
			pSrcUBegin = pSrcYBegin + m_nWidth * m_nHeight * 2;
			pSrcVBegin = pSrcUBegin + m_nWidth * m_nHeight;

			pSrcYEnd = pSrcYBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUEnd = pSrcUBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[1];
			pSrcVEnd = pSrcVBegin + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[2];

			pSrcYBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
			pSrcVBegin += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

			ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(y, pSrcYBegin, pSrcYEnd, m_cbPlanePredictStride[0], m_cbPlanePredictStride[0], m_counts[nBandIndex].dwCount[0]);
			ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(u, pSrcUBegin, pSrcUEnd, m_cbPlanePredictStride[1], m_cbPlanePredictStride[1], m_counts[nBandIndex].dwCount[1]);
			ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(v, pSrcVBegin, pSrcVEnd, m_cbPlanePredictStride[2], m_cbPlanePredictStride[2], m_counts[nBandIndex].dwCount[2]);
		}
		return true;

	case UTVF_P210:
		{
			const uint8_t *pSrcYBegin, *pSrcUVBegin;
			const uint8_t *pSrcYEnd, *pSrcUVEnd;

			pSrcYBegin = ((const uint8_t*)m_pInput);
			pSrcUVBegin = pSrcYBegin + m_nWidth * m_nHeight * 2;

			pSrcYEnd  = pSrcYBegin  + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVEnd = pSrcUVBegin + m_dwStripeEnd[nBandIndex] * m_nWidth * 2;

			pSrcYBegin  += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVBegin += m_dwStripeBegin[nBandIndex] * m_nWidth * 2;

			ConvertLittleEndian16ToHostEndian10Noround_PredictCylindricalLeftAndCount(y, pSrcYBegin, pSrcYEnd, m_cbPlanePredictStride[0], m_cbPlanePredictStride[0], m_counts[nBandIndex].dwCount[0]);
			ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictCylindricalLeftAndCount(u, v, pSrcUVBegin, pSrcUVEnd, m_nWidth * 2, m_nWidth * 2, m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		}
		return true;

	case UTVF_P216:
		{
			const uint8_t *pSrcYBegin, *pSrcUVBegin;
			const uint8_t *pSrcYEnd, *pSrcUVEnd;

			pSrcYBegin = ((const uint8_t*)m_pInput);
			pSrcUVBegin = pSrcYBegin + m_nWidth * m_nHeight * 2;

			pSrcYEnd  = pSrcYBegin  + m_dwStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVEnd = pSrcUVBegin + m_dwStripeEnd[nBandIndex] * m_nWidth * 2;

			pSrcYBegin  += m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcUVBegin += m_dwStripeBegin[nBandIndex] * m_nWidth * 2;

			ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount(y, pSrcYBegin, pSrcYEnd, m_cbPlanePredictStride[0], m_cbPlanePredictStride[0], m_counts[nBandIndex].dwCount[0]);
			ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictCylindricalLeftAndCount(u, v, pSrcUVBegin, pSrcUVEnd, m_nWidth * 2, m_nWidth * 2, m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
		}
		return true;
	}

	return false;
}

void CUQY2Codec::GenerateDecodeTable(uint32_t nPlaneIndex)
{
	if (m_utvfRaw == UTVF_YUV422P16LE)
	{
		GenerateHuffmanDecodeTable<10, 6>(&m_hdt[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
	}
	else
	{
		CUQ00Codec::GenerateDecodeTable(nPlaneIndex);
	}
}

bool CUQY2Codec::DecodeDirect(uint32_t nBandIndex)
{
	if (m_utvfRaw == UTVF_YUV422P16LE)
	{
		uint8_t* pDstBegin[3];

		pDstBegin[0] = ((uint8_t*)m_pOutput);
		pDstBegin[1] = pDstBegin[0] + m_nWidth * m_nHeight * 2;
		pDstBegin[2] = pDstBegin[1] + m_nWidth * m_nHeight;

		DecodeAndRestoreCustomToPlanar(nBandIndex, pDstBegin);

		return true;
	}

	return false;
}

void CUQY2Codec::RestoreCustom(uint32_t nBandIndex, int nPlaneIndex, uint8_t* const* pDstBegin, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd)
{
	if (m_utvfRaw == UTVF_YUV422P16LE)
	{
		uint8_t *pCurDstBegin;
		uint8_t *pCurDstEnd;

		pCurDstBegin = pDstBegin[nPlaneIndex] + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		pCurDstEnd   = pDstBegin[nPlaneIndex] + m_dwStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(pCurDstBegin, pCurDstEnd, (const uint8_t*)pSrcBegin, m_cbPlanePredictStride[nPlaneIndex], m_cbPlanePredictStride[nPlaneIndex]);
	}
}
