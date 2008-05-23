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
#include "VCMCodec.h"
#include "ULY2Encoder.h"
#include "ULY2Decoder.h"
#include "ULRGEncoder.h"
#include "ULRGDecoder.h"

const CVCMCodec::CODECLIST CVCMCodec::m_codeclist[] = {
	{ -1,          "",       CDummyEncoder::CreateInstance, CDummyDecoder::CreateInstance },
	{ FCC('ULY2'), "YUV422", CULY2Encoder::CreateInstance,  CULY2Decoder::CreateInstance  },
	{ FCC('ULRG'), "RGB",    CULRGEncoder::CreateInstance,  CULRGDecoder::CreateInstance  },
};

void CVCMCodec::ICInstallAll(void)
{
	char szLongFilename[MAX_PATH];
	char szShortFilename[MAX_PATH];

	/*
	 * 何故か長いファイル名ではダメ（登録はできるがロードに失敗する）で、
	 * 8.3 形式のファイル名で登録しなければならない。
	 */
	GetModuleFileName(hModule, szLongFilename, sizeof(szLongFilename));
	GetShortPathName(szLongFilename, szShortFilename, sizeof(szShortFilename));
	for (int i = 1; i < _countof(m_codeclist); i++)
		ICInstall(ICTYPE_VIDEO, m_codeclist[i].fcc, (LPARAM)szShortFilename, NULL, ICINSTALL_DRIVER);
}

void CVCMCodec::ICRemoveAll(void)
{
	for (int i = 1; i < _countof(m_codeclist); i++)
		ICRemove(ICTYPE_VIDEO, m_codeclist[i].fcc, 0);
}

CVCMCodec::CVCMCodec(DWORD fccHandler)
{
	int idx;

	for (idx = 0; idx < _countof(m_codeclist); idx++)
	{
		if (m_codeclist[idx].fcc == fccHandler)
			break;
	}
	if (idx == _countof(m_codeclist))
		idx = 0;

	m_fccHandler         = m_codeclist[idx].fcc;
	m_pszColorFormatName = m_codeclist[idx].pszColorFormatName;
	m_pEncoder           = m_codeclist[idx].pfnCreateEncoder();
	m_pDecoder           = m_codeclist[idx].pfnCreateDecoder();
	_RPT2(_CRT_WARN, "infcc=%08X foundfcc=%08X\n", fccHandler, m_fccHandler);
}

CVCMCodec::~CVCMCodec(void)
{
	delete m_pEncoder;
	delete m_pDecoder;
}

CVCMCodec *CVCMCodec::Open(ICOPEN *icopen)
{
	union
	{
		DWORD fccHandler;
		char fccChar[4];
	};

	if (icopen != NULL)
	{
		if (icopen->fccType != ICTYPE_VIDEO)
			return NULL;
		fccHandler = icopen->fccHandler;
		// なぜか小文字で渡されることがあるので、最初に大文字化しておく。
		for (int i = 0; i < 4; i++)
			fccChar[i] = toupper(fccChar[i]);
		icopen->dwError = ICERR_OK;
	}
	else
		fccHandler = (DWORD)-1;

	return new CVCMCodec(fccHandler);
}

DWORD CVCMCodec::QueryAbout(void)
{
	return ICERR_OK;
}

DWORD CVCMCodec::About(HWND hwnd)
{
	char buf[256];

	wsprintf(buf, "Ut Video Codec Suite, Version %d.%d.%d\nCopyright (C) 2008  UMEZAWA Takeshi",
		(BYTE)(UTVIDEO_ENCODER_VERSION >> 24),
		(BYTE)(UTVIDEO_ENCODER_VERSION >> 16),
		(BYTE)(UTVIDEO_ENCODER_VERSION >> 8));
	MessageBox(hwnd, buf, "Ut Video Codec Suite", MB_OK);
	return ICERR_OK;
}

