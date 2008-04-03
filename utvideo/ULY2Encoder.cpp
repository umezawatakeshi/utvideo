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
#include "ULY2Encoder.h"
#include "utvideo.h"
#include "HuffmanCode.h"

CULY2Encoder::CULY2Encoder(void)
{
}

CULY2Encoder::~CULY2Encoder(void)
{
}

DWORD CULY2Encoder::Compress(ICCOMPRESS *icc, DWORD dwSize)
{
	CFrameBuffer *pCurFrame;
	FRAMEHEADER *pfh;
	BYTE *p;

	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');

	pCurFrame = new CFrameBuffer();
	pCurFrame->AddPlane(m_dwYPlaneSize, m_dwYPlaneStride); // Y
	pCurFrame->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // U
	pCurFrame->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // V
	pfh = (FRAMEHEADER *)icc->lpOutput;
	memset(pfh, 0, sizeof(FRAMEHEADER));
	p = (BYTE *)(pfh + 1);

	ConvertFromYUY2ToPlanar(pCurFrame, (BYTE *)icc->lpInput, m_dwFrameSize);
	p += EncodePlane(p, pCurFrame->GetPlane(0), pCurFrame->GetPlane(0) + m_dwYPlaneSize, m_dwYPlaneStride);
	p += EncodePlane(p, pCurFrame->GetPlane(1), pCurFrame->GetPlane(1) + m_dwCPlaneSize, m_dwCPlaneStride);
	p += EncodePlane(p, pCurFrame->GetPlane(2), pCurFrame->GetPlane(2) + m_dwCPlaneSize, m_dwCPlaneStride);
	memset(p, 0, 8);
	p += 8;

	icc->lpbiOutput->biSizeImage = p - ((BYTE *)icc->lpOutput);
	*icc->lpdwFlags = AVIIF_KEYFRAME;

	delete pCurFrame;

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressBegin(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	m_dwFrameStride = ROUNDUP(pbmihIn->biWidth, 2) * 2;
	m_dwFrameSize = m_dwFrameStride * pbmihIn->biHeight;

	m_dwYPlaneStride = ROUNDUP(pbmihIn->biWidth, 2);
	m_dwYPlaneSize = m_dwYPlaneStride * pbmihIn->biHeight;

	m_dwCPlaneStride = ROUNDUP(pbmihIn->biWidth, 2) / 2;
	m_dwCPlaneSize = m_dwCPlaneStride * pbmihIn->biHeight;

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressEnd(void)
{
	return ICERR_OK;
}

DWORD CULY2Encoder::CompressGetFormat(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	BITMAPINFOHEADER_EXTRA *pbmiheOut;

	if (pbmihOut == NULL)
		return sizeof(BITMAPINFOHEADER) + sizeof(BITMAPINFOHEADER_EXTRA);

	memset(pbmihOut, 0, sizeof(BITMAPINFOHEADER) + sizeof(BITMAPINFOHEADER_EXTRA));

	pbmihOut->biSize          = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPINFOHEADER_EXTRA);
	pbmihOut->biWidth         = pbmihIn->biWidth;
	pbmihOut->biHeight        = pbmihIn->biHeight;
	pbmihOut->biPlanes        = 1;
	pbmihOut->biBitCount      = 16;
	pbmihOut->biCompression   = FCC('ULY2');
	pbmihOut->biSizeImage     = pbmihIn->biSizeImage;
	//pbmihOut->biXPelsPerMeter
	//pbmihOut->biYPelsPerMeter
	//pbmihOut->biClrUsed
	//pbmihOut->biClrImportant

	pbmiheOut = (BITMAPINFOHEADER_EXTRA *)(pbmihOut + 1);
	pbmiheOut->dwEncoderVersion  = UTVIDEO_ENCODER_VERSION;
	pbmiheOut->fccOriginalFormat = pbmihIn->biCompression;
	pbmiheOut->wExtraSize        = sizeof(BITMAPINFOHEADER_EXTRA);
	pbmiheOut->wFrameHeaderSize  = sizeof(FRAMEHEADER);
	pbmiheOut->dwFlags0          = BMIHE_FLAGS0_COMPRESS_HUFFMAN_CODE;

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressGetSize(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	return pbmihIn->biSizeImage;
}

DWORD CULY2Encoder::CompressQuery(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	if (pbmihIn->biCompression == FCC('YUY2'))
		return ICERR_OK;
	else
		return ICERR_BADFORMAT;
}

void CULY2Encoder::ConvertFromYUY2ToPlanar(CFrameBuffer *pBuffer, const BYTE *pSrcBegin, DWORD dwFrameSize)
{
	BYTE *y, *u, *v;
	const BYTE *pSrcEnd, *p;

	pSrcEnd = pSrcBegin + dwFrameSize;
	y = pBuffer->GetPlane(0);
	u = pBuffer->GetPlane(1);
	v = pBuffer->GetPlane(2);

	for (p = pSrcBegin; p < pSrcEnd; p += 4)
	{
		*y++ = *p;
		*u++ = *(p+1);
		*y++ = *(p+2);
		*v++ = *(p+3);
	}
}

DWORD CULY2Encoder::EncodePlane(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwFrameStride)
{
	const BYTE *p;
	DWORD count[256];
	BYTE *pCodeLengthTable;
	HUFFMAN_ENCODE_TABLE het;
	DWORD cbEncoded;

	pCodeLengthTable = pDst;

	for (int i = 0; i < 256; i++)
		count[i] = 0;

	for (p = pSrcBegin; p < pSrcEnd; p++)
		count[*p]++;

	GenerateHuffmanCodeLengthTable(pCodeLengthTable, count);
	GenerateHuffmanEncodeTable(&het, pCodeLengthTable);
	cbEncoded = HuffmanEncode(pDst + 256 + 4, pSrcBegin, pSrcEnd, &het);
	*(DWORD *)(pDst + 256) = cbEncoded + 256 + 4;
	return cbEncoded + 256 + 4;
}
