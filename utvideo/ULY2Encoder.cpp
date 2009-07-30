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
	m_dwPlaneSize[0]   = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]   = pbih->biWidth * pbih->biHeight / 2;
	m_dwPlaneSize[2]   = pbih->biWidth * pbih->biHeight / 2;

	m_dwPlaneWidth[0]  = pbih->biWidth;
	m_dwPlaneWidth[1]  = pbih->biWidth / 2;
	m_dwPlaneWidth[2]  = pbih->biWidth / 2;
}

void CULY2Encoder::ConvertToPlanar(DWORD nBandIndex)
{
	DWORD dwPlaneStrideBegin = m_dwNumStripes *  nBandIndex      / m_dwDivideCount;
	DWORD dwPlaneStrideEnd   = m_dwNumStripes * (nBandIndex + 1) / m_dwDivideCount;
	DWORD dwFrameStrideBegin, dwFrameStrideEnd;
	BYTE *y, *u, *v;
	const BYTE *pSrcBegin, *pSrcEnd, *p;

	if (!m_bBottomUpFrame)
	{
		dwFrameStrideBegin = dwPlaneStrideBegin;
		dwFrameStrideEnd   = dwPlaneStrideEnd;
	}
	else
	{
		dwFrameStrideBegin = m_dwNumStripes - dwPlaneStrideEnd;
		dwFrameStrideEnd   = m_dwNumStripes - dwPlaneStrideBegin;
	}

	pSrcBegin = ((BYTE *)m_icc->lpInput) + dwFrameStrideBegin * m_dwRawWidth;
	pSrcEnd   = ((BYTE *)m_icc->lpInput) + dwFrameStrideEnd   * m_dwRawWidth;
	y = m_pCurFrame->GetPlane(0) + dwPlaneStrideBegin * m_dwPlaneWidth[0];
	u = m_pCurFrame->GetPlane(1) + dwPlaneStrideBegin * m_dwPlaneWidth[1];
	v = m_pCurFrame->GetPlane(2) + dwPlaneStrideBegin * m_dwPlaneWidth[2];

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
			ConvertBottomupRGB24ToULY2(y, u, v, pSrcBegin, pSrcEnd, m_dwRawWidth, m_icc->lpbiOutput->biWidth * 3);
			break;
		case 32:
			ConvertBottomupRGB32ToULY2(y, u, v, pSrcBegin, pSrcEnd, m_dwRawWidth, m_dwRawWidth);
			break;
		}
		break;
	}
}
