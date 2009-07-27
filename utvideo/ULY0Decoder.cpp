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
#include "ULY0Decoder.h"
#include "Predict.h"

const CULY0Decoder::OUTPUTFORMAT CULY0Decoder::m_outfmts[] = {
	{ FCC('YV12'), 12 },
	{ FCC('YUY2'), 16 }, { FCC('YUYV'), 16 }, { FCC('YUNV'), 16 },
	{ BI_RGB, 32 },
	{ BI_RGB, 24 },
};

CULY0Decoder::CULY0Decoder(void)
{
}

CULY0Decoder::~CULY0Decoder(void)
{
}

int CULY0Decoder::GetNumSupportedOutputFormats(void)
{
	return _countof(m_outfmts);
}

CDecoder *CULY0Decoder::CreateInstance(void)
{
	return new CULY0Decoder();
}

DWORD CULY0Decoder::Decompress(const ICDECOMPRESS *icd, DWORD dwSize)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)icd->lpbiInput;
	const BYTE *p;

	m_icd = icd;

	memset(&m_fi, 0, sizeof(FRAMEINFO));
	memcpy(&m_fi, ((BYTE *)icd->lpInput) + pbieIn->bih.biSizeImage - pbieIn->dwFrameInfoSize, pbieIn->dwFrameInfoSize);

	p = (BYTE *)icd->lpInput;
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
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

DWORD CULY0Decoder::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DWORD dwRet;
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	dwRet = DecompressQuery(pbihIn, pbihOut);
	if (dwRet != ICERR_OK)
		return dwRet;

	m_dwNumMacroStrides = pbihIn->biHeight / 2;
	m_dwDivideCount = ((pbieIn->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;

	_ASSERT(m_dwDivideCount >= 1 && m_dwDivideCount <= 256);
	_RPT1(_CRT_WARN, "divide count = %d\n", m_dwDivideCount);

	m_bBottomUpFrame = FALSE;
	switch (pbihOut->biCompression)
	{
	case FCC('YV12'):
		//m_dwFrameSize = (pbihOut->biWidth * pbihOut->biHeight * 12) / 8; // XXX 幅や高さが奇数の場合は考慮していない
		m_dwFrameSize = (pbihOut->biWidth * pbihOut->biHeight * 3) / 2; // XXX 幅や高さが奇数の場合は考慮していない
		break;
	default:
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
		m_dwFrameSize = m_dwFrameStride * m_dwNumMacroStrides * 2;
	}

	CalcPlaneSizes(pbihOut);

	m_pRestoredFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pRestoredFrame->AddPlane(m_dwPlaneSize[i], m_dwPlaneStride[i]);

	m_pDecodedFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pDecodedFrame->AddPlane(m_dwPlaneSize[i], m_dwPlaneStride[i]);

	m_ptm = new CThreadManager();

	return ICERR_OK;
}

DWORD CULY0Decoder::DecompressEnd(void)
{
	delete m_pRestoredFrame;
	delete m_pDecodedFrame;

	delete m_ptm;

	return ICERR_OK;
}

DWORD CULY0Decoder::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
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

DWORD CULY0Decoder::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	const OUTPUTFORMAT *pfmts;

	if (pbihIn->biCompression != GetInputFCC())
		return ICERR_BADFORMAT;

	if (pbihIn->biWidth % GetMacroPixelWidth() != 0 || pbihIn->biHeight % GetMacroPixelHeight() != 0)
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
		if (pbihOut->biCompression == pfmts[i].fcc && pbihOut->biBitCount == pfmts[i].nBitCount && pbihOut->biHeight > 0 && pbihOut->biHeight == pbihIn->biHeight && pbihOut->biWidth == pbihIn->biWidth)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}

