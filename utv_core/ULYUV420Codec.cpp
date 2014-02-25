/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULYUV420Codec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "ColorOrder.h"

template<>
const utvf_t CULYUV420Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
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

template<>
const utvf_t CULYUV420Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
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

template<>
const utvf_t CULYUV420Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
#ifndef __APPLE__
	UTVF_YV12,
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
const utvf_t CULYUV420Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
#ifndef __APPLE__
	UTVF_YV12,
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
const utvf_t CULYUV420Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CULYUV420Codec<CBT601Coefficient>::CULYUV420Codec(const char *pszInterfaceName) : CUL00Codec("ULY0", pszInterfaceName)
{
}

template<>
CULYUV420Codec<CBT709Coefficient>::CULYUV420Codec(const char *pszInterfaceName) : CUL00Codec("ULH0", pszInterfaceName)
{
}

template<>
const char *CULYUV420Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV420 BT.601";
}

template<>
const char *CULYUV420Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV420 BT.709";
}

template<class C>
void CULYUV420Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height / 4;
	m_cbPlaneSize[2]          = width * height / 4;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width / 2;
	m_cbPlaneWidth[2]         = width / 2;

	m_cbPlaneStripeSize[0]    = width * 2;
	m_cbPlaneStripeSize[1]    = width / 2;
	m_cbPlaneStripeSize[2]    = width / 2;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width / 2;
	m_cbPlanePredictStride[2] = width / 2;
}

template<class C, class T>
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
				*(y+0)                    = min(max(int((*(p        +T::B))*C::B2Y + (*(p        +T::G))*C::G2Y + (*(p        +T::R))*C::R2Y + 16.5), 16), 235);
				*(y+1)                    = min(max(int((*(p+T::BYPP+T::B))*C::B2Y + (*(p+T::BYPP+T::G))*C::G2Y + (*(p+T::BYPP+T::R))*C::R2Y + 16.5), 16), 235);
				*(y+dwYPlaneGrossWidth+0) = min(max(int((*(q        +T::B))*C::B2Y + (*(q        +T::G))*C::G2Y + (*(q        +T::R))*C::R2Y + 16.5), 16), 235);
				*(y+dwYPlaneGrossWidth+1) = min(max(int((*(q+T::BYPP+T::B))*C::B2Y + (*(q+T::BYPP+T::G))*C::G2Y + (*(q+T::BYPP+T::R))*C::R2Y + 16.5), 16), 235);
				*u                        = min(max(int(((*(p+T::B)+*(p+T::BYPP+T::B)+*(q+T::B)+*(q+T::BYPP+T::B))*C::B2U + (*(p+T::G)+*(p+T::BYPP+T::G)+*(q+T::G)+*(q+T::BYPP+T::G))*C::G2U + (*(p+T::R)+*(p+T::BYPP+T::R)+*(q+T::R)+*(q+T::BYPP+T::R))*C::R2U)/4 + 128.5), 16), 240);
				*v                        = min(max(int(((*(p+T::B)+*(p+T::BYPP+T::B)+*(q+T::B)+*(q+T::BYPP+T::B))*C::B2V + (*(p+T::G)+*(p+T::BYPP+T::G)+*(q+T::G)+*(q+T::BYPP+T::G))*C::G2V + (*(p+T::R)+*(p+T::BYPP+T::R)+*(q+T::R)+*(q+T::BYPP+T::R))*C::R2V)/4 + 128.5), 16), 240);
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

template<class C>
void CULYUV420Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *pDstYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	uint8_t *pDstUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	uint8_t *pDstVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			const uint8_t *pSrcYBegin, *pSrcVBegin, *pSrcUBegin;

			pSrcYBegin = ((uint8_t *)m_pInput);
			pSrcVBegin = pSrcYBegin + m_nWidth * m_nHeight;
			pSrcUBegin = pSrcVBegin + m_nWidth * m_nHeight / 4;

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
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertYUV422ToULY0<CYUYVColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_HDYC:
		ConvertYUV422ToULY0<CUYVYColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertRGBToULY0<C, CBGRColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertRGBToULY0<C, CBGRAColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertRGBToULY0<C, CBGRColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertRGBToULY0<C, CBGRAColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY0<C, CRGBColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertRGBToULY0<C, CARGBColorOrder>(pDstYBegin, pDstUBegin, pDstVBegin, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	}
}

template<class C, class T>
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
				*(p+T::G) = min(max(int((*y                     -16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
				*(p+T::B) = min(max(int((*y                     -16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
				*(p+T::R) = min(max(int((*y                     -16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
				*(q+T::G) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
				*(q+T::B) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
				*(q+T::R) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
				if (T::BYPP == 4)
				{
					*(p+T::A) = 255;
					*(q+T::A) = 255;
				}
				y++;
				*(p+T::BYPP+T::G) = min(max(int((*y                     -16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
				*(p+T::BYPP+T::B) = min(max(int((*y                     -16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
				*(p+T::BYPP+T::R) = min(max(int((*y                     -16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
				*(q+T::BYPP+T::G) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
				*(q+T::BYPP+T::B) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
				*(q+T::BYPP+T::R) = min(max(int((*(y+dwYPlaneGrossWidth)-16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
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

template<class C>
void CULYUV420Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *pSrcYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	const uint8_t *pSrcUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	const uint8_t *pSrcVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			uint8_t *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((uint8_t *)m_pOutput);
			pDstVBegin = pDstYBegin + m_nWidth * m_nHeight;
			pDstUBegin = pDstVBegin + m_nWidth * m_nHeight / 4;

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
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
		ConvertULY0ToYUV422<CYUYVColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_HDYC:
		ConvertULY0ToYUV422<CUYVYColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_BU:
		ConvertULY0ToRGB<C, CBGRColorOrder>(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULY0ToRGB<C, CBGRAColorOrder>(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULY0ToRGB<C, CBGRColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULY0ToRGB<C, CBGRAColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY0ToRGB<C, CRGBColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY0ToRGB<C, CARGBColorOrder>(pDstBegin, pDstEnd, pSrcYBegin, pSrcUBegin, pSrcVBegin, m_cbRawNetWidth, m_cbRawGrossWidth, m_cbPlanePredictStride[0], m_bInterlace);
		break;
	}
}

template<class C>
bool CULYUV420Codec<C>::DecodeDirect(uint32_t nBandIndex)
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
