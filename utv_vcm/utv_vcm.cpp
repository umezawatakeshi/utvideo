/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// utv_vcm.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_ATTACH, lpReserved=%p)", hModule, lpReserved);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_DETACH, lpReserved=%p)", hModule, lpReserved);
	}

	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

