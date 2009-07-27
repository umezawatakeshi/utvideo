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

DWORD CULY0Encoder::Configure(HWND hwnd)
{
	DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_CONFIG_DIALOG), hwnd, DialogProc, (LPARAM)this);
	return ICERR_OK;
}

int CALLBACK CULY0Encoder::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CULY0Encoder *pThis = (CULY0Encoder *)GetWindowLong(hwnd, DWL_USER);
	char buf[256];
	int	n;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLong(hwnd, DWL_USER, lParam);
		pThis = (CULY0Encoder *)lParam;
		wsprintf(buf, "Ut Video Codec %s (%c%c%c%c) VCM の設定", pThis->GetColorFormatName(), FCC4PRINTF(pThis->GetOutputFCC()));
		SetWindowText(hwnd, buf);
		wsprintf(buf, "%d", (pThis->m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_MASK) + 1);
		SetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf);
		switch (pThis->m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		default:
			_ASSERT(false);
			/* FALLTHROUGH */
		case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_LEFT_RADIO, BST_CHECKED);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_MEDIAN_RADIO, BST_CHECKED);
			break;
		}
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
			if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_LEFT_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_LEFT;
			else if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_MEDIAN_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;
			/* FALLTHROUGH */
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

DWORD CULY0Encoder::GetState(void *pState, DWORD dwSize)
{
	if (dwSize < sizeof(ENCODERCONF))
		return ICERR_BADSIZE;

	memcpy(pState, &m_ec, sizeof(ENCODERCONF));
	return ICERR_OK;
}

DWORD CULY0Encoder::SetState(const void *pState, DWORD dwSize)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));

	memcpy(&m_ec, pState, min(sizeof(ENCODERCONF), dwSize));
	m_ec.dwFlags0 &= ~EC_FLAGS0_RESERVED;
	if ((m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK) == EC_FLAGS0_INTRAFRAME_PREDICT_RESERVED)
		m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;

	return min(sizeof(ENCODERCONF), dwSize);
}

DWORD CULY0Encoder::Compress(const ICCOMPRESS *icc, DWORD dwSize)
{
	FRAMEINFO fi;
	BYTE *p;
	DWORD count[256];

	m_icc = icc;

	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');

	memset(&fi, 0, sizeof(FRAMEINFO));

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CPredictJob(this, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_LEFT;
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
		fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;
		break;
	default:
		_ASSERT(false);
	}

	p = (BYTE *)icc->lpOutput;

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
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
		m_ptm->SubmitJob(new CEncodeJob(this, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	icc->lpbiOutput->biSizeImage = p - ((BYTE *)icc->lpOutput);
	*icc->lpdwFlags = AVIIF_KEYFRAME;

	return ICERR_OK;
}

DWORD CULY0Encoder::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DWORD dwRet;
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;

	dwRet = CompressQuery(pbihIn, pbihOut);
	if (dwRet != ICERR_OK)
		return dwRet;

	m_dwNumMacroStrides = pbihIn->biHeight / 2;
	m_dwDivideCount = ((pbieOut->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;

	_ASSERT(m_dwDivideCount >= 1 && m_dwDivideCount <= 256);

	CalcPlaneSizes(pbihIn);


	m_pCurFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_dwPlaneSize[i], m_dwPlaneWidth[i]);

	m_pMedianPredicted = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pMedianPredicted->AddPlane(m_dwPlaneSize[i], m_dwPlaneWidth[i]);

	m_counts = (COUNTS *)VirtualAlloc(NULL, sizeof(COUNTS) * m_dwDivideCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	m_ptm = new CThreadManager();

	return ICERR_OK;
}

DWORD CULY0Encoder::CompressEnd(void)
{
	delete m_pCurFrame;
	delete m_pMedianPredicted;

	VirtualFree(m_counts, 0, MEM_RELEASE);

	delete m_ptm;

	return ICERR_OK;
}

DWORD CULY0Encoder::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
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
	pbieOut->bih.biBitCount      = min(pbihIn->biBitCount, GetMaxBitCount());
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

DWORD CULY0Encoder::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ROUNDUP(pbihIn->biWidth, 4) * ROUNDUP(pbihIn->biHeight, 2) * GetOutputBitCount() / 8 + 4096; // +4096 はどんぶり勘定。
}

DWORD CULY0Encoder::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	const INPUTFORMAT *pfmts;

	if (pbihIn->biWidth % GetMacroPixelWidth() != 0 || pbihIn->biHeight % GetMacroPixelHeight() != 0)
		return ICERR_BADFORMAT;

	pfmts = GetSupportedInputFormats();
	for (int i = 0; i < GetNumSupportedInputFormats(); i++)
	{
		if (pbihIn->biCompression == pfmts[i].fcc && pbihIn->biBitCount == pfmts[i].nBitCount && pbihIn->biHeight > 0)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}

void CULY0Encoder::PredictProc(DWORD nBandIndex)
{
	DWORD dwMacroStrideBegin = m_dwNumMacroStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwMacroStrideEnd   = m_dwNumMacroStrides * (nBandIndex + 1) / m_dwDivideCount;

	ConvertToPlanar(nBandIndex);

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		DWORD dwPlaneBegin = dwMacroStrideBegin * m_dwPlaneMacroStride[nPlaneIndex];
		DWORD dwPlaneEnd   = dwMacroStrideEnd   * m_dwPlaneMacroStride[nPlaneIndex];

		for (int i = 0; i < 256; i++)
			m_counts[nBandIndex].dwCount[nPlaneIndex][i] = 0;

		switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			PredictLeftAndCount(m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_counts[nBandIndex].dwCount[nPlaneIndex]);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
			PredictMedianAndCount(m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_dwPlaneWidth[nPlaneIndex], m_counts[nBandIndex].dwCount[nPlaneIndex]);
			break;
		default:
			_ASSERT(false);
		}
	}
}