void CULY0Decoder::DecodeProc(DWORD nBandIndex)
{
	DWORD dwStrideBegin = m_dwNumMacroStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwStrideEnd   = m_dwNumMacroStrides * (nBandIndex + 1) / m_dwDivideCount;

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		DWORD dwPlaneBegin = dwStrideBegin * m_dwPlaneMacroStride[nPlaneIndex];
		DWORD dwPlaneEnd   = dwStrideEnd   * m_dwPlaneMacroStride[nPlaneIndex];
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

void CULY0Decoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
{
	m_dwPlaneSize[0]        = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]        = pbih->biWidth * pbih->biHeight / 4;
	m_dwPlaneSize[2]        = pbih->biWidth * pbih->biHeight / 4;

	m_dwPlaneStride[0]      = pbih->biWidth;
	m_dwPlaneMacroStride[0] = m_dwPlaneStride[0] * 2;

	m_dwPlaneStride[1]      = pbih->biWidth / 2;
	m_dwPlaneMacroStride[1] = m_dwPlaneStride[1];

	m_dwPlaneStride[2]      = m_dwPlaneStride[1];
	m_dwPlaneMacroStride[2] = m_dwPlaneMacroStride[1];
}

void CULY0Decoder::ConvertFromPlanar(DWORD nBandIndex)
{
	DWORD dwMacroStrideBegin = m_dwNumMacroStrides *  nBandIndex      / m_dwDivideCount;
	DWORD dwMacroStrideEnd   = m_dwNumMacroStrides * (nBandIndex + 1) / m_dwDivideCount;

	const BYTE *pSrcYBegin = m_pCurFrame->GetPlane(0) + dwMacroStrideBegin * m_icd->lpbiOutput->biWidth * 2;
	const BYTE *pSrcUBegin = m_pCurFrame->GetPlane(1) + dwMacroStrideBegin * m_icd->lpbiOutput->biWidth / 2;
	const BYTE *pSrcVBegin = m_pCurFrame->GetPlane(2) + dwMacroStrideBegin * m_icd->lpbiOutput->biWidth / 2;

	switch (m_icd->lpbiOutput->biCompression)
	{
	case FCC('YV12'):
		{
			BYTE *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((BYTE *)m_icd->lpOutput);
			pDstVBegin = pDstYBegin + m_icd->lpbiOutput->biWidth * m_icd->lpbiOutput->biHeight;
			pDstUBegin = pDstVBegin + m_icd->lpbiOutput->biWidth * m_icd->lpbiOutput->biHeight / 4;

			pDstYBegin += dwMacroStrideBegin * m_icd->lpbiOutput->biWidth * 2;
			pDstVBegin += dwMacroStrideBegin * m_icd->lpbiOutput->biWidth / 2;
			pDstUBegin += dwMacroStrideBegin * m_icd->lpbiOutput->biWidth / 2;

			memcpy(pDstYBegin, pSrcYBegin, (dwMacroStrideEnd - dwMacroStrideBegin) * m_icd->lpbiOutput->biWidth * 2);
			memcpy(pDstUBegin, pSrcUBegin, (dwMacroStrideEnd - dwMacroStrideBegin) * m_icd->lpbiOutput->biWidth / 2);
			memcpy(pDstVBegin, pSrcVBegin, (dwMacroStrideEnd - dwMacroStrideBegin) * m_icd->lpbiOutput->biWidth / 2);
		}
		break;
	default:
		{
			const BYTE *y, *u, *v;
			BYTE *pDstBegin, *pDstEnd;

			y = pSrcYBegin;
			u = pSrcUBegin;
			v = pSrcVBegin;

			switch (m_icd->lpbiOutput->biCompression)
			{
			case FCC('YVYU'):
				swap(pSrcUBegin, pSrcVBegin);
				swap(u, v);
				/* FALLTHROUGH */
			case FCC('YUY2'):
			case FCC('YUYV'):
			case FCC('YUNV'):
				{
					DWORD dwDstStride;
					DWORD dwYStride;

					dwDstStride = m_icd->lpbiOutput->biWidth * 2;
					dwYStride = m_icd->lpbiOutput->biWidth;

					pDstBegin = ((BYTE *)m_icd->lpOutput) + dwMacroStrideBegin * dwDstStride * 2;
					pDstEnd   = ((BYTE *)m_icd->lpOutput) + dwMacroStrideEnd   * dwDstStride * 2;

					for (BYTE *pStrideBegin = pDstBegin; pStrideBegin < pDstEnd; pStrideBegin += dwDstStride * 2)
					{
						BYTE *pStrideEnd = pStrideBegin + dwDstStride;
						for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
						{
							BYTE *q = p + dwDstStride;
							*p = *(y+0);
							*(p+2) = *(y+1);
							*q = *(y+dwYStride+0);
							*(q+2) = *(y+dwYStride+1);
							*(p+1) = *u;
							*(q+1) = *u;
							*(p+3) = *v;
							*(q+3) = *v;

							y+=2; u++; v++;
						}
						y += dwYStride;
					}
				}
				break;
			case FCC('VYUY'):
				swap(pSrcUBegin, pSrcVBegin);
				swap(u, v);
				/* FALLTHROUGH */
			case FCC('UYVY'):
			case FCC('UYNV'):
				{
					DWORD dwDstStride;
					DWORD dwYStride;

					dwDstStride = m_icd->lpbiOutput->biWidth * 2;
					dwYStride = m_icd->lpbiOutput->biWidth;

					pDstBegin = ((BYTE *)m_icd->lpOutput) + dwMacroStrideBegin * dwDstStride * 2;
					pDstEnd   = ((BYTE *)m_icd->lpOutput) + dwMacroStrideEnd   * dwDstStride * 2;

					for (BYTE *pStrideBegin = pDstBegin; pStrideBegin < pDstEnd; pStrideBegin += dwDstStride * 2)
					{
						BYTE *pStrideEnd = pStrideBegin + dwDstStride;
						for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
						{
							BYTE *q = p + dwDstStride;
							*(p+1) = *(y+0);
							*(p+3) = *(y+1);
							*(q+1) = *(y+dwYStride+0);
							*(q+3) = *(y+dwYStride+1);
							*(p+0) = *u;
							*(q+0) = *u;
							*(p+2) = *v;
							*(q+2) = *v;

							y+=2; u++; v++;
						}
						y += dwYStride;
					}
				}
				break;
			case BI_RGB:
				switch (m_icd->lpbiOutput->biBitCount)
				{
				case 24:
					{
						DWORD dwDstStride;
						DWORD dwYStride;
						DWORD dwDataStride;

						dwDataStride = m_icd->lpbiOutput->biWidth * 3;
						dwDstStride = ROUNDUP(dwDataStride, 4);
						dwYStride = m_icd->lpbiOutput->biWidth;

						pDstBegin = ((BYTE *)m_icd->lpOutput) + (m_dwNumMacroStrides - dwMacroStrideEnd  ) * dwDstStride * 2;
						pDstEnd   = ((BYTE *)m_icd->lpOutput) + (m_dwNumMacroStrides - dwMacroStrideBegin) * dwDstStride * 2;

						for (BYTE *pStrideBegin = pDstEnd - dwDstStride * 2; pStrideBegin >= pDstBegin; pStrideBegin -= dwDstStride * 2)
						{
							BYTE *pStrideEnd = pStrideBegin + dwDataStride;
							for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 6)
							{
								BYTE *q = p + dwDstStride;
								*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+1) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+0) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+2) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								y++;
								*(q+4) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+3) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+5) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+4) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+3) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+5) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								y++; u++; v++;
							}
							y += dwYStride;
						}
					}
					break;
				case 32:
					{
						DWORD dwDstStride;
						DWORD dwYStride;
						DWORD dwDataStride;

						dwDataStride = m_icd->lpbiOutput->biWidth * 4;
						dwDstStride = ROUNDUP(dwDataStride, 4);
						dwYStride = m_icd->lpbiOutput->biWidth;

						pDstBegin = ((BYTE *)m_icd->lpOutput) + (m_dwNumMacroStrides - dwMacroStrideEnd  ) * dwDstStride * 2;
						pDstEnd   = ((BYTE *)m_icd->lpOutput) + (m_dwNumMacroStrides - dwMacroStrideBegin) * dwDstStride * 2;

						for (BYTE *pStrideBegin = pDstEnd - dwDstStride * 2; pStrideBegin >= pDstBegin; pStrideBegin -= dwDstStride * 2)
						{
							BYTE *pStrideEnd = pStrideBegin + dwDataStride;
							for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 8)
							{
								BYTE *q = p + dwDstStride;
								*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(q+3) = 0xff;
								*(p+1) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+0) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+2) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+3) = 0xff;
								y++;
								*(q+5) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+4) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+6) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(q+7) = 0xff;
								*(p+5) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+4) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+6) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+7) = 0xff;
								y++; u++; v++;
							}
							y += dwYStride;
						}
					}
					break;
				}
			}
			break;
		}
	}
}
