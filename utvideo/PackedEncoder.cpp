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
#include "PackedEncoder.h"
#include "Predict.h"
#include "HuffmanTable.h"
#include "resource.h"

CPackedEncoder::CPackedEncoder(void)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));
	m_ec.dwFlags0 = (CThreadManager::GetNumProcessors() - 1) | EC_FLAGS0_QUICKMODE;
}

CPackedEncoder::~CPackedEncoder(void)
{
}

DWORD CPackedEncoder::Configure(HWND hwnd)
{
	DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_CONFIG_DIALOG2), hwnd, DialogProc, (LPARAM)this);
	return ICERR_OK;
}

int CALLBACK CPackedEncoder::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPackedEncoder *pThis = (CPackedEncoder *)GetWindowLong(hwnd, DWL_USER);
	char buf[256];
	int	n;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLong(hwnd, DWL_USER, lParam);
		pThis = (CPackedEncoder *)lParam;
		wsprintf(buf, "Ut Video Codec %s (%c%c%c%c) VCM の設定", pThis->GetColorFormatName(), FCC4PRINTF(pThis->GetOutputFCC()));
		SetWindowText(hwnd, buf);
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
			memset(&pThis->m_ec, 0, sizeof(ENCODERCONF));
			pThis->m_ec.dwFlags0 |= (n - 1) & EC_FLAGS0_DIVIDE_COUNT_MASK;
			/* FALLTHROUGH */
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

DWORD CPackedEncoder::GetState(void *pState, DWORD dwSize)
{
	if (dwSize < sizeof(ENCODERCONF))
		return ICERR_BADSIZE;

	memcpy(pState, &m_ec, sizeof(ENCODERCONF));
	return ICERR_OK;
}

DWORD CPackedEncoder::SetState(const void *pState, DWORD dwSize)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));

	memcpy(&m_ec, pState, min(sizeof(ENCODERCONF), dwSize));
	m_ec.dwFlags0 &= ~EC_FLAGS0_RESERVED;
	if ((m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK) == EC_FLAGS0_INTRAFRAME_PREDICT_RESERVED)
		m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;

	return min(sizeof(ENCODERCONF), dwSize);
}

DWORD CPackedEncoder::Compress(const ICCOMPRESS *icc, DWORD dwSize)
{
	FRAMEINFO fi;
	BYTE *p;

	m_icc = icc;

	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');

	memset(&fi, 0, sizeof(FRAMEINFO));
	fi.dwFlags0 = FI_FLAGS0_INTRAFRAME_PREDICT_LEFT;

	p = (BYTE *)icc->lpOutput;

	memcpy(p, StaticHuffmanCodeLengthTableYUV, sizeof(StaticHuffmanCodeLengthTableYUV));
	memcpy(m_het, GetStaticHuffmanEncodeTableYUV(), sizeof(HUFFMAN_ENCODE_TABLE) * 3);

	p += sizeof(StaticHuffmanCodeLengthTableYUV);
	m_pdwTailOffsetTable = (DWORD *)p;
	p += sizeof(DWORD) * m_dwDivideCount;
	m_pEncodedDataBase = p;

	for (DWORD nStripIndex = 0; nStripIndex < m_dwDivideCount; nStripIndex++)
		m_ptm->SubmitJob(new CEncodeJob(this, nStripIndex), nStripIndex);
	m_ptm->WaitForJobCompletion();
	p += m_pdwTailOffsetTable[m_dwDivideCount - 1];

	memcpy(p, &fi, sizeof(FRAMEINFO));
	p += sizeof(FRAMEINFO);

	icc->lpbiOutput->biSizeImage = p - ((BYTE *)icc->lpOutput);
	*icc->lpdwFlags = AVIIF_KEYFRAME;

	return ICERR_OK;
}

DWORD CPackedEncoder::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;
	DWORD dwRet;

	dwRet = CompressQuery(pbihIn, pbihOut);
	if (dwRet != ICERR_OK)
		return dwRet;

	m_dwDivideCount = ((pbieOut->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;

	m_pEncodeBuffer = new CFrameBuffer();
	m_pEncodeBuffer->AddPlane(1, 1);
	for (unsigned int i = 1; i < m_dwDivideCount; i++)
		m_pEncodeBuffer->AddPlane(pbihIn->biWidth * abs(pbihIn->biHeight) * GetOutputBitCount() / 8 * 3, 1);	// XXX

	m_phEncodeCompletionEvent = new HANDLE[m_dwDivideCount];
	for (unsigned int i = 0; i < m_dwDivideCount; i++)
		m_phEncodeCompletionEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);	// auto-reset, initialy non-signaled

	m_ptm = new CThreadManager();

	SetCompressionProperty(pbihIn, pbihOut);

	return ICERR_OK;
}

