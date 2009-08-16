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
#include "ULY0Encoder.h"
#include "Predict.h"
#include "resource.h"

const CULY0Encoder::INPUTFORMAT CULY0Encoder::m_infmts[] = {
	{ FCC('YV12'), 12 },
	{ FCC('YUY2'), 16 }, { FCC('YUYV'), 16 }, { FCC('YUNV'), 16 },
	{ FCC('UYVY'), 16 }, { FCC('UYNV'), 16 },
	{ FCC('YVYU'), 16 },
	{ FCC('VYUY'), 16 },
	{ BI_RGB, 32 },
	{ BI_RGB, 24 },
};

CULY0Encoder::CULY0Encoder(void)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));
	m_ec.dwFlags0 = (CThreadManager::GetNumProcessors() - 1) | EC_FLAGS0_INTRAFRAME_PREDICT_LEFT;
}

CULY0Encoder::~CULY0Encoder(void)
{
}

int CULY0Encoder::GetNumSupportedInputFormats(void)
{
	return _countof(m_infmts);
}

CEncoder *CULY0Encoder::CreateInstance(void)
{
	return new CULY0Encoder();
}

void CULY0Encoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
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

void CULY0Encoder::ConvertBottomupRGBToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD bypp)
{
	BYTE *y = pDstYBegin;
	BYTE *u = pDstUBegin;
	BYTE *v = pDstVBegin;

	DWORD dwDataStride = m_icc->lpbiInput->biWidth * bypp;
	DWORD dwSrcStride = ROUNDUP(dwDataStride, 4);
	DWORD dwYStride = m_icc->lpbiInput->biWidth; // = m_dwPlaneWidth[0]

	const BYTE *pSrcBegin = ((BYTE *)m_icc->lpInput) + (m_dwNumStripes - m_dwPlaneStripeEnd[nBandIndex]  ) * m_dwRawStripeSize;
	const BYTE *pSrcEnd   = ((BYTE *)m_icc->lpInput) + (m_dwNumStripes - m_dwPlaneStripeBegin[nBandIndex]) * m_dwRawStripeSize;

	for (const BYTE *pStrideBegin = pSrcEnd - dwSrcStride * 2; pStrideBegin >= pSrcBegin; pStrideBegin -= dwSrcStride * 2)
	{
		const BYTE *pStrideEnd = pStrideBegin + dwDataStride;
		for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			const BYTE *q = p + dwSrcStride;
			*(y+0)           = min(max(int((*(q     +0))*0.098 + (*(q     +1))*0.504 + (*(q     +2))*0.257 + 16.5), 16), 235);
			*(y+1)           = min(max(int((*(q+bypp+0))*0.098 + (*(q+bypp+1))*0.504 + (*(q+bypp+2))*0.257 + 16.5), 16), 235);
			*(y+dwYStride+0) = min(max(int((*(p     +0))*0.098 + (*(p     +1))*0.504 + (*(p     +2))*0.257 + 16.5), 16), 235);
			*(y+dwYStride+1) = min(max(int((*(p+bypp+0))*0.098 + (*(p+bypp+1))*0.504 + (*(p+bypp+2))*0.257 + 16.5), 16), 235);
			*u               = min(max(int(((*(p+0)+*(p+bypp+0)+*(q+0)+*(q+bypp+0))*0.439 + (*(p+1)+*(p+bypp+1)+*(q+1)+*(q+bypp+1))*-0.291 + (*(p+2)+*(p+bypp+2)+*(q+2)+*(q+bypp+2))*-0.148)/4 + 128.5), 16), 240);
			*v               = min(max(int(((*(p+0)+*(p+bypp+0)+*(q+0)+*(q+bypp+0))*-0.071 + (*(p+1)+*(p+bypp+1)+*(q+1)+*(q+bypp+1))*-0.368 + (*(p+2)+*(p+bypp+2)+*(q+2)+*(q+bypp+2))*0.439)/4 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
		y += dwYStride;
	}
}

void CULY0Encoder::ConvertYUV422ToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD nYOffset)
{
	BYTE *y = pDstYBegin;
	BYTE *u = pDstUBegin;
	BYTE *v = pDstVBegin;

	DWORD dwSrcStride = m_icc->lpbiInput->biWidth * 2;
	DWORD dwYStride = m_icc->lpbiInput->biWidth; // = m_dwPlaneWidth[0]

	const BYTE *pSrcBegin = ((BYTE *)m_icc->lpInput) + m_dwPlaneStripeBegin[nBandIndex] * m_dwRawStripeSize;
	const BYTE *pSrcEnd   = ((BYTE *)m_icc->lpInput) + m_dwPlaneStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	for (const BYTE *pStrideBegin = pSrcBegin; pStrideBegin < pSrcEnd; pStrideBegin += dwSrcStride * 2)
	{
		const BYTE *pStrideEnd = pStrideBegin + dwSrcStride;
		for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			const BYTE *q = p + dwSrcStride;
			*(y+0) = *(p+0+nYOffset);
			*(y+1) = *(p+2+nYOffset);
			*(y+dwYStride+0) = *(q+0+nYOffset);
			*(y+dwYStride+1) = *(q+2+nYOffset);
			*u = (*(p+1-nYOffset) + *(q+1-nYOffset)) / 2;
			*v = (*(p+3-nYOffset) + *(q+3-nYOffset)) / 2;

			y+=2; u++; v++;
		}
		y += dwYStride;
	}
}

void CULY0Encoder::ConvertToPlanar(DWORD nBandIndex)
{
	BYTE *pDstYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	BYTE *pDstUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	BYTE *pDstVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_icc->lpbiInput->biCompression)
	{
	case FCC('YV12'):
		{
			const BYTE *pSrcYBegin, *pSrcVBegin, *pSrcUBegin;

			pSrcYBegin = ((BYTE *)m_icc->lpInput);
			pSrcVBegin = pSrcYBegin + m_icc->lpbiInput->biWidth * m_icc->lpbiInput->biHeight;
			pSrcUBegin = pSrcVBegin + m_icc->lpbiInput->biWidth * m_icc->lpbiInput->biHeight / 4;

			pSrcYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pSrcVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pSrcUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[2]);
		}
		break;
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
		ConvertYUV422ToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 0);
		break;
	case FCC('UYVY'):
	case FCC('UYNV'):
		ConvertYUV422ToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 1);
		break;
	case FCC('YVYU'):
		ConvertYUV422ToULY0(pDstYBegin, pDstVBegin, pDstUBegin, nBandIndex, 0);
		break;
	case FCC('VYUY'):
		ConvertYUV422ToULY0(pDstYBegin, pDstVBegin, pDstUBegin, nBandIndex, 1);
		break;
	case BI_RGB:
		switch (m_icc->lpbiInput->biBitCount)
		{
		case 24:
			ConvertBottomupRGBToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 3);
			break;
		case 32:
			ConvertBottomupRGBToULY0(pDstYBegin, pDstUBegin, pDstVBegin, nBandIndex, 4);
			break;
		}
		break;
	}
}
