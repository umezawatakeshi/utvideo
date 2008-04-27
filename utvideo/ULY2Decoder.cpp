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
#include "Predict.h"

CULY2Decoder::CULY2Decoder(void)
{
}

CULY2Decoder::~CULY2Decoder(void)
{
}

DWORD CULY2Decoder::Decompress(const ICDECOMPRESS *icd, DWORD dwSize)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)icd->lpbiInput;
	CFrameBuffer *pCurFrame;
	CFrameBuffer *pDecodedFrame;
	FRAMEINFO fi;
	const BYTE *p;
	HUFFMAN_DECODE_TABLE hdt[3];
	const BYTE *pCodeLengthTable[3];

	pDecodedFrame = new CFrameBuffer();
	pDecodedFrame->AddPlane(m_dwPlaneSize[0], m_dwPlaneStride[0]);
	pDecodedFrame->AddPlane(m_dwPlaneSize[1], m_dwPlaneStride[1]);
	pDecodedFrame->AddPlane(m_dwPlaneSize[2], m_dwPlaneStride[2]);

	memset(&fi, 0, sizeof(FRAMEINFO));
	memcpy(&fi, ((BYTE *)icd->lpInput) + pbieIn->bih.biSizeImage - pbieIn->dwFrameInfoSize, pbieIn->dwFrameInfoSize);

	switch (fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
		pCurFrame = pDecodedFrame;
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
		pCurFrame = new CFrameBuffer();
		pCurFrame->AddPlane(m_dwPlaneSize[0], m_dwPlaneStride[0]);
		pCurFrame->AddPlane(m_dwPlaneSize[1], m_dwPlaneStride[1]);
		pCurFrame->AddPlane(m_dwPlaneSize[2], m_dwPlaneStride[2]);
		break;
	default:
		return ICERR_ABORT;
	}

	p = (BYTE *)icd->lpInput;
	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		pCodeLengthTable[nPlaneIndex] = p;
		GenerateHuffmanDecodeTable(&hdt[nPlaneIndex], pCodeLengthTable[nPlaneIndex]);
		p += 256 + sizeof(DWORD) * m_dwDivideCount;
		p += ((const DWORD *)p)[-1];
	}

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
	{
		DWORD dwStrideBegin = m_dwNumStrides *  nBandIndex      / m_dwDivideCount;
		DWORD dwStrideEnd   = m_dwNumStrides * (nBandIndex + 1) / m_dwDivideCount;

		const BYTE *y, *u, *v;
		BYTE *pDstBegin, *pDstEnd, *p;

		for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
		{
			DWORD dwPlaneBegin = dwStrideBegin * m_dwPlaneStride[nPlaneIndex];
			DWORD dwPlaneEnd   = dwStrideEnd   * m_dwPlaneStride[nPlaneIndex];
			DWORD dwDstOffset;
			if (nBandIndex == 0)
				dwDstOffset = 0;
			else
				dwDstOffset = ((const DWORD *)(pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex - 1];
			HuffmanDecode(pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, pCodeLengthTable[nPlaneIndex] + 256 + sizeof(DWORD) * m_dwDivideCount + dwDstOffset, &hdt[nPlaneIndex]);

			switch (fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
			{
			case FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
				RestoreMedian(pCurFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_dwPlaneStride[nPlaneIndex]);
				break;
			}
		}

		pDstBegin = ((BYTE *)icd->lpOutput) + dwStrideBegin * m_dwFrameStride;
		pDstEnd   = ((BYTE *)icd->lpOutput) + dwStrideEnd   * m_dwFrameStride;
		y = pCurFrame->GetPlane(0) + dwStrideBegin * m_dwPlaneStride[0];
		u = pCurFrame->GetPlane(1) + dwStrideBegin * m_dwPlaneStride[1];
		v = pCurFrame->GetPlane(2) + dwStrideBegin * m_dwPlaneStride[2];

		for (p = pDstBegin; p < pDstEnd; p += 4)
		{
			*p     = *y++;
			*(p+1) = *u++;
			*(p+2) = *y++;
			*(p+3) = *v++;
		}
	}
	//ConvertFromPlanarToYUY2((BYTE *)icd->lpOutput, pCurFrame, m_dwFrameSize);

	icd->lpbiOutput->biSizeImage = m_dwFrameSize;

	switch (fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
		delete pDecodedFrame;
		break;
	}
	delete pCurFrame;

	return ICERR_OK;
}

DWORD CULY2Decoder::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	m_dwNumStrides = pbihIn->biHeight;
	m_dwDivideCount = ((pbieIn->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;

	_ASSERT(m_dwDivideCount >= 1 && m_dwDivideCount <= 256);
	_RPT1(_CRT_WARN, "divide count = %d\n", m_dwDivideCount);

	m_dwFrameStride = ROUNDUP(pbihIn->biWidth, 2) * 2;
	m_dwFrameSize = m_dwFrameStride * pbihIn->biHeight;

	m_dwPlaneStride[0] = ROUNDUP(pbihIn->biWidth, 2);
	m_dwPlaneSize[0]   = m_dwPlaneStride[0] * pbihIn->biHeight;

	m_dwPlaneStride[1] = ROUNDUP(pbihIn->biWidth, 2) / 2;
	m_dwPlaneSize[1]   = m_dwPlaneStride[1] * pbihIn->biHeight;

	m_dwPlaneStride[2] = m_dwPlaneStride[1];
	m_dwPlaneSize[2]   = m_dwPlaneSize[1];

	return ICERR_OK;
}

DWORD CULY2Decoder::DecompressEnd(void)
{
	return ICERR_OK;
}

DWORD CULY2Decoder::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
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

DWORD CULY2Decoder::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	if (pbihIn->biCompression != FCC('ULY2'))
		return ICERR_BADFORMAT;

	if (pbihOut != NULL && pbihOut->biCompression != FCC('YUY2'))
		return ICERR_BADFORMAT;

	if (pbihIn->biSize > sizeof(BITMAPINFOEXT))
		return ICERR_BADFORMAT;
	if (pbieIn->dwFrameInfoSize > sizeof(FRAMEINFO))
		return ICERR_BADFORMAT;
	if (pbieIn->dwFlags0 & BIE_FLAGS0_RESERVED)
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
