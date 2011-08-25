/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULY0Codec.h"
#include "Predict.h"
#include "MediaSubType.h"

const utvf_t CULY0Codec::m_utvfEncoderInput[] = {
	UTVF_YV12,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
	UTVF_RGB24_WIN,
	UTVF_RGB32_WIN,
	UTVF_INVALID,
};

const utvf_t CULY0Codec::m_utvfDecoderOutput[] = {
	UTVF_YV12,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV,
	UTVF_UYVY, UTVF_UYNV,
	UTVF_RGB24_WIN,
	UTVF_RGB32_WIN,
	UTVF_INVALID,
};

const utvf_t CULY0Codec::m_utvfCompressed[] = {
	UTVF_ULY0,
	UTVF_INVALID,
};

CULY0Codec::CULY0Codec(const char *pszInterfaceName) : CUL00Codec(pszInterfaceName)
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

void CULY0Codec::ConvertBottomupRGBToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD bypp)
{
	BYTE *y = pDstYBegin;
	BYTE *u = pDstUBegin;
	BYTE *v = pDstVBegin;

	DWORD dwRawPredictStride = m_dwRawGrossWidth * (m_bInterlace ? 2 : 1);

	const BYTE *pSrcBegin = ((BYTE *)m_pInput) + (m_dwNumStripes - m_dwPlaneStripeEnd[nBandIndex]  ) * m_dwRawStripeSize;
	const BYTE *pSrcEnd   = ((BYTE *)m_pInput) + (m_dwNumStripes - m_dwPlaneStripeBegin[nBandIndex]) * m_dwRawStripeSize;

	for (const BYTE *pStripeBegin = pSrcEnd; pStripeBegin > pSrcBegin; pStripeBegin -= m_dwRawStripeSize) {
		for (const BYTE *pStrideBegin = pStripeBegin - m_dwRawGrossWidth; pStrideBegin >= pStripeBegin - dwRawPredictStride; pStrideBegin -= m_dwRawGrossWidth)
		{
			const BYTE *pStrideEnd = pStrideBegin + m_dwRawNetWidth;
			for (const BYTE *q = pStrideBegin; q < pStrideEnd; q += bypp * 2)
			{
				const BYTE *p = q - dwRawPredictStride;
				*(y+0)                           = min(max(int((*(q     +0))*0.098 + (*(q     +1))*0.504 + (*(q     +2))*0.257 + 16.5), 16), 235);
				*(y+1)                           = min(max(int((*(q+bypp+0))*0.098 + (*(q+bypp+1))*0.504 + (*(q+bypp+2))*0.257 + 16.5), 16), 235);
				*(y+m_dwPlanePredictStride[0]+0) = min(max(int((*(p     +0))*0.098 + (*(p     +1))*0.504 + (*(p     +2))*0.257 + 16.5), 16), 235);
				*(y+m_dwPlanePredictStride[0]+1) = min(max(int((*(p+bypp+0))*0.098 + (*(p+bypp+1))*0.504 + (*(p+bypp+2))*0.257 + 16.5), 16), 235);
				*u                               = min(max(int(((*(p+0)+*(p+bypp+0)+*(q+0)+*(q+bypp+0))*0.439 + (*(p+1)+*(p+bypp+1)+*(q+1)+*(q+bypp+1))*-0.291 + (*(p+2)+*(p+bypp+2)+*(q+2)+*(q+bypp+2))*-0.148)/4 + 128.5), 16), 240);
				*v                               = min(max(int(((*(p+0)+*(p+bypp+0)+*(q+0)+*(q+bypp+0))*-0.071 + (*(p+1)+*(p+bypp+1)+*(q+1)+*(q+bypp+1))*-0.368 + (*(p+2)+*(p+bypp+2)+*(q+2)+*(q+bypp+2))*0.439)/4 + 128.5), 16), 240);
				y+=2; u++; v++;
			}
		}
		y += m_dwPlanePredictStride[0];
	}
}

