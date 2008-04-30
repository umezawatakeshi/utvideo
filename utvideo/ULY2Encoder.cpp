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
#include "ULY2Encoder.h"
#include "Predict.h"
#include "resource.h"

CULY2Encoder::CULY2Encoder(void)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));
}

CULY2Encoder::~CULY2Encoder(void)
{
}

DWORD CULY2Encoder::Configure(HWND hwnd)
{
	DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_CONFIG_DIALOG), hwnd, DialogProc, (LPARAM)this);
	return ICERR_OK;
}

int CALLBACK CULY2Encoder::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CULY2Encoder *pThis = (CULY2Encoder *)GetWindowLong(hwnd, DWL_USER);
	char buf[256];
	int	n;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLong(hwnd, DWL_USER, lParam);
		pThis = (CULY2Encoder *)lParam;
		wsprintf(buf, "%d", (pThis->m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_MASK) + 1);
		SetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf, sizeof(buf));
			n = atoi(buf);
			if (n < 1 || n > 256)
			{
				MessageBox(hwnd, "1 <= DIVIDE_COUNT <= 256", "ERR", MB_ICONERROR);
				return TRUE;
			}
			pThis->m_ec.dwFlags0 = (n - 1) & EC_FLAGS0_DIVIDE_COUNT_MASK;
			/* FALLTHROUGH */
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

DWORD CULY2Encoder::GetState(void *pState, DWORD dwSize)
{
	if (dwSize < sizeof(ENCODERCONF))
		return ICERR_BADSIZE;

	memcpy(pState, &m_ec, sizeof(ENCODERCONF));
	return ICERR_OK;
}

DWORD CULY2Encoder::SetState(const void *pState, DWORD dwSize)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));

	memcpy(&m_ec, pState, min(sizeof(ENCODERCONF), dwSize));
	m_ec.dwFlags0 &= ~EC_FLAGS0_RESERVED;

	return min(sizeof(ENCODERCONF), dwSize);
}

