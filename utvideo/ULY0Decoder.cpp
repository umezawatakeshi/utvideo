/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008-2009  UMEZAWA Takeshi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * 
 * Ut Video Codec Suite
 * Copyright (C) 2008-2009  梅澤 威志
 * 
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフ
 * トウェア財団によって発行された GNU 一般公衆利用許諾契約書(バージョ
 * ン2か、希望によってはそれ以降のバージョンのうちどれか)の定める条件
 * の下で再頒布または改変することができます。
 * 
 * このプログラムは有用であることを願って頒布されますが、*全くの無保
 * 証* です。商業可能性の保証や特定の目的への適合性は、言外に示された
 * ものも含め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご
 * 覧ください。
 * 
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を
 * 一部受け取ったはずです。もし受け取っていなければ、フリーソフトウェ
 * ア財団まで請求してください(宛先は the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)。
 */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULY0Decoder.h"
#include "Predict.h"

const CULY0Decoder::OUTPUTFORMAT CULY0Decoder::m_outfmts[] = {
	{ FCC('YV12'), 12 },
	{ FCC('YUY2'), 16 }, { FCC('YUYV'), 16 }, { FCC('YUNV'), 16 },
	{ FCC('UYVY'), 16 }, { FCC('UYNV'), 16 },
	{ FCC('YVYU'), 16 },
	{ FCC('VYUY'), 16 },
//	{ BI_RGB, 32 },
//	{ BI_RGB, 24 },
};

CULY0Decoder::CULY0Decoder(void)
{
}

CULY0Decoder::~CULY0Decoder(void)
{
}

int CULY0Decoder::GetNumSupportedOutputFormats(void)
{
	return _countof(m_outfmts);
}

CDecoder *CULY0Decoder::CreateInstance(void)
{
	return new CULY0Decoder();
}

void CULY0Decoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
{
	m_dwPlaneSize[0]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]          = pbih->biWidth * pbih->biHeight / 4;
	m_dwPlaneSize[2]          = pbih->biWidth * pbih->biHeight / 4;

	m_dwPlaneWidth[0]         = pbih->biWidth;
	m_dwPlaneWidth[1]         = pbih->biWidth / 2;
	m_dwPlaneWidth[2]         = pbih->biWidth / 2;

	m_dwPlaneStripeSize[0]    = pbih->biWidth * 2;
	m_dwPlaneStripeSize[1]    = pbih->biWidth / 2;
	m_dwPlaneStripeSize[2]    = pbih->biWidth / 2;

	m_dwPlanePredictStride[0] = pbih->biWidth;
	m_dwPlanePredictStride[1] = pbih->biWidth / 2;
	m_dwPlanePredictStride[2] = pbih->biWidth / 2;
}

void CULY0Decoder::ConvertULY0ToBottomupRGB(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD bypp)
{
	const BYTE *y = pSrcYBegin;
	const BYTE *u = pSrcUBegin;
	const BYTE *v = pSrcVBegin;

	DWORD dwDataStride = m_icd->lpbiOutput->biWidth * bypp;
	DWORD dwDstStride = ROUNDUP(dwDataStride, 4);
	DWORD dwYStride = m_icd->lpbiOutput->biWidth; // = m_dwPlaneWidth[0]

	BYTE *pDstBegin = ((BYTE *)m_icd->lpOutput) + (m_dwNumStripes - m_dwPlaneStripeEnd[nBandIndex]  ) * m_dwRawStripeSize;
	BYTE *pDstEnd   = ((BYTE *)m_icd->lpOutput) + (m_dwNumStripes - m_dwPlaneStripeBegin[nBandIndex]) * m_dwRawStripeSize;

	for (BYTE *pStrideBegin = pDstEnd - dwDstStride * 2; pStrideBegin >= pDstBegin; pStrideBegin -= m_dwRawStripeSize)
	{
		BYTE *pStrideEnd = pStrideBegin + dwDataStride;
		for (BYTE *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			BYTE *q = p + dwDstStride;
			*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			*(p+1) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(p+0) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(p+2) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
			y++;
			*(q+bypp+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(q+bypp+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(q+bypp+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			*(p+bypp+1) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(p+bypp+0) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(p+bypp+2) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
			y++; u++; v++;
		}
		y += dwYStride;
	}
}

void CULY0Decoder::ConvertULY0ToYUV422(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD nYOffset)
{
	const BYTE *y = pSrcYBegin;
	const BYTE *u = pSrcUBegin;
	const BYTE *v = pSrcVBegin;

	BYTE *pDstBegin = ((BYTE *)m_icd->lpOutput) + m_dwPlaneStripeBegin[nBandIndex] * m_dwRawStripeSize;
	BYTE *pDstEnd   = ((BYTE *)m_icd->lpOutput) + m_dwPlaneStripeEnd[nBandIndex]   * m_dwRawStripeSize;

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

void CULY0Decoder::ConvertFromPlanar(DWORD nBandIndex)
{
	const BYTE *pSrcYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	const BYTE *pSrcUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	const BYTE *pSrcVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_icd->lpbiOutput->biCompression)
	{
	case FCC('YV12'):
		{
			BYTE *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((BYTE *)m_icd->lpOutput);
			pDstVBegin = pDstYBegin + m_icd->lpbiOutput->biWidth * m_icd->lpbiOutput->biHeight;
			pDstUBegin = pDstVBegin + m_icd->lpbiOutput->biWidth * m_icd->lpbiOutput->biHeight / 4;

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[2]);
		}
		break;
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
		ConvertULY0ToYUV422(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 0);
		break;
	case FCC('UYVY'):
	case FCC('UYNV'):
		ConvertULY0ToYUV422(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 1);
		break;
	case FCC('YVYU'):
		ConvertULY0ToYUV422(pSrcYBegin, pSrcVBegin, pSrcUBegin, nBandIndex, 0);
		break;
	case FCC('VYUY'):
		ConvertULY0ToYUV422(pSrcYBegin, pSrcVBegin, pSrcUBegin, nBandIndex, 1);
		break;
	case BI_RGB:
		switch (m_icd->lpbiOutput->biBitCount)
		{
		case 24:
			ConvertULY0ToBottomupRGB(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 3);
			break;
		case 32:
			ConvertULY0ToBottomupRGB(pSrcYBegin, pSrcUBegin, pSrcVBegin, nBandIndex, 4);
			break;
		}
		break;
	}
}