DWORD CVCMCodec::GetInfo(ICINFO *icinfo, DWORD dwSize)
{
	if (icinfo == NULL)
		return sizeof(ICINFO);

	if (dwSize < sizeof(ICINFO))
		return 0;

	icinfo->dwSize       = sizeof(ICINFO);
	icinfo->fccType      = ICTYPE_VIDEO;
	icinfo->fccHandler   = m_fccHandler;
	icinfo->dwFlags      = 0;
	icinfo->dwVersion    = UTVIDEO_ENCODER_VERSION;
	icinfo->dwVersionICM = ICVERSION;
	wsprintfW(icinfo->szName, L"Ut Video (%C%C%C%C)", FCC4PRINTF(m_fccHandler));
	wsprintfW(icinfo->szDescription, L"Ut Video Codec %S (%C%C%C%C) VCM", m_pszColorFormatName, FCC4PRINTF(m_fccHandler));

	return sizeof(ICINFO);
}

DWORD CVCMCodec::QueryConfigure(void)
{
	return ICERR_OK;
}

DWORD CVCMCodec::Configure(HWND hwnd)
{
	return m_pEncoder->Configure(hwnd);
}

DWORD CVCMCodec::GetStateSize(void)
{
	return sizeof(ENCODERCONF);
}

DWORD CVCMCodec::GetState(void *pState, DWORD dwSize)
{
	_RPT0(_CRT_WARN, "CVCMCodec::GetState()\n");
	return m_pEncoder->GetState(pState, dwSize);
}

DWORD CVCMCodec::SetState(const void *pState, DWORD dwSize)
{
	_RPT0(_CRT_WARN, "CVCMCodec::SetState()\n");
	return m_pEncoder->SetState(pState, dwSize);
}

DWORD CVCMCodec::Compress(const ICCOMPRESS *icc, DWORD dwSize)
{
	return m_pEncoder->Compress(icc, dwSize);
}

DWORD CVCMCodec::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	_RPT0(_CRT_WARN, "CVCMCodec::CompressBegin()\n");
	return m_pEncoder->CompressBegin(pbihIn, pbihOut);
}

DWORD CVCMCodec::CompressEnd(void)
{
	_RPT0(_CRT_WARN, "CVCMCodec::CompressEnd()\n");
	return m_pEncoder->CompressEnd();
}

DWORD CVCMCodec::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	_RPT0(_CRT_WARN, "CVCMCodec::CompressGetFormat()\n");
	return m_pEncoder->CompressGetFormat(pbihIn, pbihOut);
}

DWORD CVCMCodec::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	_RPT0(_CRT_WARN, "CVCMCodec::CompressGetSize()\n");
	return m_pEncoder->CompressGetSize(pbihIn, pbihOut);
}

DWORD CVCMCodec::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	_RPT0(_CRT_WARN, "CVCMCodec::CompressQuery()\n");
	return m_pEncoder->CompressQuery(pbihIn, pbihOut);
}

DWORD CVCMCodec::Decompress(const ICDECOMPRESS *icd, DWORD dwSize)
{
	return m_pDecoder->Decompress(icd, dwSize);
}

DWORD CVCMCodec::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	_RPT0(_CRT_WARN, "CVCMCodec::DecompressBegin()\n");
	return m_pDecoder->DecompressBegin(pbihIn, pbihOut);
}

DWORD CVCMCodec::DecompressEnd(void)
{
	_RPT0(_CRT_WARN, "CVCMCodec::DecompressEnd()\n");
	return m_pDecoder->DecompressEnd();
}

DWORD CVCMCodec::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	_RPT0(_CRT_WARN, "CVCMCodec::DecompressGetFormat()\n");
	return m_pDecoder->DecompressGetFormat(pbihIn, pbihOut);
}

DWORD CVCMCodec::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	_RPT0(_CRT_WARN, "CVCMCodec::DecompressQuery()\n");
	return m_pDecoder->DecompressQuery(pbihIn, pbihOut);
}
