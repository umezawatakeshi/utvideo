/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULY0Codec.h"
#include "Predict.h"
#include "ColorOrder.h"

const utvf_t CULY0Codec::m_utvfEncoderInput[] = {
#ifndef __APPLE__
	UTVF_YV12,
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

const utvf_t CULY0Codec::m_utvfDecoderOutput[] = {
#ifndef __APPLE__
	UTVF_YV12,
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

const utvf_t CULY0Codec::m_utvfCompressed[] = {
	UTVF_ULY0,
	UTVF_INVALID,
};

CULY0Codec::CULY0Codec(const char *pszInterfaceName) : CUL00Codec("ULY0", pszInterfaceName)
{
}

CULY0Codec::~CULY0Codec(void)
{
}

CCodec *CULY0Codec::CreateInstance(const char *pszInterfaceName)
{
	return new CULY0Codec(pszInterfaceName);
}

void CULY0Codec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_dwPlaneSize[0]          = width * height;
	m_dwPlaneSize[1]          = width * height / 4;
	m_dwPlaneSize[2]          = width * height / 4;

	m_dwPlaneWidth[0]         = width;
	m_dwPlaneWidth[1]         = width / 2;
	m_dwPlaneWidth[2]         = width / 2;

	m_dwPlaneStripeSize[0]    = width * 2;
	m_dwPlaneStripeSize[1]    = width / 2;
	m_dwPlaneStripeSize[2]    = width / 2;

	m_dwPlanePredictStride[0] = width;
	m_dwPlanePredictStride[1] = width / 2;
	m_dwPlanePredictStride[2] = width / 2;
}

template<class T>
void ConvertRGBToULY0(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	uint8_t *y = pDstYBegin;
	uint8_t *u = pDstUBegin;
	uint8_t *v = pDstVBegin;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	for (const uint8_t *pStripeBegin = pSrcBegin; pStripeBegin != pSrcEnd; pStripeBegin += scbRawStripeSize)
	{
		for (const uint8_t *pStrideBegin = pStripeBegin; pStrideBegin != pStripeBegin + scbPredictStride; pStrideBegin += scbStride)
		{
			const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
			for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP * 2)
			{
				const uint8_t *q = p + scbPredictStride;
				*(y+0)                    = min(max(int((*(p        +T::B))*0.098 + (*(p        +T::G))*0.504 + (*(p        +T::R))*0.257 + 16.5), 16), 235);
				*(y+1)                    = min(max(int((*(p+T::BYPP+T::B))*0.098 + (*(p+T::BYPP+T::G))*0.504 + (*(p+T::BYPP+T::R))*0.257 + 16.5), 16), 235);
				*(y+dwYPlaneGrossWidth+0) = min(max(int((*(q        +T::B))*0.098 + (*(q        +T::G))*0.504 + (*(q        +T::R))*0.257 + 16.5), 16), 235);
				*(y+dwYPlaneGrossWidth+1) = min(max(int((*(q+T::BYPP+T::B))*0.098 + (*(q+T::BYPP+T::G))*0.504 + (*(q+T::BYPP+T::R))*0.257 + 16.5), 16), 235);
				*u                        = min(max(int(((*(p+T::B)+*(p+T::BYPP+T::B)+*(q+T::B)+*(q+T::BYPP+T::B))* 0.439 + (*(p+T::G)+*(p+T::BYPP+T::G)+*(q+T::G)+*(q+T::BYPP+T::G))*-0.291 + (*(p+T::R)+*(p+T::BYPP+T::R)+*(q+T::R)+*(q+T::BYPP+T::R))*-0.148)/4 + 128.5), 16), 240);
				*v                        = min(max(int(((*(p+T::B)+*(p+T::BYPP+T::B)+*(q+T::B)+*(q+T::BYPP+T::B))*-0.071 + (*(p+T::G)+*(p+T::BYPP+T::G)+*(q+T::G)+*(q+T::BYPP+T::G))*-0.368 + (*(p+T::R)+*(p+T::BYPP+T::R)+*(q+T::R)+*(q+T::BYPP+T::R))* 0.439)/4 + 128.5), 16), 240);
				y+=2; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

template<class T>
void ConvertYUV422ToULY0(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	uint8_t *y = pDstYBegin;
	uint8_t *u = pDstUBegin;
	uint8_t *v = pDstVBegin;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	for (const uint8_t *pStripeBegin = pSrcBegin; pStripeBegin != pSrcEnd; pStripeBegin += scbRawStripeSize)
	{
		for (const uint8_t *pStrideBegin = pStripeBegin; pStrideBegin != pStripeBegin + scbPredictStride; pStrideBegin += scbStride)
		{
			const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
			for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += 4)
			{
				const uint8_t *q = p + scbPredictStride;
				*(y+0)                    = *(p+T::Y0);
				*(y+1)                    = *(p+T::Y1);
				*(y+dwYPlaneGrossWidth+0) = *(q+T::Y0);
				*(y+dwYPlaneGrossWidth+1) = *(q+T::Y1);
				*u                        = (*(p+T::U) + *(q+T::U)) / 2;
				*v                        = (*(p+T::V) + *(q+T::V)) / 2;

				y+=2; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

void CULY0Codec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *pDstYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	uint8_t *pDstUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	uint8_t *pDstVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			const uint8_t *pSrcYBegin, *pSrcVBegin, *pSrcUBegin;

			pSrcYBegin = ((uint8_t *)m_pInput);
			pSrcVBegin = pSrcYBegin + m_nWidth * m_nHeight;
			pSrcUBegin = pSrcVBegin + m_nWidth * m_nHeight / 4;

			pSrcYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pSrcVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pSrcUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[2]);
		}
		return;
	}

	const uint8_t *pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	const uint8_t *pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertYUV422ToULY0<CYUYVColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
		ConvertYUV422ToULY0<CUYVYColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertRGBToULY0<CBGRColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_dwRawGrossWidth, pSrcBegin - m_dwRawGrossWidth, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertRGBToULY0<CBGRAColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_dwRawGrossWidth, pSrcBegin - m_dwRawGrossWidth, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertRGBToULY0<CBGRColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertRGBToULY0<CBGRAColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY0<CRGBColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertRGBToULY0<CARGBColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	}
}

template<class T>
void ConvertULY0ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	const uint8_t *y = pSrcYBegin;
	const uint8_t *u = pSrcUBegin;
	const uint8_t *v = pSrcVBegin;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	for (uint8_t *pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += scbRawStripeSize)
	{
		for (uint8_t *pStrideBegin = pStripeBegin; pStrideBegin != pStripeBegin + scbPredictStride; pStrideBegin += scbStride)
		{
			uint8_t *pStrideEnd = pStrideBegin + cbWidth;
			for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP * 2)
			{
				uint8_t *q = p + scbPredictStride;
				*(p+T::G) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(p+T::B) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(p+T::R) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
				*(q+T::G) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(q+T::B) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(q+T::R) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*1.164                  + (*v-128)*1.596), 0), 255);
				if (T::BYPP == 4)
				{
					*(p+T::A) = 255;
					*(q+T::A) = 255;
				}
				y++;
				*(p+T::BYPP+T::G) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(p+T::BYPP+T::B) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(p+T::BYPP+T::R) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
				*(q+T::BYPP+T::G) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(q+T::BYPP+T::B) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(q+T::BYPP+T::R) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*1.164                  + (*v-128)*1.596), 0), 255);
				if (T::BYPP == 4)
				{
					*(p+T::BYPP+T::A) = 255;
					*(q+T::BYPP+T::A) = 255;
				}
				y++; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

