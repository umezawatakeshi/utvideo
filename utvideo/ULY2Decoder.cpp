/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008  UMEZAWA Takeshi
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
 * Copyright (C) 2008  梅澤 威志
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
#include "ULY2Decoder.h"
#include "HuffmanCode.h"

CULY2Decoder::CULY2Decoder(void)
{
}

CULY2Decoder::~CULY2Decoder(void)
{
}

DWORD CULY2Decoder::Decompress(ICDECOMPRESS *icd, DWORD dwSize)
{
	CFrameBuffer *pCurFrame;
	FRAMEHEADER *pfh;
	const BYTE *p;

	pCurFrame = new CFrameBuffer();
	pCurFrame->AddPlane(m_dwYPlaneSize, m_dwYPlaneStride); // Y
	pCurFrame->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // U
	pCurFrame->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // V

	pfh = (FRAMEHEADER *)icd->lpInput;
	p = (BYTE *)(pfh + 1);

	p += DecodePlane(pCurFrame->GetPlane(0), pCurFrame->GetPlane(0) + m_dwYPlaneSize, p, m_dwYPlaneStride);
	p += DecodePlane(pCurFrame->GetPlane(1), pCurFrame->GetPlane(1) + m_dwCPlaneSize, p, m_dwCPlaneStride);
	p += DecodePlane(pCurFrame->GetPlane(2), pCurFrame->GetPlane(2) + m_dwCPlaneSize, p, m_dwCPlaneStride);

	ConvertFromPlanarToYUY2((BYTE *)icd->lpOutput, pCurFrame, m_dwFrameSize);

	icd->lpbiOutput->biSizeImage = m_dwFrameSize;

	delete pCurFrame;

	return ICERR_OK;
}

DWORD CULY2Decoder::DecompressBegin(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	m_dwFrameStride = ROUNDUP(pbihIn->biWidth, 2) * 2;
	m_dwFrameSize = m_dwFrameStride * pbihIn->biHeight;

	m_dwYPlaneStride = ROUNDUP(pbihIn->biWidth, 2);
	m_dwYPlaneSize = m_dwYPlaneStride * pbihIn->biHeight;

	m_dwCPlaneStride = ROUNDUP(pbihIn->biWidth, 2) / 2;
	m_dwCPlaneSize = m_dwCPlaneStride * pbihIn->biHeight;

	return ICERR_OK;
}

DWORD CULY2Decoder::DecompressEnd(void)
{
	return ICERR_OK;
}

DWORD CULY2Decoder::DecompressGetFormat(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER);

	memset(pbihOut, 0, sizeof(BITMAPINFOHEADER));

	pbihOut->biSize          = sizeof(BITMAPINFOHEADER);
	pbihOut->biWidth         = pbihIn->biWidth;
	pbihOut->biHeight        = pbihIn->biHeight;
	pbihOut->biPlanes        = 1;
	pbihOut->biBitCount      = 16;
	pbihOut->biCompression   = FCC('YUY2');
	pbihOut->biSizeImage     = pbihIn->biSizeImage;
	//pbihOut->biXPelsPerMeter
	//pbihOut->biYPelsPerMeter
	//pbihOut->biClrUsed
	//pbihOut->biClrImportant

	return ICERR_OK;
}

DWORD CULY2Decoder::DecompressQuery(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	if (pbihIn->biCompression != FCC('ULY2'))
		return ICERR_BADFORMAT;

	if (pbihOut != NULL && pbihOut->biCompression != FCC('YUY2'))
		return ICERR_BADFORMAT;

	if (pbihIn->biSize != sizeof(BITMAPINFOEXT))
		return ICERR_BADFORMAT;

	if (	(pbieIn->dwFlags0 & BIE_FLAGS0_RESERVED) ||
			(pbieIn->dwFlags1 & BIE_FLAGS1_RESERVED) ||
			(pbieIn->dwFlags2 & BIE_FLAGS2_RESERVED) ||
			(pbieIn->dwFlags3 & BIE_FLAGS3_RESERVED) ||
			(pbieIn->dwFlags4 & BIE_FLAGS4_RESERVED) ||
			(pbieIn->dwFlags5 & BIE_FLAGS5_RESERVED) ||
			(pbieIn->dwFlags6 & BIE_FLAGS6_RESERVED) ||
			(pbieIn->dwFlags7 & BIE_FLAGS7_RESERVED))
		return ICERR_BADFORMAT;

	return ICERR_OK;
}

void CULY2Decoder::ConvertFromPlanarToYUY2(BYTE *pDstBegin, CFrameBuffer *pBuffer, DWORD dwFrameSize)
{
	const BYTE *y, *u, *v;
	BYTE *pDstEnd, *p;

	pDstEnd = pDstBegin + dwFrameSize;
	y = pBuffer->GetPlane(0);
	u = pBuffer->GetPlane(1);
	v = pBuffer->GetPlane(2);

	for (p = pDstBegin; p < pDstEnd; p += 4)
	{
		*p     = *y++;
		*(p+1) = *u++;
		*(p+2) = *y++;
		*(p+3) = *v++;
	}
}

DWORD CULY2Decoder::DecodePlane(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, DWORD dwFrameStride)
{
	const BYTE *pCodeLengthTable;
	HUFFMAN_DECODE_TABLE hdt;
	DWORD cbEncoded;

	pCodeLengthTable = pSrcBegin;
	cbEncoded = *(DWORD *)(pSrcBegin + 256);

	GenerateHuffmanDecodeTable(&hdt, pCodeLengthTable);
	HuffmanDecode(pDstBegin, pDstEnd, pSrcBegin + 256 + 4, &hdt);
	return cbEncoded + 256 + 4;
}
