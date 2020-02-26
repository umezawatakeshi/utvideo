/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQY0Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQY0Codec::m_utvfEncoderInput[] = {
	UTVF_YUV420P16LE,
	UTVF_INVALID,
};

const utvf_t CUQY0Codec::m_utvfDecoderOutput[] = {
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
	uint8_t *y, *u, *v;

	y = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
		{
			const uint8_t *pSrcYBegin, *pSrcUBegin, *pSrcVBegin;
			const uint8_t *pSrcYEnd, *pSrcUEnd, *pSrcVEnd;

			pSrcYBegin = ((const uint8_t*)m_pInput);
			pSrcUBegin = pSrcYBegin + m_nWidth * m_nHeight * 2;
			pSrcVBegin = pSrcUBegin + m_nWidth * m_nHeight / 2;

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
	}
}

void CUQY0Codec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *y, *u, *v;

	y = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUV420P16LE:
		{
			uint8_t *pDstYBegin, *pDstUBegin, *pDstVBegin;
			uint8_t *pDstYEnd, *pDstUEnd, *pDstVEnd;

			pDstYBegin = ((uint8_t*)m_pOutput);
			pDstUBegin = pDstYBegin + m_nWidth * m_nHeight * 2;
			pDstVBegin = pDstUBegin + m_nWidth * m_nHeight / 2;

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
	}
}
