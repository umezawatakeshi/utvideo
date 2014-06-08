/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "utv_core.h"
#include "GlobalConfig.h"

#ifdef _WIN32

#include "resource.h"

static INT_PTR CALLBACK GlobalConfigDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void OnInitDialog(HWND hWnd);
static void OnOK(HWND hWnd);

__declspec(dllexport) INT_PTR GlobalConfigDialog(HWND hWnd)
{
	return DialogBox(hModule, MAKEINTRESOURCE(IDD_GLOBAL_CONFIG), hWnd, GlobalConfigDialogProc);
}

INT_PTR CALLBACK GlobalConfigDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			OnOK(hWnd);
			/* FALLTHROUGH */
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

void OnInitDialog(HWND hWnd)
{
	HKEY hkUtVideo;
	DWORD dwValue;
	DWORD cb;
	DWORD dwType;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Ut Video Codec Suite", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkUtVideo, NULL) != ERROR_SUCCESS)
		return;

	cb = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "SaveConfig", NULL, &dwType, (BYTE *)&dwValue, &cb) == ERROR_SUCCESS)
		CheckDlgButton(hWnd, IDC_SAVE_CONFIG_CHECK, dwValue);

	cb = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "IgnoreSetConfig", NULL, &dwType, (BYTE *)&dwValue, &cb) == ERROR_SUCCESS)
		CheckDlgButton(hWnd, IDC_IGNORE_SET_CONFIG_CHECK, dwValue);

	RegCloseKey(hkUtVideo);
}

void OnOK(HWND hWnd)
{
	HKEY hkUtVideo;
	DWORD dwValue;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Ut Video Codec Suite", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkUtVideo, NULL) != ERROR_SUCCESS)
		return;

	dwValue = IsDlgButtonChecked(hWnd, IDC_SAVE_CONFIG_CHECK);
	RegSetValueEx(hkUtVideo, "SaveConfig", 0, REG_DWORD, (const BYTE *)&dwValue, sizeof(DWORD));

	dwValue = IsDlgButtonChecked(hWnd, IDC_IGNORE_SET_CONFIG_CHECK);
	RegSetValueEx(hkUtVideo, "IgnoreSetConfig", 0, REG_DWORD, (const BYTE *)&dwValue, sizeof(DWORD));

	RegCloseKey(hkUtVideo);
}

#endif