void CULY0Codec::ConvertYUV422ToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD nYOffset)
{
	BYTE *y = pDstYBegin;
	BYTE *u = pDstUBegin;
	BYTE *v = pDstVBegin;

	const BYTE *pSrcBegin = ((BYTE *)m_pInput) + m_dwPlaneStripeBegin[nBandIndex] * m_dwRawStripeSize;
	const BYTE *pSrcEnd   = ((BYTE *)m_pInput) + m_dwPlaneStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	for (const BYTE *pStrideBegin = pSrcBegin; pStrideBegin < pSrcEnd; pStrideBegin += m_dwRawStripeSize)
	{
		const BYTE *pStrideEnd = pStrideBegin + m_dwRawStripeSize / 2;
		for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			const BYTE *q = p + m_dwRawStripeSize / 2;
			*(y+0) = *(p+0+nYOffset);
			*(y+1) = *(p+2+nYOffset);
			*(y+m_dwPlanePredictStride[0]+0) = *(q+0+nYOffset);
			*(y+m_dwPlanePredictStride[0]+1) = *(q+2+nYOffset);
			*u = (*(p+1-nYOffset) + *(q+1-nYOffset)) / 2;
			*v = (*(p+3-nYOffset) + *(q+3-nYOffset)) / 2;

			y+=2; u++; v++;
		}
		y += m_dwPlanePredictStride[0];
	}
}

void CULY0Codec::ConvertToPlanar(DWORD nBandIndex)
{
	BYTE *pDstYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	BYTE *pDstUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	BYTE *pDstVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			const BYTE *pSrcYBegin, *pSrcVBegin, *pSrcUBegin;

			pSrcYBegin = ((BYTE *)m_pInput);
			pSrcVBegin = pSrcYBegin + m_nWidth * m_nHeight;
			pSrcUBegin = pSrcVBegin + m_nWidth * m_nHeight / 4;

			pSrcYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pSrcVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pSrcUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[2]);
		}
		break;
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertYUV422ToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 0);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
		ConvertYUV422ToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 1);
		break;
	case UTVF_RGB24_WIN:
		ConvertBottomupRGBToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 3);
		break;
	case UTVF_RGB32_WIN:
		ConvertBottomupRGBToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 4);
		break;
	}
}

