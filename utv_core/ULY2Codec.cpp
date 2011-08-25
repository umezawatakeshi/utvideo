/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULY2Codec.h"
#include "Predict.h"
#include "Convert.h"

const utvf_t CULY2Codec::m_utvfEncoderInput[] = {
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
	UTVF_RGB24_WIN,
	UTVF_RGB32_WIN,
	UTVF_INVALID,
};

const utvf_t CULY2Codec::m_utvfDecoderOutput[] = {
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
	UTVF_RGB24_WIN,
	UTVF_RGB32_WIN,
	UTVF_INVALID,
};

const utvf_t CULY2Codec::m_utvfCompressed[] = {
	UTVF_ULY2,
	UTVF_INVALID,
};

CULY2Codec::CULY2Codec(const char *pszInterfaceName) : CUL00Codec(pszInterfaceName)
{
}

CULY2Codec::~CULY2Codec(void)
{
}

CCodec *CULY2Codec::CreateInstance(const char *pszInterfaceName)
{
	return new CULY2Codec(pszInterfaceName);
}

void CULY2Codec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_dwPlaneSize[0]          = width * height;
	m_dwPlaneSize[1]          = width * height / 2;
	m_dwPlaneSize[2]          = width * height / 2;

	m_dwPlaneWidth[0]         = width;
	m_dwPlaneWidth[1]         = width / 2;
	m_dwPlaneWidth[2]         = width / 2;

	m_dwPlaneStripeSize[0]    = width;
	m_dwPlaneStripeSize[1]    = width / 2;
	m_dwPlaneStripeSize[2]    = width / 2;

	m_dwPlanePredictStride[0] = width;
	m_dwPlanePredictStride[1] = width / 2;
	m_dwPlanePredictStride[2] = width / 2;
}

void CULY2Codec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *y, *u, *v;
	const uint8_t *pSrcBegin, *pSrcEnd, *p;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	y = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		for (p = pSrcBegin; p < pSrcEnd; p += 4)
		{
			*y++ = *p;
			*u++ = *(p+1);
			*y++ = *(p+2);
			*v++ = *(p+3);
		}
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
		for (p = pSrcBegin; p < pSrcEnd; p += 4)
		{
			*u++ = *p;
			*y++ = *(p+1);
			*v++ = *(p+2);
			*y++ = *(p+3);
		}
		break;
	case UTVF_RGB24_WIN:
		ConvertBottomupRGB24ToULY2(y, u, v, pSrcBegin, pSrcEnd, m_dwRawGrossWidth, m_nWidth * 3);
		break;
	case UTVF_RGB32_WIN:
		ConvertBottomupRGB32ToULY2(y, u, v, pSrcBegin, pSrcEnd, m_dwRawGrossWidth, m_dwRawGrossWidth);
		break;
	}
}

void CULY2Codec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *y, *u, *v;
	uint8_t *pDstBegin, *pDstEnd, *p;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	y = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		for (p = pDstBegin; p < pDstEnd; p += 4)
		{
			*p     = *y++;
			*(p+1) = *u++;
			*(p+2) = *y++;
			*(p+3) = *v++;
		}
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
		for (p = pDstBegin; p < pDstEnd; p += 4)
		{
			*p     = *u++;
			*(p+1) = *y++;
			*(p+2) = *v++;
			*(p+3) = *y++;
		}
		break;
	case UTVF_RGB24_WIN:
		ConvertULY2ToBottomupRGB24(pDstBegin, pDstEnd, y, u, v, m_dwRawGrossWidth, m_nWidth * 3);
		break;
	case UTVF_RGB32_WIN:
		ConvertULY2ToBottomupRGB32(pDstBegin, pDstEnd, y, u, v, m_dwRawGrossWidth, m_dwRawGrossWidth);
		break;
	}
}

bool CULY2Codec::DecodeDirect(uint32_t nBandIndex)
{
	if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) != FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
		return false;

	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		HuffmanDecodeAndAccumStep2(pDstBegin+0, pDstEnd+0, m_pDecodeCode[0][nBandIndex], &m_hdt[0]);
		HuffmanDecodeAndAccumStep4(pDstBegin+1, pDstEnd+1, m_pDecodeCode[1][nBandIndex], &m_hdt[1]);
		HuffmanDecodeAndAccumStep4(pDstBegin+3, pDstEnd+3, m_pDecodeCode[2][nBandIndex], &m_hdt[2]);
		return true;
	case UTVF_UYVY:
	case UTVF_UYNV:
		HuffmanDecodeAndAccumStep2(pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0]);
		HuffmanDecodeAndAccumStep4(pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1]);
		HuffmanDecodeAndAccumStep4(pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2]);
		return true;
	}

	return false;
}
