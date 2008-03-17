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

#include "stdafx.h"
#include "VCMCodec.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

const DWORD fccUAY2 = FCC('UAY2');

HMODULE hModule;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
		::hModule = hModule;

	return TRUE;
}

BOOL APIENTRY ICInstallSelf(void)
{
	char szLongFilename[MAX_PATH];
	char szShortFilename[MAX_PATH];

	/*
	 * 何故か長いファイル名ではダメ（登録はできるがロードに失敗する）で、
	 * 8.3 形式のファイル名で登録しなければならない。
	 */
	GetModuleFileName(hModule, szLongFilename, sizeof(szLongFilename));
	GetShortPathName(szLongFilename, szShortFilename, sizeof(szShortFilename));
	return ICInstall(ICTYPE_VIDEO, fccUAY2, (LPARAM)szShortFilename, NULL, ICINSTALL_DRIVER);
}

BOOL APIENTRY ICRemoveSelf(void)
{
	return ICRemove(ICTYPE_VIDEO, fccUAY2, 0);
}

/*
 * ICInstall()/ICRemove() の返り値は信用できない（常に nonzero が返る）ので、
 * エラーチェックは行わない。
 */

void CALLBACK ICInstallByRundll(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	ICInstallSelf();
}

void CALLBACK ICRemoveByRundll(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	ICRemoveSelf();
}

LRESULT CALLBACK DriverProc(DWORD dwDriverId, HDRVR hdrvr,UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	CVCMCodec *pCodec = (CVCMCodec *)dwDriverId;

	switch (uMsg)
	{
	/* Driver Messages */
	case DRV_LOAD:
		return TRUE;

	case DRV_FREE:
		return TRUE;

	case DRV_OPEN:
		return (LRESULT)CVCMCodec::Open((ICOPEN *)lParam2);

	case DRV_CLOSE:
		if (pCodec != NULL)
			delete pCodec;
		return TRUE;

	/* Codec Messages */
	case ICM_ABOUT:
		if (lParam1 == -1)
			return pCodec->QueryAbout();
		else
			return pCodec->About((HWND)lParam1);

	case ICM_GETINFO:
		return pCodec->GetInfo((ICINFO *)lParam1, (DWORD)lParam2);

	/* Encoder Messages */
	case ICM_CONFIGURE:
		if (lParam1 == -1)
			return pCodec->QueryConfigure();
		else
			return pCodec->Configure((HWND)lParam1);

	//case ICM_GETSTATE:
	//case ICM_SETSTATE:

	case ICM_COMPRESS:
		return pCodec->Compress((ICCOMPRESS *)lParam1, (DWORD)lParam2);

	case ICM_COMPRESS_BEGIN:
		return pCodec->CompressBegin((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_COMPRESS_END:
		return pCodec->CompressEnd();

	case ICM_COMPRESS_GET_FORMAT:
		return pCodec->CompressGetFormat((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_COMPRESS_GET_SIZE:
		return pCodec->CompressGetSize((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_COMPRESS_QUERY:
		return pCodec->CompressQuery((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	/* Decoder Messages */
	case ICM_DECOMPRESS:
	case ICM_DECOMPRESS_BEGIN:
	case ICM_DECOMPRESS_END:
	case ICM_DECOMPRESS_GET_FORMAT:
	case ICM_DECOMPRESS_GET_PALETTE:
	case ICM_DECOMPRESS_QUERY:
	case ICM_DECOMPRESS_SET_PALETTE:
		;
	}

	return DefDriverProc(dwDriverId, hdrvr, uMsg, lParam1, lParam2);
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
