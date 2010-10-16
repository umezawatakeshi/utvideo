/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULRACodec.h"
#include "Predict.h"
#include "MediaSubType.h"

const FORMATINFO CULRACodec::m_fiEncoderInput[] = {
	{ BI_RGB, 32, MEDIASUBTYPE_ARGB32 },
	{ BI_RGB, 32, MEDIASUBTYPE_RGB32 },
	FORMATINFO_END,
};

const FORMATINFO CULRACodec::m_fiDecoderOutput[] = {
	{ BI_RGB, 32, MEDIASUBTYPE_ARGB32 },
	{ BI_RGB, 32, MEDIASUBTYPE_RGB32 },
	FORMATINFO_END,
};

const FORMATINFO CULRACodec::m_fiCompressed[] = {
	{ FCC('ULRA'), 32, MEDIASUBTYPE_ULRA },
	FORMATINFO_END,
};

CULRACodec::CULRACodec(const char *pszInterfaceName) : CUL00Codec(FCC('ULRA'), pszInterfaceName)
{
}

CULRACodec::~CULRACodec(void)
{
}

CCodec *CULRACodec::CreateInstance(const char *pszInterfaceName)
{
	return new CULRACodec(pszInterfaceName);
}

void CULRACodec::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
{
	m_dwPlaneSize[0]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[2]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[3]          = pbih->biWidth * pbih->biHeight;

	m_dwPlaneWidth[0]         = pbih->biWidth;
	m_dwPlaneWidth[1]         = pbih->biWidth;
	m_dwPlaneWidth[2]         = pbih->biWidth;
	m_dwPlaneWidth[3]         = pbih->biWidth;

	m_dwPlaneStripeSize[0]    = pbih->biWidth;
	m_dwPlaneStripeSize[1]    = pbih->biWidth;
	m_dwPlaneStripeSize[2]    = pbih->biWidth;
	m_dwPlaneStripeSize[3]    = pbih->biWidth;

	m_dwPlanePredictStride[0] = pbih->biWidth;
	m_dwPlanePredictStride[1] = pbih->biWidth;
	m_dwPlanePredictStride[2] = pbih->biWidth;
	m_dwPlanePredictStride[3] = pbih->biWidth;
}

void CULRACodec::ConvertToPlanar(DWORD nBandIndex)
{
	BYTE *g, *b, *r, *a;
	const BYTE *pSrcBegin, *pSrcEnd, *pStrideBegin, *p;

	pSrcBegin = ((BYTE *)m_icc->lpInput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pSrcEnd   = ((BYTE *)m_icc->lpInput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[3];

	switch (m_icc->lpbiInput->biCompression)
	{
	case BI_RGB:
		switch (m_icc->lpbiInput->biBitCount)
		{
		case 32:
			for (pStrideBegin = pSrcEnd - m_dwRawGrossWidth; pStrideBegin >= pSrcBegin; pStrideBegin -= m_dwRawGrossWidth)
			{
				const BYTE *pStrideEnd = pStrideBegin + m_icc->lpbiInput->biWidth * 4;
				for (p = pStrideBegin; p < pStrideEnd; p += 4)
				{
					*g++ = *(p+1);
					*b++ = *(p+0) - *(p+1) + 0x80;
					*r++ = *(p+2) - *(p+1) + 0x80;
					*a++ = *(p+3);
				}
			}
			break;
		}
		break;
	}
}

void CULRACodec::ConvertFromPlanar(DWORD nBandIndex)
{
	const BYTE *g, *b, *r, *a;
	BYTE *pDstBegin, *pDstEnd, *pStrideBegin, *p;

	pDstBegin = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pDstEnd   = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[3];

	switch (m_icd->lpbiOutput->biCompression)
	{
	case BI_RGB:
		switch (m_icd->lpbiOutput->biBitCount)
		{
		case 32:
			for (pStrideBegin = pDstEnd - m_dwRawGrossWidth; pStrideBegin >= pDstBegin; pStrideBegin -= m_dwRawGrossWidth)
			{
				BYTE *pStrideEnd = pStrideBegin + m_icd->lpbiOutput->biWidth * 4;
				for (p = pStrideBegin; p < pStrideEnd; p += 4)
				{
					*(p+1) = *g;
					*(p+0) = *b + *g - 0x80;
					*(p+2) = *r + *g - 0x80;
					*(p+3) = *a;
					g++; b++; r++; a++;
				}
			}
			break;
		}
		break;
	}
}

BOOL CULRACodec::DecodeDirect(DWORD nBandIndex)
{
	if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) != FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
		return FALSE;

	BYTE *pDstBegin = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	BYTE *pDstEnd   = ((BYTE *)m_icd->lpOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	switch (m_icd->lpbiOutput->biCompression)
	{
	case BI_RGB:
		switch (m_icd->lpbiOutput->biBitCount)
		{
		case 32:
			cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, m_dwRawGrossWidth);
			cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue (pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, m_dwRawGrossWidth);
			cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red  (pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, m_dwRawGrossWidth);
			cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(pDstBegin+3, pDstEnd+3, m_pDecodeCode[3][nBandIndex], &m_hdt[3], m_dwRawNetWidth, m_dwRawGrossWidth);
			return TRUE;
		}
		return FALSE;
	default:
		return FALSE;
	}
}