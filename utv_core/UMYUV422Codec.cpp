/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UMYUV422Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

template<>
const utvf_t CUMYUV422Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV422Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV422Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
	UTVF_HDYC,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV422Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_HDYC,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<class C>
const utvf_t CUMYUV422Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CUMYUV422Codec<CBT601Coefficient>::CUMYUV422Codec(const char *pszInterfaceName) : CUM00Codec("UMY2", pszInterfaceName)
{
}

template<>
CUMYUV422Codec<CBT709Coefficient>::CUMYUV422Codec(const char *pszInterfaceName) : CUM00Codec("UMH2", pszInterfaceName)
{
}

template<>
const char *CUMYUV422Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.601";
}

template<>
const char *CUMYUV422Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.709";
}

template<class C>
void CUMYUV422Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	auto cbYWidth = ROUNDUP(width, 64);
	auto cbCWidth = ROUNDUP(width / 2, 64);

	m_cbPlaneSize[0]          = cbYWidth * height;
	m_cbPlaneSize[1]          = cbCWidth * height;
	m_cbPlaneSize[2]          = cbCWidth * height;

	m_cbPlaneWidth[0]         = cbYWidth;
	m_cbPlaneWidth[1]         = cbCWidth;
	m_cbPlaneWidth[2]         = cbCWidth;

	m_cbPlaneStripeSize[0]    = cbYWidth;
	m_cbPlaneStripeSize[1]    = cbCWidth;
	m_cbPlaneStripeSize[2]    = cbCWidth;

	m_cbPlanePredictStride[0] = cbYWidth;
	m_cbPlanePredictStride[1] = cbCWidth;
	m_cbPlanePredictStride[2] = cbCWidth;
}

