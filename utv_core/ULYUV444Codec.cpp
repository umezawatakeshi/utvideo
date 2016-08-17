/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULYUV444Codec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "ColorOrder.h"

template<>
const utvf_t CULYUV444Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CULYUV444Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CULYUV444Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CULYUV444Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<class C>
const utvf_t CULYUV444Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CULYUV444Codec<CBT601Coefficient>::CULYUV444Codec(const char *pszInterfaceName) : CUL00Codec("ULY4", pszInterfaceName)
{
}

template<>
CULYUV444Codec<CBT709Coefficient>::CULYUV444Codec(const char *pszInterfaceName) : CUL00Codec("ULH4", pszInterfaceName)
{
}

template<>
const char *CULYUV444Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV444 BT.601";
}

template<>
const char *CULYUV444Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV444 BT.709";
}

template<class C>
void CULYUV444Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
}

template<class C, class T>
void ConvertRGBToULY4(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	uint8_t *y = pDstYBegin;
	uint8_t *u = pDstUBegin;
	uint8_t *v = pDstVBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*y = min(max(int((*(p + T::B))*C::B2Y + (*(p + T::G))*C::G2Y + (*(p + T::R))*C::R2Y + 16.5), 16), 235);
			*u = min(max(int((*(p + T::B))*C::B2U + (*(p + T::G))*C::G2U + (*(p + T::R))*C::R2U + 128.5), 16), 240);
			*v = min(max(int((*(p + T::B))*C::B2V + (*(p + T::G))*C::G2V + (*(p + T::R))*C::R2V + 128.5), 16), 240);
			y++; u++; v++;
		}
	}
}

template<class C>
void CULYUV444Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *pDstYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	uint8_t *pDstUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	uint8_t *pDstVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV24:
		{
			const uint8_t *pSrcYBegin, *pSrcVBegin, *pSrcUBegin;

			pSrcYBegin = ((uint8_t *)m_pInput);
			pSrcVBegin = pSrcYBegin + m_nWidth * m_nHeight;
			pSrcUBegin = pSrcVBegin + m_nWidth * m_nHeight;

			pSrcYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pSrcVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
			pSrcUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[2]);
		}
		return;
	}

	const uint8_t *pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	const uint8_t *pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertRGBToULY4<C, CBGRColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertRGBToULY4<C, CBGRAColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertRGBToULY4<C, CBGRColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertRGBToULY4<C, CBGRAColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY4<C, CRGBColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertRGBToULY4<C, CARGBColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	}
}

template<class C, class T>
void ConvertULY4ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	const uint8_t *y = pSrcYBegin;
	const uint8_t *u = pSrcUBegin;
	const uint8_t *v = pSrcVBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			uint8_t *q = p + T::BYPP;
			*(p + T::G) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2G + (*v - 128)*C::V2G), 0), 255);
			*(p + T::B) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2B), 0), 255);
			*(p + T::R) = min(max(int((*y - 16)*C::Y2RGB + (*v - 128)*C::V2R), 0), 255);
			if (T::BYPP == 4)
				*(p + T::A) = 0xff;
			y++; u++; v++;
		}
	}
}

template<class C>
void CULYUV444Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *pSrcYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	const uint8_t *pSrcUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	const uint8_t *pSrcVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV24:
		{
			uint8_t *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((uint8_t *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight;

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[2]);
		}
		return;
	}

	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertULY4ToRGB<C, CBGRColorOrder>(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULY4ToRGB<C, CBGRAColorOrder>(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULY4ToRGB<C, CBGRColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULY4ToRGB<C, CBGRAColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY4ToRGB<C, CRGBColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY4ToRGB<C, CARGBColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	}
}

template<class C>
bool CULYUV444Codec<C>::DecodeDirect(uint32_t nBandIndex)
{
	if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) != FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
		return false;

	switch (m_utvfRaw)
	{
	case UTVF_YV24:
		{
			uint8_t *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((uint8_t *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight;

			uint8_t *pDstYEnd = pDstYBegin + m_dwPlaneStripeEnd[nBandIndex] * m_cbPlaneStripeSize[0];
			uint8_t *pDstUEnd = pDstUBegin + m_dwPlaneStripeEnd[nBandIndex] * m_cbPlaneStripeSize[1];
			uint8_t *pDstVEnd = pDstVBegin + m_dwPlaneStripeEnd[nBandIndex] * m_cbPlaneStripeSize[2];

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

			HuffmanDecodeAndAccum(pDstYBegin, pDstYEnd, m_pDecodeCode[0][nBandIndex], &m_hdt[0], pDstYEnd - pDstYBegin, pDstYEnd - pDstYBegin);
			HuffmanDecodeAndAccum(pDstUBegin, pDstUEnd, m_pDecodeCode[1][nBandIndex], &m_hdt[1], pDstUEnd - pDstUBegin, pDstUEnd - pDstUBegin);
			HuffmanDecodeAndAccum(pDstVBegin, pDstVEnd, m_pDecodeCode[2][nBandIndex], &m_hdt[2], pDstVEnd - pDstVBegin, pDstVEnd - pDstVBegin);
		}
		return true;
	}

	return false;
}
