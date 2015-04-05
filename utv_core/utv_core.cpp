/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"

#ifdef _WIN32

HMODULE hModule;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		LOGPRINTF("DllMain(hModule=%" PRIp ", dwReason=DLL_PROCESS_ATTACH lpReserved=%" PRIp ")", hModule, lpReserved);
		::hModule = hModule;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOGPRINTF("DllMain(hModule=%" PRIp ", dwReason=DLL_PROCESS_DETACH lpReserved=%" PRIp ")", hModule, lpReserved);
	}

	return TRUE;
}

#endif

#ifdef _DEBUG

DLLEXPORT void UnitTest_core(void)
{
}

#endif
