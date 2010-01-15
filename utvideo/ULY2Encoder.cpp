/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULY2Encoder.h"
#include "Predict.h"
#include "Convert.h"
#include "resource.h"

const CPlanarEncoder::INPUTFORMAT CULY2Encoder::m_infmts[] = {
	{ FCC('YUY2'), 16 }, { FCC('YUYV'), 16 }, { FCC('YUNV'), 16 },
	{ FCC('UYVY'), 16 }, { FCC('UYNV'), 16 },
	{ FCC('YVYU'), 16 },
	{ FCC('VYUY'), 16 },
	{ BI_RGB, 32 },
	{ BI_RGB, 24 },
};

CULY2Encoder::CULY2Encoder(void)
{
}

CULY2Encoder::~CULY2Encoder(void)
{
}

int CULY2Encoder::GetNumSupportedInputFormats(void)
{
	return _countof(m_infmts);
}

CEncoder *CULY2Encoder::CreateInstance(void)
{
	return new CULY2Encoder();
}

void CULY2Encoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
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

void CULY2Encoder::ConvertToPlanar(DWORD nBandIndex)
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