DWORD CPackedEncoder::CompressEnd(void)
{
	for (unsigned int i = 0; i < m_dwDivideCount; i++)
		CloseHandle(m_phEncodeCompletionEvent[i]);
	delete m_phEncodeCompletionEvent;
	delete m_pEncodeBuffer;
	delete m_ptm;

	return ICERR_OK;
}

DWORD CPackedEncoder::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;
	DWORD dwDivideCount;
	const INPUTFORMAT *pifmt;
	DWORD dwRet;

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOEXT);

	dwRet = CompressQuery(pbihIn, NULL);
	if (dwRet != ICERR_OK)
		return dwRet;

	pifmt = LookupInputFormat(pbihIn);
	_ASSERT(pifmt != NULL);

	memset(pbihOut, 0, sizeof(BITMAPINFOEXT));

	dwDivideCount = min(ROUNDUP(abs(pbihIn->biHeight), 2) / 2, (m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_MASK) + 1);

	_ASSERT(dwDivideCount >= 1 && dwDivideCount <= 256);

	pbieOut->bih.biSize          = sizeof(BITMAPINFOEXT);
	pbieOut->bih.biWidth         = pbihIn->biWidth;
	pbieOut->bih.biHeight        = abs(pbihIn->biHeight);
	pbieOut->bih.biPlanes        = 1;
	pbieOut->bih.biBitCount      = pifmt->nEffectiveBitCount;
	pbieOut->bih.biCompression   = GetOutputFCC();
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

DWORD CPackedEncoder::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return pbihIn->biWidth * abs(pbihIn->biHeight) * GetOutputBitCount() / 8 * 3 + 4096; // +4096 はどんぶり勘定。
}

DWORD CPackedEncoder::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	if (pbihIn->biWidth <= 0)
		return ICERR_BADFORMAT;
	if (pbihIn->biHeight <= 0)
		return ICERR_BADFORMAT;

	if (pbihIn->biWidth % 16 != 0)
		return ICERR_BADFORMAT;
	if (pbihIn->biHeight % 8 != 0)
		return ICERR_BADFORMAT;

	if (LookupInputFormat(pbihIn) != NULL)
		return ICERR_OK;
	else
		return ICERR_BADFORMAT;
}

const CPackedEncoder::INPUTFORMAT *CPackedEncoder::LookupInputFormat(const BITMAPINFOHEADER *pbihIn)
{
	const INPUTFORMAT *pfmts;

	pfmts = GetSupportedInputFormats();
	for (int i = 0; i < GetNumSupportedInputFormats(); i++)
	{
		if (pbihIn->biCompression == pfmts[i].fcc && pbihIn->biBitCount == pfmts[i].nBitCount)
		{
			if (pbihIn->biHeight > 0 && pfmts[i].bPositiveHeightAllowed)
				return &pfmts[i];
			if (pbihIn->biHeight < 0 && pfmts[i].bNegativeHeightAllowed)
				return &pfmts[i];
		}
	}

	return NULL;
}

void CPackedEncoder::EncodeProc(DWORD nStripIndex)
{
	DWORD dwNumStrideBegin = m_dwNumStrides *  nStripIndex      / m_dwDivideCount;
	DWORD dwNumStrideEnd   = m_dwNumStrides * (nStripIndex + 1) / m_dwDivideCount;
	BYTE *pTmpDst;
	DWORD dwEncodedSize;

	if (nStripIndex == 0)
		pTmpDst = m_pEncodedDataBase;
	else
		pTmpDst = m_pEncodeBuffer->GetPlane(nStripIndex);

	dwEncodedSize = m_pfnHuffmanEncodeFirstRawWithDiff(
		pTmpDst,
		((BYTE *)m_icc->lpInput) + m_dwStrideSize * dwNumStrideBegin,
		((BYTE *)m_icc->lpInput) + m_dwStrideSize * dwNumStrideEnd,
		m_het);

	if (nStripIndex == 0)
		m_pdwTailOffsetTable[0] = dwEncodedSize;
	else
	{
		WaitForSingleObject(m_phEncodeCompletionEvent[nStripIndex - 1], INFINITE);
		memcpy(m_pEncodedDataBase + m_pdwTailOffsetTable[nStripIndex - 1], pTmpDst, dwEncodedSize);
		m_pdwTailOffsetTable[nStripIndex] = m_pdwTailOffsetTable[nStripIndex - 1] + dwEncodedSize;
	}

	SetEvent(m_phEncodeCompletionEvent[nStripIndex]);
}
