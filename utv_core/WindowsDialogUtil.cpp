/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "utv_core.h"
#include "WindowsDialogUtil.h"

static BOOL CALLBACK EnumDialogToolsProc(HWND hwndTool, LPARAM lParam)
{
	HWND hwndTip = (HWND)lParam;
	HWND hwndDialog = GetParent(hwndTip);

	if (!(GetWindowLongPtr(hwndTool, GWL_STYLE) & WS_CHILD))
		return TRUE;
	if (GetParent(hwndTool) != hwndDialog)
		return TRUE;

	UINT uId = (UINT)GetWindowLongPtr(hwndTool, GWLP_ID);

	char szTipText[512];
	if (LoadString(hModule, uId, szTipText, _countof(szTipText) - 1) == 0)
		return TRUE;

	TOOLINFO ti = {};
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd = hwndDialog;
	ti.uId = (UINT_PTR)hwndTool;
	ti.hinst = hModule;
	ti.lpszText = szTipText;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);

	return TRUE;
}

BOOL AddToolTips(HWND hwnd)
{
	HWND hwndTip = CreateWindow(TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd, NULL, hModule, NULL);
	if (hwndTip == NULL)
		return FALSE;

	SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, 640);

	return EnumChildWindows(hwnd, EnumDialogToolsProc, (LPARAM)hwndTip);
}