DWORD CULY2Encoder::Compress(const ICCOMPRESS *icc, DWORD dwSize)
{
	FRAMEINFO fi;
	BYTE *p;
	DWORD count[256];

	m_icc = icc;

	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');

	m_pCurFrame = new CFrameBuffer();
	m_pCurFrame->AddPlane(m_dwPlaneSize[0], m_dwPlaneStride[0]);
	m_pCurFrame->AddPlane(m_dwPlaneSize[1], m_dwPlaneStride[1]);
	m_pCurFrame->AddPlane(m_dwPlaneSize[2], m_dwPlaneStride[2]);

	m_pMedianPredicted = new CFrameBuffer();
	m_pMedianPredicted->AddPlane(m_dwPlaneSize[0], m_dwPlaneStride[0]);
	m_pMedianPredicted->AddPlane(m_dwPlaneSize[1], m_dwPlaneStride[1]);
	m_pMedianPredicted->AddPlane(m_dwPlaneSize[2], m_dwPlaneStride[2]);

	memset(&fi, 0, sizeof(FRAMEINFO));

	m_counts = (COUNTS *)malloc(sizeof(COUNTS) * m_dwDivideCount);

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_tm.SubmitJob(new CPredictJob(this, nBandIndex), nBandIndex);
	m_tm.WaitForJobCompletion();
	fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;

	p = (BYTE *)icc->lpOutput;

	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		DWORD dwCurrentOffset;
		for (int i = 0; i < 256; i++)
			count[i] = 0;
		for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
			for (int i = 0; i < 256; i++)
				count[i] += m_counts[nBandIndex].dwCount[nPlaneIndex][i];
		m_pCodeLengthTable[nPlaneIndex] = p;
		GenerateHuffmanCodeLengthTable(m_pCodeLengthTable[nPlaneIndex], count);
		GenerateHuffmanEncodeTable(&m_het[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		p += 256;
		dwCurrentOffset = 0;
		for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		{
			DWORD dwBits;
			dwBits = 0;
			for (int i = 0; i < 256; i++)
				dwBits += m_pCodeLengthTable[nPlaneIndex][i] * m_counts[nBandIndex].dwCount[nPlaneIndex][i];
			dwCurrentOffset += ROUNDUP(dwBits, 32) / 8;
			*(DWORD *)p = dwCurrentOffset;
			p += 4;
		}
		p += dwCurrentOffset;
	}

	memcpy(p, &fi, sizeof(FRAMEINFO));
	p += sizeof(FRAMEINFO);

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_tm.SubmitJob(new CEncodeJob(this, nBandIndex), nBandIndex);
	m_tm.WaitForJobCompletion();

	icc->lpbiOutput->biSizeImage = p - ((BYTE *)icc->lpOutput);
	*icc->lpdwFlags = AVIIF_KEYFRAME;

	free(m_counts);

	delete m_pCurFrame;
	delete m_pMedianPredicted;

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;

	m_dwNumStrides = pbihIn->biHeight;
	m_dwDivideCount = ((pbieOut->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;

	_ASSERT(m_dwDivideCount >= 1 && m_dwDivideCount <= 256);

	m_dwFrameStride = ROUNDUP(pbihIn->biWidth, 2) * 2;
	m_dwFrameSize = m_dwFrameStride * m_dwNumStrides;

	m_dwPlaneStride[0] = ROUNDUP(pbihIn->biWidth, 2);
	m_dwPlaneSize[0]   = m_dwPlaneStride[0] * m_dwNumStrides;

	m_dwPlaneStride[1] = ROUNDUP(pbihIn->biWidth, 2) / 2;
	m_dwPlaneSize[1]   = m_dwPlaneStride[1] * m_dwNumStrides;

	m_dwPlaneStride[2] = m_dwPlaneStride[1];
	m_dwPlaneSize[2]   = m_dwPlaneSize[1];

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressEnd(void)
{
	return ICERR_OK;
}

DWORD CULY2Encoder::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;
	DWORD dwDivideCount;

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOEXT);

	memset(pbihOut, 0, sizeof(BITMAPINFOEXT));

	dwDivideCount = min(ROUNDUP(pbihIn->biHeight, 2) / 2, (m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_MASK) + 1);

	_ASSERT(dwDivideCount >= 1 && dwDivideCount <= 256);

	pbieOut->bih.biSize          = sizeof(BITMAPINFOEXT);
	pbieOut->bih.biWidth         = pbihIn->biWidth;
	pbieOut->bih.biHeight        = pbihIn->biHeight;
	pbieOut->bih.biPlanes        = 1;
	pbieOut->bih.biBitCount      = 16;
	pbieOut->bih.biCompression   = FCC('ULY2');
	pbieOut->bih.biSizeImage     = pbihIn->biSizeImage;
	//pbieOut->bih.biXPelsPerMeter
	//pbieOut->bih.biYPelsPerMeter
	//pbieOut->bih.biClrUsed
	//pbieOut->bih.biClrImportant
	pbieOut->dwEncoderVersion  = UTVIDEO_ENCODER_VERSION;
	pbieOut->fccOriginalFormat = pbihIn->biCompression;
	pbieOut->dwFrameInfoSize   = sizeof(FRAMEINFO);
	pbieOut->dwFlags0          = BIE_FLAGS0_COMPRESS_HUFFMAN_CODE | ((dwDivideCount - 1) << BIE_FLAGS0_DIVIDE_COUNT_SHIFT);

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return pbihIn->biSizeImage + 2048; // +2048 はどんぶり勘定。
}

DWORD CULY2Encoder::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	if (pbihIn->biCompression == FCC('YUY2'))
		return ICERR_OK;
	else
		return ICERR_BADFORMAT;
}

void CULY2Encoder::PredictProc(DWORD nBandIndex)
{
	DWORD dwStrideBegin = m_dwNumStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwStrideEnd   = m_dwNumStrides * (nBandIndex + 1) / m_dwDivideCount;
	BYTE *y, *u, *v;
	const BYTE *pSrcBegin, *pSrcEnd, *p;

	pSrcBegin = ((BYTE *)m_icc->lpInput) + dwStrideBegin * m_dwFrameStride;
	pSrcEnd   = ((BYTE *)m_icc->lpInput) + dwStrideEnd   * m_dwFrameStride;
	y = m_pCurFrame->GetPlane(0) + dwStrideBegin * m_dwPlaneStride[0];
	u = m_pCurFrame->GetPlane(1) + dwStrideBegin * m_dwPlaneStride[1];
	v = m_pCurFrame->GetPlane(2) + dwStrideBegin * m_dwPlaneStride[2];

	for (p = pSrcBegin; p < pSrcEnd; p += 4)
	{
		*y++ = *p;
		*u++ = *(p+1);
		*y++ = *(p+2);
		*v++ = *(p+3);
	}

	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		DWORD dwPlaneBegin = dwStrideBegin * m_dwPlaneStride[nPlaneIndex];
		DWORD dwPlaneEnd   = dwStrideEnd   * m_dwPlaneStride[nPlaneIndex];

		PredictMedian(m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_dwPlaneStride[nPlaneIndex]);

		for (int i = 0; i < 256; i++)
			m_counts[nBandIndex].dwCount[nPlaneIndex][i] = 0;

		for (p = m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin; p < m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneEnd; p++)
			m_counts[nBandIndex].dwCount[nPlaneIndex][*p]++;
	}
}

void CULY2Encoder::EncodeProc(DWORD nBandIndex)
{
	DWORD dwStrideBegin = m_dwNumStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwStrideEnd   = m_dwNumStrides * (nBandIndex + 1) / m_dwDivideCount;
	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		DWORD dwPlaneBegin = dwStrideBegin * m_dwPlaneStride[nPlaneIndex];
		DWORD dwPlaneEnd   = dwStrideEnd   * m_dwPlaneStride[nPlaneIndex];
		DWORD dwDstOffset;
#ifdef _DEBUG
		DWORD dwDstEnd;
		DWORD dwEncodedSize;
#endif
		if (nBandIndex == 0)
			dwDstOffset = 0;
		else
			dwDstOffset = ((DWORD *)(m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex - 1];
#ifdef _DEBUG
		dwDstEnd = ((DWORD *)(m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex];
		dwEncodedSize =
#endif
		HuffmanEncode(m_pCodeLengthTable[nPlaneIndex] + 256 + sizeof(DWORD) * m_dwDivideCount + dwDstOffset, m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneEnd, &m_het[nPlaneIndex]);
		_ASSERT(dwEncodedSize == dwDstEnd - dwDstOffset);
	}
}
