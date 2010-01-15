/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULRAEncoder.h"
#include "Predict.h"
#include "resource.h"

const CPlanarEncoder::INPUTFORMAT CULRAEncoder::m_infmts[] = {
	{ BI_RGB, 32 },
};

CULRAEncoder::CULRAEncoder(void)
{
}

CULRAEncoder::~CULRAEncoder(void)
{
}

int CULRAEncoder::GetNumSupportedInputFormats(void)
{
	return _countof(m_infmts);
}

CEncoder *CULRAEncoder::CreateInstance(void)
{
	return new CULRAEncoder();
}

void CULRAEncoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
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

void CULRAEncoder::ConvertToPlanar(DWORD nBandIndex)
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
