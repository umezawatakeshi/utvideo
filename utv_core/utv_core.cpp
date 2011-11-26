/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"

#ifdef _WIN32

HMODULE hModule;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	//_RPT3(_CRT_WARN, "DllMain(HMODULE, DWORD, LPVOID) hModule=%p dwReason=%08X lpReserved=%p\n", hModule, dwReason, lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		::hModule = hModule;
	}

	return TRUE;
}

#endif

#ifdef _DEBUG

#include "Predict.h"

DLLEXPORT void UnitTest_core(void)
{
	UnitTest_Predict();
}

#endif