template<class C>
void CUMYUV422Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *pDstYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	uint8_t *pDstUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	uint8_t *pDstVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV16:
		{
			const uint8_t *pSrcYBegin, *pSrcVBegin, *pSrcUBegin;
			const uint8_t *pSrcYEnd, *pSrcVEnd, *pSrcUEnd;

			pSrcYBegin = ((uint8_t *)m_pInput);
			pSrcVBegin = pSrcYBegin + m_nWidth * m_nHeight;
			pSrcUBegin = pSrcVBegin + m_nWidth * m_nHeight / 2;

			pSrcYEnd = pSrcYBegin + m_dwRawStripeEnd[nBandIndex] * m_nWidth;
			pSrcVEnd = pSrcVBegin + m_dwRawStripeEnd[nBandIndex] * m_nWidth / 2;
			pSrcUEnd = pSrcUBegin + m_dwRawStripeEnd[nBandIndex] * m_nWidth / 2;

			pSrcYBegin += m_dwRawStripeBegin[nBandIndex] * m_nWidth;
			pSrcVBegin += m_dwRawStripeBegin[nBandIndex] * m_nWidth / 2;
			pSrcUBegin += m_dwRawStripeBegin[nBandIndex] * m_nWidth / 2;

			for (auto p = pSrcYBegin; p != pSrcYEnd; p += m_nWidth, pDstYBegin += m_cbPlaneWidth[0])
			{
				memcpy(pDstYBegin, p, m_nWidth);
				std::fill(pDstYBegin + m_nWidth, pDstYBegin + m_cbPlaneWidth[0], pDstYBegin[m_nWidth - 1]);
			}
			for (auto p = pSrcUBegin; p != pSrcUEnd; p += m_nWidth / 2, pDstUBegin += m_cbPlaneWidth[1])
			{
				memcpy(pDstUBegin, p, m_nWidth / 2);
				std::fill(pDstUBegin + m_nWidth / 2, pDstUBegin + m_cbPlaneWidth[1], pDstUBegin[m_nWidth / 2 - 1]);
			}
			for (auto p = pSrcVBegin; p != pSrcVEnd; p += m_nWidth / 2, pDstVBegin += m_cbPlaneWidth[2])
			{
				memcpy(pDstVBegin, p, m_nWidth / 2);
				std::fill(pDstVBegin + m_nWidth / 2, pDstVBegin + m_cbPlaneWidth[2], pDstVBegin[m_nWidth / 2 - 1]);
			}
		}
		return;
	}

	const uint8_t *pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	const uint8_t *pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertYUYVToULY2(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertUYVYToULY2(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertBGRToULY2(C)(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRXToULY2(C)(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULY2(C)(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULY2(C)(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY2(C)(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULY2(C)(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	}
}

template<class C>
void CUMYUV422Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *pSrcYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	const uint8_t *pSrcUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	const uint8_t *pSrcVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV16:
		{
			uint8_t *pDstYBegin, *pDstVBegin, *pDstUBegin;
			uint8_t *pDstYEnd, *pDstVEnd, *pDstUEnd;

			pDstYBegin = ((uint8_t *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight / 2;

			pDstYEnd = pDstYBegin + m_dwRawStripeEnd[nBandIndex] * m_nWidth;
			pDstVEnd = pDstVBegin + m_dwRawStripeEnd[nBandIndex] * m_nWidth / 2;
			pDstUEnd = pDstUBegin + m_dwRawStripeEnd[nBandIndex] * m_nWidth / 2;

			pDstYBegin += m_dwRawStripeBegin[nBandIndex] * m_nWidth;
			pDstVBegin += m_dwRawStripeBegin[nBandIndex] * m_nWidth / 2;
			pDstUBegin += m_dwRawStripeBegin[nBandIndex] * m_nWidth / 2;

			for (auto p = pDstYBegin; p != pDstYEnd; p += m_nWidth, pSrcYBegin += m_cbPlaneWidth[0])
				memcpy(p, pSrcYBegin, m_nWidth);
			for (auto p = pDstUBegin; p != pDstUEnd; p += m_nWidth / 2, pSrcUBegin += m_cbPlaneWidth[1])
				memcpy(p, pSrcUBegin, m_nWidth / 2);
			for (auto p = pDstVBegin; p != pDstVEnd; p += m_nWidth / 2, pSrcVBegin += m_cbPlaneWidth[2])
				memcpy(p, pSrcVBegin, m_nWidth / 2);
		}
		return;
	}

	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertULY2ToYUYV(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertULY2ToUYVY(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertULY2ToBGR(C)(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULY2ToBGRX(C)(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULY2ToBGR(C)(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULY2ToBGRX(C)(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY2ToRGB(C)(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY2ToXRGB(C)(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	}
}

template<class C>
bool CUMYUV422Codec<C>::PredictDirect(uint32_t nBandIndex)
{
	if (m_nKeyFrameInterval <= 1)
	{
		if (m_utvfRaw == UTVF_YV16 && (m_nWidth % 128) == 0)
		{
			const uint8_t *pSrcBegin[3];

			pSrcBegin[0] = ((const uint8_t *)m_pInput);
			pSrcBegin[2] = pSrcBegin[0] + m_nWidth * m_nHeight;
			pSrcBegin[1] = pSrcBegin[2] + m_nWidth * m_nHeight / 2;

			PredictFromPlanar(nBandIndex, pSrcBegin);

			return true;
		}
	}

	return false;
}

template<class C>
bool CUMYUV422Codec<C>::DecodeDirect(uint32_t nBandIndex)
{
	if (!(m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION))
	{
		if (m_utvfRaw == UTVF_YV16 && (m_nWidth % 128) == 0)
		{
			uint8_t *pDstBegin[3];

			pDstBegin[0] = ((uint8_t *)m_pOutput);
			pDstBegin[2] = pDstBegin[0] + m_nWidth * m_nHeight;
			pDstBegin[1] = pDstBegin[2] + m_nWidth * m_nHeight / 2;

			DecodeToPlanar(nBandIndex, pDstBegin);

			return true;
		}
	}

	return false;
}