void CULY0Encoder::EncodeProc(DWORD nBandIndex)
{
	DWORD dwStrideBegin = m_dwNumMacroStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwStrideEnd   = m_dwNumMacroStrides * (nBandIndex + 1) / m_dwDivideCount;
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		DWORD dwPlaneBegin = dwStrideBegin * m_dwPlaneMacroStride[nPlaneIndex];
		DWORD dwPlaneEnd   = dwStrideEnd   * m_dwPlaneMacroStride[nPlaneIndex];
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

void CULY0Encoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
{
	m_dwPlaneSize[0]        = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]        = pbih->biWidth * pbih->biHeight / 4;
	m_dwPlaneSize[2]        = pbih->biWidth * pbih->biHeight / 4;

	m_dwPlaneWidth[0]       = pbih->biWidth;
	m_dwPlaneMacroStride[0] = m_dwPlaneWidth[0] * 2;

	m_dwPlaneWidth[1]       = pbih->biWidth / 2;
	m_dwPlaneMacroStride[1] = m_dwPlaneWidth[1];

	m_dwPlaneWidth[2]       = m_dwPlaneWidth[1];
	m_dwPlaneMacroStride[2] = m_dwPlaneMacroStride[1];
}

void CULY0Encoder::ConvertToPlanar(DWORD nBandIndex)
{
	DWORD dwMacroStrideBegin = m_dwNumMacroStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwMacroStrideEnd   = m_dwNumMacroStrides * (nBandIndex + 1) / m_dwDivideCount;

	BYTE *pDstYBegin = m_pCurFrame->GetPlane(0) + dwMacroStrideBegin * m_icc->lpbiInput->biWidth * 2;
	BYTE *pDstUBegin = m_pCurFrame->GetPlane(1) + dwMacroStrideBegin * m_icc->lpbiInput->biWidth / 2;
	BYTE *pDstVBegin = m_pCurFrame->GetPlane(2) + dwMacroStrideBegin * m_icc->lpbiInput->biWidth / 2;

	switch (m_icc->lpbiInput->biCompression)
	{
	case FCC('YV12'):
		{
			const BYTE *pSrcYBegin, *pSrcVBegin, *pSrcUBegin;

			pSrcYBegin = ((BYTE *)m_icc->lpInput);
			pSrcVBegin = pSrcYBegin + m_icc->lpbiInput->biWidth * m_icc->lpbiInput->biHeight;
			pSrcUBegin = pSrcVBegin + m_icc->lpbiInput->biWidth * m_icc->lpbiInput->biHeight / 4;

			pSrcYBegin += dwMacroStrideBegin * m_icc->lpbiInput->biWidth * 2;
			pSrcVBegin += dwMacroStrideBegin * m_icc->lpbiInput->biWidth / 2;
			pSrcUBegin += dwMacroStrideBegin * m_icc->lpbiInput->biWidth / 2;

			memcpy(pDstYBegin, pSrcYBegin, (dwMacroStrideEnd - dwMacroStrideBegin) * m_icc->lpbiInput->biWidth * 2);
			memcpy(pDstUBegin, pSrcUBegin, (dwMacroStrideEnd - dwMacroStrideBegin) * m_icc->lpbiInput->biWidth / 2);
			memcpy(pDstVBegin, pSrcVBegin, (dwMacroStrideEnd - dwMacroStrideBegin) * m_icc->lpbiInput->biWidth / 2);
		}
		break;
	default:
		{
			BYTE *y, *u, *v;
			const BYTE *pSrcBegin, *pSrcEnd;

			y = pDstYBegin;
			u = pDstUBegin;
			v = pDstVBegin;

			switch (m_icc->lpbiInput->biCompression)
			{
			case FCC('YVYU'):
				swap(pDstUBegin, pDstVBegin);
				swap(u, v);
				/* FALLTHROUGH */
			case FCC('YUY2'):
			case FCC('YUYV'):
			case FCC('YUNV'):
				{
					DWORD dwSrcStride = m_icc->lpbiInput->biWidth * 2;
					DWORD dwYStride = m_icc->lpbiInput->biWidth;

					pSrcBegin = ((BYTE *)m_icc->lpInput) + dwMacroStrideBegin * dwSrcStride * 2;
					pSrcEnd   = ((BYTE *)m_icc->lpInput) + dwMacroStrideEnd   * dwSrcStride * 2;

					for (const BYTE *pStrideBegin = pSrcBegin; pStrideBegin < pSrcEnd; pStrideBegin += dwSrcStride * 2)
					{
						const BYTE *pStrideEnd = pStrideBegin + dwSrcStride;
						for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
						{
							const BYTE *q = p + dwSrcStride;
							*(y+0) = *p;
							*(y+1) = *(p+2);
							*(y+dwYStride+0) = *q;
							*(y+dwYStride+1) = *(q+2);
							*u = (*(p+1) + *(q+1)) / 2;
							*v = (*(p+3) + *(q+3)) / 2;

							y+=2; u++; v++;
						}
						y += dwYStride;
					}
				}
				break;
			case FCC('VYUY'):
				swap(pDstUBegin, pDstVBegin);
				swap(u, v);
				/* FALLTHROUGH */
			case FCC('UYVY'):
			case FCC('UYNV'):
				{
					DWORD dwSrcStride = m_icc->lpbiInput->biWidth * 2;
					DWORD dwYStride = m_icc->lpbiInput->biWidth;

					pSrcBegin = ((BYTE *)m_icc->lpInput) + dwMacroStrideBegin * dwSrcStride * 2;
					pSrcEnd   = ((BYTE *)m_icc->lpInput) + dwMacroStrideEnd   * dwSrcStride * 2;

					for (const BYTE *pStrideBegin = pSrcBegin; pStrideBegin < pSrcEnd; pStrideBegin += dwSrcStride * 2)
					{
						const BYTE *pStrideEnd = pStrideBegin + dwSrcStride;
						for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
						{
							const BYTE *q = p + dwSrcStride;
							*(y+0) = *(p+1);
							*(y+1) = *(p+3);
							*(y+dwYStride+0) = *(q+1);
							*(y+dwYStride+1) = *(q+3);
							*u = (*(p+0) + *(q+0)) / 2;
							*v = (*(p+2) + *(q+2)) / 2;

							y+=2; u++; v++;
						}
						y += dwYStride;
					}
				}
				break;
			case BI_RGB:
				switch (m_icc->lpbiInput->biBitCount)
				{
				case 24:
					{
						DWORD dwDataStride = m_icc->lpbiInput->biWidth * 3;
						DWORD dwSrcStride = ROUNDUP(dwDataStride, 4);
						DWORD dwYStride = m_icc->lpbiInput->biWidth;

						pSrcBegin = ((BYTE *)m_icc->lpInput) + (m_dwNumMacroStrides - dwMacroStrideEnd  ) * dwSrcStride * 2;
						pSrcEnd   = ((BYTE *)m_icc->lpInput) + (m_dwNumMacroStrides - dwMacroStrideBegin) * dwSrcStride * 2;

						for (const BYTE *pStrideBegin = pSrcEnd - dwSrcStride * 2; pStrideBegin >= pSrcBegin; pStrideBegin -= dwSrcStride * 2)
						{
							const BYTE *pStrideEnd = pStrideBegin + dwDataStride;
							for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 6)
							{
								const BYTE *q = p + dwSrcStride;
								*(y+0)           = min(max(int((*(q+0))*0.098 + (*(q+1))*0.504 + (*(q+2))*0.257 + 16.5), 16), 235);
								*(y+1)           = min(max(int((*(q+3))*0.098 + (*(q+4))*0.504 + (*(q+5))*0.257 + 16.5), 16), 235);
								*(y+dwYStride+0) = min(max(int((*(p+0))*0.098 + (*(p+1))*0.504 + (*(p+2))*0.257 + 16.5), 16), 235);
								*(y+dwYStride+1) = min(max(int((*(p+3))*0.098 + (*(p+4))*0.504 + (*(p+5))*0.257 + 16.5), 16), 235);
								*u               = min(max(int(((*(p+0)+*(p+3)+*(q+0)+*(q+3))*0.439 + (*(p+1)+*(p+4)+*(q+1)+*(q+4))*-0.291 + (*(p+2)+*(p+5)+*(q+2)+*(q+5))*-0.148)/4 + 128.5), 16), 240);
								*v               = min(max(int(((*(p+0)+*(p+3)+*(q+0)+*(q+3))*-0.071 + (*(p+1)+*(p+4)+*(q+1)+*(q+4))*-0.368 + (*(p+2)+*(p+5)+*(q+2)+*(q+5))*0.439)/4 + 128.5), 16), 240);
								y+=2; u++; v++;
							}
							y += dwYStride;
						}
					}
					break;
				case 32:
					{
						DWORD dwDataStride = m_icc->lpbiInput->biWidth * 4;
						DWORD dwSrcStride = ROUNDUP(dwDataStride, 4);
						DWORD dwYStride = m_icc->lpbiInput->biWidth;

						pSrcBegin = ((BYTE *)m_icc->lpInput) + (m_dwNumMacroStrides - dwMacroStrideEnd  ) * dwSrcStride * 2;
						pSrcEnd   = ((BYTE *)m_icc->lpInput) + (m_dwNumMacroStrides - dwMacroStrideBegin) * dwSrcStride * 2;

						for (const BYTE *pStrideBegin = pSrcEnd - dwSrcStride * 2; pStrideBegin >= pSrcBegin; pStrideBegin -= dwSrcStride * 2)
						{
							const BYTE *pStrideEnd = pStrideBegin + dwDataStride;
							for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 8)
							{
								const BYTE *q = p + dwSrcStride;
								*(y+0)           = min(max(int((*(q+0))*0.098 + (*(q+1))*0.504 + (*(q+2))*0.257 + 16.5), 16), 235);
								*(y+1)           = min(max(int((*(q+4))*0.098 + (*(q+5))*0.504 + (*(q+6))*0.257 + 16.5), 16), 235);
								*(y+dwYStride+0) = min(max(int((*(p+0))*0.098 + (*(p+1))*0.504 + (*(p+2))*0.257 + 16.5), 16), 235);
								*(y+dwYStride+1) = min(max(int((*(p+4))*0.098 + (*(p+5))*0.504 + (*(p+6))*0.257 + 16.5), 16), 235);
								*u               = min(max(int(((*(p+0)+*(p+4)+*(q+0)+*(q+4))*0.439 + (*(p+1)+*(p+5)+*(q+1)+*(q+5))*-0.291 + (*(p+2)+*(p+6)+*(q+2)+*(q+6))*-0.148)/4 + 128.5), 16), 240);
								*v               = min(max(int(((*(p+0)+*(p+4)+*(q+0)+*(q+4))*-0.071 + (*(p+1)+*(p+5)+*(q+1)+*(q+5))*-0.368 + (*(p+2)+*(p+6)+*(q+2)+*(q+6))*0.439)/4 + 128.5), 16), 240);
								y+=2; u++; v++;
							}
							y += dwYStride;
						}
					}
					break;
				}
			}
		}
	}
}
