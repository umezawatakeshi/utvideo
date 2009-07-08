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

#pragma once
#include "Encoder.h"
#include "Decoder.h"

class CVCMCodec
{
private:
	struct CODECLIST
	{
		DWORD fcc;
		const char *pszColorFormatName;
		CEncoder *(*pfnCreateEncoder)(void);
		CDecoder *(*pfnCreateDecoder)(void);
	};

	static const CODECLIST m_codeclist[];

public:
	static void ICInstallAll(void);
	static void ICRemoveAll(void);

private:
	DWORD m_fccHandler;
	const char *m_pszColorFormatName;
	CEncoder *m_pEncoder;
	CDecoder *m_pDecoder;

private:
	CVCMCodec(DWORD fccHandler);
public:
	~CVCMCodec(void);

public:
	static CVCMCodec *Open(ICOPEN *icopen);

	DWORD QueryAbout(void);
	DWORD About(HWND hwnd);
	DWORD GetInfo(ICINFO *icinfo, DWORD dwSize);

	DWORD QueryConfigure(void);
	DWORD Configure(HWND hwnd);
	DWORD GetStateSize(void);
	DWORD GetState(void *pState, DWORD dwSize);
	DWORD SetState(const void *pState, DWORD dwSize);
	DWORD Compress(const ICCOMPRESS *icc, DWORD dwSize);
	DWORD CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	DWORD CompressEnd(void);
	DWORD CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	DWORD CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	DWORD CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

	DWORD Decompress(const ICDECOMPRESS *icd, DWORD dwSize);
	DWORD DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	DWORD DecompressEnd(void);
	DWORD DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	DWORD DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
