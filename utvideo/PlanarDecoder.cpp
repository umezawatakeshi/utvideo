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
#include "PlanarDecoder.h"
#include "Predict.h"

CPlanarDecoder::CPlanarDecoder(void)
{
}

CPlanarDecoder::~CPlanarDecoder(void)
{
}

DWORD CPlanarDecoder::Decompress(const ICDECOMPRESS *icd, DWORD dwSize)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)icd->lpbiInput;
	const BYTE *p;

	m_icd = icd;

	memset(&m_fi, 0, sizeof(FRAMEINFO));
	memcpy(&m_fi, ((BYTE *)icd->lpInput) + pbieIn->bih.biSizeImage - pbieIn->dwFrameInfoSize, pbieIn->dwFrameInfoSize);

	p = (BYTE *)icd->lpInput;
	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		m_pCodeLengthTable[nPlaneIndex] = p;
		GenerateHuffmanDecodeTable(&m_hdt[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		p += 256 + sizeof(DWORD) * m_dwDivideCount;
		p += ((const DWORD *)p)[-1];
	}

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CDecodeJob(this, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	icd->lpbiOutput->biSizeImage = m_dwFrameSize;

	return ICERR_OK;
}

DWORD CPlanarDecoder::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DWORD dwRet;
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	dwRet = DecompressQuery(pbihIn, pbihOut);
	if (dwRet != ICERR_OK)
		return dwRet;

	m_dwNumStrides = pbihIn->biHeight;
	m_dwDivideCount = ((pbieIn->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;

	_ASSERT(m_dwDivideCount >= 1 && m_dwDivideCount <= 256);
	_RPT1(_CRT_WARN, "divide count = %d\n", m_dwDivideCount);

	m_bBottomUpFrame = FALSE;
	switch (pbihOut->biCompression)
	{
	case BI_RGB:
		switch (pbihOut->biBitCount)
		{
		case 24:
			m_dwFrameStride = ROUNDUP(pbihOut->biWidth * 3, 4);
			break;
		case 32:
			m_dwFrameStride = pbihOut->biWidth * 4;
			break;
		}
		if (pbihIn->biHeight > 0)
			m_bBottomUpFrame = TRUE;
		break;
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
	case FCC('UYVY'):
	case FCC('UYNV'):
	case FCC('YVYU'):
	case FCC('VYUY'):
		m_dwFrameStride = ROUNDUP(pbihOut->biWidth, 2) * 2;
		break;
	default:
		return ICERR_BADFORMAT;
	}
	m_dwFrameSize = m_dwFrameStride * m_dwNumStrides;

	CalcPlaneSizes(pbihOut);

	m_pRestoredFrame = new CFrameBuffer();
	m_pRestoredFrame->AddPlane(m_dwPlaneSize[0], m_dwPlaneStride[0]);
	m_pRestoredFrame->AddPlane(m_dwPlaneSize[1], m_dwPlaneStride[1]);
	m_pRestoredFrame->AddPlane(m_dwPlaneSize[2], m_dwPlaneStride[2]);
	m_pDecodedFrame = new CFrameBuffer();
	m_pDecodedFrame->AddPlane(m_dwPlaneSize[0], m_dwPlaneStride[0]);
	m_pDecodedFrame->AddPlane(m_dwPlaneSize[1], m_dwPlaneStride[1]);
	m_pDecodedFrame->AddPlane(m_dwPlaneSize[2], m_dwPlaneStride[2]);

	m_ptm = new CThreadManager();

	return ICERR_OK;
}

DWORD CPlanarDecoder::DecompressEnd(void)
{
	delete m_pRestoredFrame;
	delete m_pDecodedFrame;

	delete m_ptm;

	return ICERR_OK;
}

DWORD CPlanarDecoder::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER);

	memset(pbihOut, 0, sizeof(BITMAPINFOHEADER));

	pbihOut->biSize          = sizeof(BITMAPINFOHEADER);
	pbihOut->biWidth         = pbihIn->biWidth;
	pbihOut->biHeight        = pbihIn->biHeight;
	pbihOut->biPlanes        = 1;
	pbihOut->biBitCount      = GetSupportedOutputFormats()[0].nBitCount;
	pbihOut->biCompression   = GetSupportedOutputFormats()[0].fcc;
	pbihOut->biSizeImage     = pbihIn->biSizeImage;
	//pbihOut->biXPelsPerMeter
	//pbihOut->biYPelsPerMeter
	//pbihOut->biClrUsed
	//pbihOut->biClrImportant

	return ICERR_OK;
}

DWORD CPlanarDecoder::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	const OUTPUTFORMAT *pfmts;

	if (pbihIn->biCompression != GetInputFCC())
		return ICERR_BADFORMAT;

	if (pbihIn->biSize > sizeof(BITMAPINFOEXT))
		return ICERR_BADFORMAT;
	if (pbieIn->dwFrameInfoSize > sizeof(FRAMEINFO))
		return ICERR_BADFORMAT;
	if (pbieIn->dwFlags0 & BIE_FLAGS0_RESERVED)
		return ICERR_BADFORMAT;

	if (pbihOut == NULL)
		return ICERR_OK;

	pfmts = GetSupportedOutputFormats();
	for (int i = 0; i < GetNumSupportedOutputFormats(); i++)
	{
		if (pbihOut->biCompression == pfmts[i].fcc && pbihOut->biBitCount == pfmts[i].nBitCount && (pfmts[i].bNegativeHeightAllowed || pbihOut->biHeight > 0) && abs(pbihOut->biHeight) == pbihIn->biHeight && pbihOut->biWidth == pbihIn->biWidth)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}

void CPlanarDecoder::DecodeProc(DWORD nBandIndex)
{
	DWORD dwStrideBegin = m_dwNumStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwStrideEnd   = m_dwNumStrides * (nBandIndex + 1) / m_dwDivideCount;

	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		DWORD dwPlaneBegin = dwStrideBegin * m_dwPlaneStride[nPlaneIndex];
		DWORD dwPlaneEnd   = dwStrideEnd   * m_dwPlaneStride[nPlaneIndex];
		DWORD dwDstOffset;
		if (nBandIndex == 0)
			dwDstOffset = 0;
		else
			dwDstOffset = ((const DWORD *)(m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex - 1];

		if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) == FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
			HuffmanDecodeAndAccum(m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_pCodeLengthTable[nPlaneIndex] + 256 + sizeof(DWORD) * m_dwDivideCount + dwDstOffset, &m_hdt[nPlaneIndex]);
		else
			HuffmanDecode(m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_pCodeLengthTable[nPlaneIndex] + 256 + sizeof(DWORD) * m_dwDivideCount + dwDstOffset, &m_hdt[nPlaneIndex]);

		switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
		case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			m_pCurFrame = m_pDecodedFrame;
			break;
		case FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
			RestoreMedian(m_pRestoredFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_dwPlaneStride[nPlaneIndex]);
			m_pCurFrame = m_pRestoredFrame;
			break;
		}
	}

	ConvertFromPlanar(nBandIndex);
}