void CULY0Codec::ConvertULY0ToBottomupRGB(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD bypp)
{
	const BYTE *y = pSrcYBegin;
	const BYTE *u = pSrcUBegin;
	const BYTE *v = pSrcVBegin;

	DWORD dwRawPredictStride = m_dwRawGrossWidth * (m_bInterlace ? 2 : 1);

	BYTE *pDstBegin = ((BYTE *)m_pOutput) + (m_dwNumStripes - m_dwPlaneStripeEnd[nBandIndex]  ) * m_dwRawStripeSize;
	BYTE *pDstEnd   = ((BYTE *)m_pOutput) + (m_dwNumStripes - m_dwPlaneStripeBegin[nBandIndex]) * m_dwRawStripeSize;

	for (BYTE *pStripeBegin = pDstEnd; pStripeBegin > pDstBegin; pStripeBegin -= m_dwRawStripeSize) {
		for (BYTE *pStrideBegin = pStripeBegin - m_dwRawGrossWidth; pStrideBegin >= pStripeBegin - dwRawPredictStride; pStrideBegin -= m_dwRawGrossWidth)
		{
			BYTE *pStrideEnd = pStrideBegin + m_dwRawNetWidth;
			for (BYTE *q = pStrideBegin; q < pStrideEnd; q += bypp * 2)
			{
				BYTE *p = q - dwRawPredictStride;
				*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
				*(p+1) = min(max(int((*(y+m_dwPlanePredictStride[0])-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(p+0) = min(max(int((*(y+m_dwPlanePredictStride[0])-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(p+2) = min(max(int((*(y+m_dwPlanePredictStride[0])-16)*1.164                  + (*v-128)*1.596), 0), 255);
				if (bypp == 4)
				{
					*(q+3) = 255;
					*(p+3) = 255;
				}
				y++;
				*(q+bypp+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(q+bypp+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(q+bypp+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
				*(p+bypp+1) = min(max(int((*(y+m_dwPlanePredictStride[0])-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(p+bypp+0) = min(max(int((*(y+m_dwPlanePredictStride[0])-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(p+bypp+2) = min(max(int((*(y+m_dwPlanePredictStride[0])-16)*1.164                  + (*v-128)*1.596), 0), 255);
				if (bypp == 4)
				{
					*(q+bypp+3) = 255;
					*(p+bypp+3) = 255;
				}
				y++; u++; v++;
			}
		}
		y += m_dwPlanePredictStride[0];
	}
}

void CULY0Codec::ConvertULY0ToYUV422(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD nYOffset)
{
	const BYTE *y = pSrcYBegin;
	const BYTE *u = pSrcUBegin;
	const BYTE *v = pSrcVBegin;

	BYTE *pDstBegin = ((BYTE *)m_pOutput) + m_dwPlaneStripeBegin[nBandIndex] * m_dwRawStripeSize;
	BYTE *pDstEnd   = ((BYTE *)m_pOutput) + m_dwPlaneStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	for (BYTE *pStrideBegin = pDstBegin; pStrideBegin < pDstEnd; pStrideBegin += m_dwRawStripeSize)
	{
		BYTE *pStrideEnd = pStrideBegin + m_dwRawStripeSize / 2;
		for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			BYTE *q = p + m_dwRawStripeSize / 2;
			*(p+0+nYOffset) = *(y+0);
			*(p+2+nYOffset) = *(y+1);
			*(q+0+nYOffset) = *(y+m_dwPlanePredictStride[0]+0);
			*(q+2+nYOffset) = *(y+m_dwPlanePredictStride[0]+1);
			*(p+1-nYOffset) = *u;
			*(q+1-nYOffset) = *u;
			*(p+3-nYOffset) = *v;
			*(q+3-nYOffset) = *v;

			y+=2; u++; v++;
		}
		y += m_dwPlanePredictStride[0];
	}
}

void CULY0Codec::ConvertFromPlanar(DWORD nBandIndex)
{
	const BYTE *pSrcYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	const BYTE *pSrcUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	const BYTE *pSrcVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			BYTE *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((BYTE *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight / 4;

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[2]);
		}
		break;
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertULY0ToYUV422(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 0);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
		ConvertULY0ToYUV422(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 1);
		break;
	case UTVF_RGB24_WIN:
		ConvertULY0ToBottomupRGB(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 3);
		break;
	case UTVF_RGB32_WIN:
		ConvertULY0ToBottomupRGB(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 4);
		break;
	}
}

bool CULY0Codec::DecodeDirect(DWORD nBandIndex)
{
	if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) != FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
		return false;

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			BYTE *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((BYTE *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight / 4;

			BYTE *pDstYEnd = pDstYBegin + m_dwPlaneStripeEnd[nBandIndex] * m_dwPlaneStripeSize[0];
			BYTE *pDstUEnd = pDstUBegin + m_dwPlaneStripeEnd[nBandIndex] * m_dwPlaneStripeSize[1];
			BYTE *pDstVEnd = pDstVBegin + m_dwPlaneStripeEnd[nBandIndex] * m_dwPlaneStripeSize[2];

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			HuffmanDecodeAndAccum(pDstYBegin, pDstYEnd, m_pDecodeCode[0][nBandIndex], &m_hdt[0]);
			HuffmanDecodeAndAccum(pDstUBegin, pDstUEnd, m_pDecodeCode[1][nBandIndex], &m_hdt[1]);
			HuffmanDecodeAndAccum(pDstVBegin, pDstVEnd, m_pDecodeCode[2][nBandIndex], &m_hdt[2]);
		}
		return true;
	}

	return false;
}
