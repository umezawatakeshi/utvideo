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
#include "ULRGDecoder.h"
#include "Predict.h"

const CPlanarDecoder::OUTPUTFORMAT CULRGDecoder::m_outfmts[] = {
	{ BI_RGB, 24 },
	{ BI_RGB, 32 },
};

CULRGDecoder::CULRGDecoder(void)
{
}

CULRGDecoder::~CULRGDecoder(void)
{
}

int CULRGDecoder::GetNumSupportedOutputFormats(void)
{
	return _countof(m_outfmts);
}

CDecoder *CULRGDecoder::CreateInstance(void)
{
	return new CULRGDecoder();
}

void CULRGDecoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
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

void CULRGDecoder::ConvertFromPlanar(DWORD nBandIndex)
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
			for (pStrideBegin = pDstEnd - m_dwRawWidth; pStrideBegin >= pDstBegin; pStrideBegin -= m_dwRawWidth)
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
			for (pStrideBegin = pDstEnd - m_dwRawWidth; pStrideBegin >= pDstBegin; pStrideBegin -= m_dwRawWidth)
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
