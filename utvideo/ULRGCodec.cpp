/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULRGCodec.h"
#include "Predict.h"
#include "MediaSubType.h"

const FORMATINFO CULRGCodec::m_fiEncoderInput[] = {
	{ BI_RGB, 24, MEDIASUBTYPE_RGB24 },
	{ BI_RGB, 32, MEDIASUBTYPE_RGB32 },
	FORMATINFO_END,
};

const FORMATINFO CULRGCodec::m_fiDecoderOutput[] = {
	{ BI_RGB, 24, MEDIASUBTYPE_RGB24 },
	{ BI_RGB, 32, MEDIASUBTYPE_RGB32 },
	FORMATINFO_END,
};

const FORMATINFO CULRGCodec::m_fiCompressed[] = {
	{ FCC('ULRG'), 24, MEDIASUBTYPE_ULRG },
	FORMATINFO_END,
};

CULRGCodec::CULRGCodec(const char *pszInterfaceName) : CUL00Codec(pszInterfaceName)
{
}

CULRGCodec::~CULRGCodec(void)
{
}

CCodec *CULRGCodec::CreateInstance(const char *pszInterfaceName)
{
	return new CULRGCodec(pszInterfaceName);
}

void CULRGCodec::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
{
	m_dwPlaneSize[0]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[2]          = pbih->biWidth * pbih->biHeight;

	m_dwPlaneWidth[0]         = pbih->biWidth;
	m_dwPlaneWidth[1]         = pbih->biWidth;
	m_dwPlaneWidth[2]         = pbih->biWidth;

	m_dwPlaneStripeSize[0]    = pbih->biWidth;
	m_dwPlaneStripeSize[1]    = pbih->biWidth;
	m_dwPlaneStripeSize[2]    = pbih->biWidth;

	m_dwPlanePredictStride[0] = pbih->biWidth;
	m_dwPlanePredictStride[1] = pbih->biWidth;
	m_dwPlanePredictStride[2] = pbih->biWidth;
}

void CULRGCodec::ConvertToPlanar(DWORD nBandIndex)
{
	BYTE *g, *b, *r;
	const BYTE *pSrcBegin, *pSrcEnd, *pStrideBegin, *p;

	pSrcBegin = ((BYTE *)m_icc->lpInput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pSrcEnd   = ((BYTE *)m_icc->lpInput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_icc->lpbiInput->biCompression)
	{
	case BI_RGB:
		switch (m_icc->lpbiInput->biBitCount)
		{
		case 24:
			for (pStrideBegin = pSrcEnd - m_dwRawGrossWidth; pStrideBegin >= pSrcBegin; pStrideBegin -= m_dwRawGrossWidth)
			{
				const BYTE *pStrideEnd = pStrideBegin + m_icc->lpbiInput->biWidth * 3;
				for (p = pStrideBegin; p < pStrideEnd; p += 3)
				{
					*g++ = *(p+1);
					*b++ = *(p+0) - *(p+1) + 0x80;
					*r++ = *(p+2) - *(p+1) + 0x80;
				}
			}
			break;
		case 32:
			for (pStrideBegin = pSrcEnd - m_dwRawGrossWidth; pStrideBegin >= pSrcBegin; pStrideBegin -= m_dwRawGrossWidth)
			{
				const BYTE *pStrideEnd = pStrideBegin + m_icc->lpbiInput->biWidth * 4;
				for (p = pStrideBegin; p < pStrideEnd; p += 4)
				{
					*g++ = *(p+1);
					*b++ = *(p+0) - *(p+1) + 0x80;
					*r++ = *(p+2) - *(p+1) + 0x80;
				}
			}
			break;
		}
		break;
	}
}

void CULRGCodec::ConvertFromPlanar(DWORD nBandIndex)
{
	const BYTE *g, *b, *r;
	BYTE *pDstBegin, *pDstEnd, *pStrideBegin, *p;

	pDstBegin = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pDstEnd   = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_icd->lpbiOutput->biCompression)
	{
	case BI_RGB:
		switch (m_icd->lpbiOutput->biBitCount)
		{
		case 24:
			for (pStrideBegin = pDstEnd - m_dwRawGrossWidth; pStrideBegin >= pDstBegin; pStrideBegin -= m_dwRawGrossWidth)
			{
				BYTE *pStrideEnd = pStrideBegin + m_icd->lpbiOutput->biWidth * 3;
				for (p = pStrideBegin; p < pStrideEnd; p += 3)
				{
					*(p+1) = *g;
					*(p+0) = *b + *g - 0x80;
					*(p+2) = *r + *g - 0x80;
					g++; b++; r++;
				}
			}
			break;
		case 32:
			for (pStrideBegin = pDstEnd - m_dwRawGrossWidth; pStrideBegin >= pDstBegin; pStrideBegin -= m_dwRawGrossWidth)
			{
				BYTE *pStrideEnd = pStrideBegin + m_icd->lpbiOutput->biWidth * 4;
				for (p = pStrideBegin; p < pStrideEnd; p += 4)
				{
					*(p+1) = *g;
					*(p+0) = *b + *g - 0x80;
					*(p+2) = *r + *g - 0x80;
					*(p+3) = 0xff;
					g++; b++; r++;
				}
			}
			break;
		}
		break;
	}
}
