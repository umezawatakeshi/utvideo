/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULY2Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "MediaSubType.h"

const FORMATINFO CULY2Codec::m_fiEncoderInput[] = {
	{ FCC('YUY2'), 16, MEDIASUBTYPE_YUY2 },	{ FCC('YUYV'), 16, MEDIASUBTYPE_YUYV }, { FCC('YUNV'), 16, MEDIASUBTYPE_YUNV },
	{ FCC('UYVY'), 16, MEDIASUBTYPE_UYVY }, { FCC('UYNV'), 16, MEDIASUBTYPE_UYNV },
	{ FCC('YVYU'), 16, MEDIASUBTYPE_YVYU },
	{ FCC('VYUY'), 16, MEDIASUBTYPE_VYUY },
	{ BI_RGB, 24, MEDIASUBTYPE_RGB24 },
	{ BI_RGB, 32, MEDIASUBTYPE_RGB32 },
	FORMATINFO_END,
};

const FORMATINFO CULY2Codec::m_fiDecoderOutput[] = {
	{ FCC('YUY2'), 16, MEDIASUBTYPE_YUY2 },	{ FCC('YUYV'), 16, MEDIASUBTYPE_YUYV }, { FCC('YUNV'), 16, MEDIASUBTYPE_YUNV },
	{ FCC('UYVY'), 16, MEDIASUBTYPE_UYVY }, { FCC('UYNV'), 16, MEDIASUBTYPE_UYNV },
	{ FCC('YVYU'), 16, MEDIASUBTYPE_YVYU },
	{ FCC('VYUY'), 16, MEDIASUBTYPE_VYUY },
	{ BI_RGB, 24, MEDIASUBTYPE_RGB24 },
	{ BI_RGB, 32, MEDIASUBTYPE_RGB32 },
	FORMATINFO_END,
};

const FORMATINFO CULY2Codec::m_fiCompressed[] = {
	{ FCC('ULY2'), 24, MEDIASUBTYPE_ULY2 },
	FORMATINFO_END,
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

void CULY2Codec::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
{
	m_dwPlaneSize[0]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]          = pbih->biWidth * pbih->biHeight / 2;
	m_dwPlaneSize[2]          = pbih->biWidth * pbih->biHeight / 2;

	m_dwPlaneWidth[0]         = pbih->biWidth;
	m_dwPlaneWidth[1]         = pbih->biWidth / 2;
	m_dwPlaneWidth[2]         = pbih->biWidth / 2;

	m_dwPlaneStripeSize[0]    = pbih->biWidth;
	m_dwPlaneStripeSize[1]    = pbih->biWidth / 2;
	m_dwPlaneStripeSize[2]    = pbih->biWidth / 2;

	m_dwPlanePredictStride[0] = pbih->biWidth;
	m_dwPlanePredictStride[1] = pbih->biWidth / 2;
	m_dwPlanePredictStride[2] = pbih->biWidth / 2;
}

void CULY2Codec::ConvertToPlanar(DWORD nBandIndex)
{
	BYTE *y, *u, *v;
	const BYTE *pSrcBegin, *pSrcEnd, *p;

	pSrcBegin = ((BYTE *)m_icc->lpInput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pSrcEnd   = ((BYTE *)m_icc->lpInput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	y = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_icc->lpbiInput->biCompression)
	{
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
		for (p = pSrcBegin; p < pSrcEnd; p += 4)
		{
			*y++ = *p;
			*u++ = *(p+1);
			*y++ = *(p+2);
			*v++ = *(p+3);
		}
		break;
	case FCC('UYVY'):
	case FCC('UYNV'):
		for (p = pSrcBegin; p < pSrcEnd; p += 4)
		{
			*u++ = *p;
			*y++ = *(p+1);
			*v++ = *(p+2);
			*y++ = *(p+3);
		}
		break;
	case FCC('YVYU'):
		for (p = pSrcBegin; p < pSrcEnd; p += 4)
		{
			*y++ = *p;
			*v++ = *(p+1);
			*y++ = *(p+2);
			*u++ = *(p+3);
		}
		break;
	case FCC('VYUY'):
		for (p = pSrcBegin; p < pSrcEnd; p += 4)
		{
			*v++ = *p;
			*y++ = *(p+1);
			*u++ = *(p+2);
			*y++ = *(p+3);
		}
		break;
	case BI_RGB:
		switch (m_icc->lpbiInput->biBitCount)
		{
		case 24:
			ConvertBottomupRGB24ToULY2(y, u, v, pSrcBegin, pSrcEnd, m_dwRawGrossWidth, m_icc->lpbiOutput->biWidth * 3);
			break;
		case 32:
			ConvertBottomupRGB32ToULY2(y, u, v, pSrcBegin, pSrcEnd, m_dwRawGrossWidth, m_dwRawGrossWidth);
			break;
		}
		break;
	}
}

void CULY2Codec::ConvertFromPlanar(DWORD nBandIndex)
{
	const BYTE *y, *u, *v;
	BYTE *pDstBegin, *pDstEnd, *p;

	pDstBegin = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pDstEnd   = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	y = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	u = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	v = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_icd->lpbiOutput->biCompression)
	{
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
		for (p = pDstBegin; p < pDstEnd; p += 4)
		{
			*p     = *y++;
			*(p+1) = *u++;
			*(p+2) = *y++;
			*(p+3) = *v++;
		}
		break;
	case FCC('UYVY'):
	case FCC('UYNV'):
		for (p = pDstBegin; p < pDstEnd; p += 4)
		{
			*p     = *u++;
			*(p+1) = *y++;
			*(p+2) = *v++;
			*(p+3) = *y++;
		}
		break;
	case FCC('YVYU'):
		for (p = pDstBegin; p < pDstEnd; p += 4)
		{
			*p     = *y++;
			*(p+1) = *v++;
			*(p+2) = *y++;
			*(p+3) = *u++;
		}
		break;
	case FCC('VYUY'):
		for (p = pDstBegin; p < pDstEnd; p += 4)
		{
			*p     = *v++;
			*(p+1) = *y++;
			*(p+2) = *u++;
			*(p+3) = *y++;
		}
		break;
	case BI_RGB:
		switch (m_icd->lpbiOutput->biBitCount)
		{
		case 24:
			ConvertULY2ToBottomupRGB24(pDstBegin, pDstEnd, y, u, v, m_dwRawGrossWidth, m_icd->lpbiOutput->biWidth * 3);
			break;
		case 32:
			ConvertULY2ToBottomupRGB32(pDstBegin, pDstEnd, y, u, v, m_dwRawGrossWidth, m_dwRawGrossWidth);
			break;
		}
		break;
	}
}