template<class T>
void ConvertULY0ToYUV422(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	const uint8_t *y = pSrcYBegin;
	const uint8_t *u = pSrcUBegin;
	const uint8_t *v = pSrcVBegin;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	for (uint8_t *pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += scbRawStripeSize)
	{
		for (uint8_t *pStrideBegin = pStripeBegin; pStrideBegin != pStripeBegin + scbPredictStride; pStrideBegin += scbStride)
		{
			uint8_t *pStrideEnd = pStrideBegin + cbWidth;
			for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += 4)
			{
				uint8_t *q = p + scbPredictStride;
				*(p+T::Y0) = *(y+0);
				*(p+T::Y1) = *(y+1);
				*(q+T::Y0) = *(y+dwYPlaneGrossWidth+0);
				*(q+T::Y1) = *(y+dwYPlaneGrossWidth+1);
				*(p+T::U)  = *u;
				*(q+T::U)  = *u;
				*(p+T::V)  = *v;
				*(q+T::V)  = *v;

				y+=2; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

void CULY0Codec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *pSrcYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	const uint8_t *pSrcUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	const uint8_t *pSrcVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			uint8_t *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((uint8_t *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight / 4;

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[2]);
		}
		return;
	}

	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertULY0ToYUV422<CYUYVColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
		ConvertULY0ToYUV422<CUYVYColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertULY0ToRGB<CBGRColorOrder>(pDstEnd - m_dwRawGrossWidth, pDstBegin - m_dwRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULY0ToRGB<CBGRAColorOrder>(pDstEnd - m_dwRawGrossWidth, pDstBegin - m_dwRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULY0ToRGB<CBGRColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULY0ToRGB<CBGRAColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY0ToRGB<CRGBColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY0ToRGB<CARGBColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_dwRawNetWidth, m_dwRawGrossWidth, m_dwPlanePredictStride[0], m_bInterlace);
		break;
	}
}

bool CULY0Codec::DecodeDirect(uint32_t nBandIndex)
{
	if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) != FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
		return false;

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			uint8_t *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((uint8_t *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight / 4;

			uint8_t *pDstYEnd = pDstYBegin + m_dwPlaneStripeEnd[nBandIndex] * m_dwPlaneStripeSize[0];
			uint8_t *pDstUEnd = pDstUBegin + m_dwPlaneStripeEnd[nBandIndex] * m_dwPlaneStripeSize[1];
			uint8_t *pDstVEnd = pDstVBegin + m_dwPlaneStripeEnd[nBandIndex] * m_dwPlaneStripeSize[2];

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			HuffmanDecodeAndAccum(pDstYBegin, pDstYEnd, m_pDecodeCode[0][nBandIndex], &m_hdt[0], pDstYEnd - pDstYBegin, pDstYEnd - pDstYBegin);
			HuffmanDecodeAndAccum(pDstUBegin, pDstUEnd, m_pDecodeCode[1][nBandIndex], &m_hdt[1], pDstUEnd - pDstUBegin, pDstUEnd - pDstUBegin);
			HuffmanDecodeAndAccum(pDstVBegin, pDstVEnd, m_pDecodeCode[2][nBandIndex], &m_hdt[2], pDstVEnd - pDstVBegin, pDstVEnd - pDstVBegin);
		}
		return true;
	}

	return false;
}
