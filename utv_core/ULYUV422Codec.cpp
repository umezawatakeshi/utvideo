/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULYUV422Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

template<>
const utvf_t CULYUV422Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
#ifndef __APPLE__
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
#endif
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CULYUV422Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
#ifndef __APPLE__
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
#endif
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CULYUV422Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
#ifndef __APPLE__
	UTVF_HDYC,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
#endif
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CULYUV422Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
#ifndef __APPLE__
	UTVF_HDYC,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
#endif
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<class C>
const utvf_t CULYUV422Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CULYUV422Codec<CBT601Coefficient>::CULYUV422Codec(const char *pszInterfaceName) : CUL00Codec("ULY2", pszInterfaceName)
{
}

template<>
CULYUV422Codec<CBT709Coefficient>::CULYUV422Codec(const char *pszInterfaceName) : CUL00Codec("ULH2", pszInterfaceName)
{
}

template<>
const char *CULYUV422Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.601";
}

template<>
const char *CULYUV422Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.709";
}

template<class C>
void CULYUV422Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height / 2;
	m_cbPlaneSize[2]          = width * height / 2;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width / 2;
	m_cbPlaneWidth[2]         = width / 2;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width / 2;
	m_cbPlaneStripeSize[2]    = width / 2;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width / 2;
	m_cbPlanePredictStride[2] = width / 2;
}

template<class C>
void CULYUV422Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *y, *u, *v;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	y = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertYUYVToULY2(y, u, v, pSrcBegin, pSrcEnd);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_HDYC:
		ConvertUYVYToULY2(y, u, v, pSrcBegin, pSrcEnd);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertBGRToULY2(C)(y, u, v, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRXToULY2(C)(y, u, v, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULY2(C)(y, u, v, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULY2(C)(y, u, v, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY2(C)(y, u, v, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULY2(C)(y, u, v, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

template<class C>
void CULYUV422Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *y, *u, *v;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	y = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertULY2ToYUYV(pDstBegin, pDstEnd, y, u, v);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_HDYC:
		ConvertULY2ToUYVY(pDstBegin, pDstEnd, y, u, v);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertULY2ToBGR(C)(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, y, u, v, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULY2ToBGRX(C)(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, y, u, v, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULY2ToBGR(C)(pDstBegin, pDstEnd, y, u, v, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULY2ToBGRX(C)(pDstBegin, pDstEnd, y, u, v, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY2ToRGB(C)(pDstBegin, pDstEnd, y, u, v, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY2ToXRGB(C)(pDstBegin, pDstEnd, y, u, v, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

template<class C>
bool CULYUV422Codec<C>::DecodeDirect(uint32_t nBandIndex)
{
	if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) != FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
		return false;

	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		HuffmanDecodeAndAccumStep2(pDstBegin+0, pDstEnd+0, m_pDecodeCode[0][nBandIndex], &m_hdt[0], pDstEnd - pDstBegin, pDstEnd - pDstBegin);
		HuffmanDecodeAndAccumStep4(pDstBegin+1, pDstEnd+1, m_pDecodeCode[1][nBandIndex], &m_hdt[1], pDstEnd - pDstBegin, pDstEnd - pDstBegin);
		HuffmanDecodeAndAccumStep4(pDstBegin+3, pDstEnd+3, m_pDecodeCode[2][nBandIndex], &m_hdt[2], pDstEnd - pDstBegin, pDstEnd - pDstBegin);
		return true;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_HDYC:
		HuffmanDecodeAndAccumStep2(pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], pDstEnd - pDstBegin, pDstEnd - pDstBegin);
		HuffmanDecodeAndAccumStep4(pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], pDstEnd - pDstBegin, pDstEnd - pDstBegin);
		HuffmanDecodeAndAccumStep4(pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], pDstEnd - pDstBegin, pDstEnd - pDstBegin);
		return true;
	}

	return false;
}
