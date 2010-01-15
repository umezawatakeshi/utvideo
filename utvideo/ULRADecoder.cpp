/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULRADecoder.h"
#include "Predict.h"

const CPlanarDecoder::OUTPUTFORMAT CULRADecoder::m_outfmts[] = {
	{ BI_RGB, 32 },
};

CULRADecoder::CULRADecoder(void)
{
}

CULRADecoder::~CULRADecoder(void)
{
}

int CULRADecoder::GetNumSupportedOutputFormats(void)
{
	return _countof(m_outfmts);
}

CDecoder *CULRADecoder::CreateInstance(void)
{
	return new CULRADecoder();
}

void CULRADecoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
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

void CULRADecoder::ConvertFromPlanar(DWORD nBandIndex)
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
