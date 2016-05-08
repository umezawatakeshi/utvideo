/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
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

void CALLBACK GlobalConfigDialog(HWND hWnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	DialogBox(hModule, MAKEINTRESOURCE(IDD_GLOBAL_CONFIG), hWnd, GlobalConfigDialogProc);
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

	SendMessage(GetDlgItem(hWnd, IDC_WORKER_THREAD_PRIORITY_SLIDER), TBM_SETRANGEMIN, FALSE, -2);
	SendMessage(GetDlgItem(hWnd, IDC_WORKER_THREAD_PRIORITY_SLIDER), TBM_SETRANGEMAX, TRUE,  +2);
	SendMessage(GetDlgItem(hWnd, IDC_WORKER_THREAD_PRIORITY_SLIDER), TBM_SETTICFREQ, 1, 0);

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Ut Video Codec Suite", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkUtVideo, NULL) != ERROR_SUCCESS)
		return;

	cb = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "SaveConfig", NULL, &dwType, (BYTE *)&dwValue, &cb) == ERROR_SUCCESS)
		CheckDlgButton(hWnd, IDC_SAVE_CONFIG_CHECK, dwValue);

	cb = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "IgnoreSetConfig", NULL, &dwType, (BYTE *)&dwValue, &cb) == ERROR_SUCCESS)
		CheckDlgButton(hWnd, IDC_IGNORE_SET_CONFIG_CHECK, dwValue);

	cb = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "WorkerThreadPriority", NULL, &dwType, (BYTE *)&dwValue, &cb) == ERROR_SUCCESS)
	{
		if ((LONG)dwValue > +2)
			dwValue = +2;
		if ((LONG)dwValue < -2)
			dwValue = -2;
		SendMessage(GetDlgItem(hWnd, IDC_WORKER_THREAD_PRIORITY_SLIDER), TBM_SETPOS, TRUE, dwValue);
	}

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

	dwValue = (DWORD)SendMessage(GetDlgItem(hWnd, IDC_WORKER_THREAD_PRIORITY_SLIDER), TBM_GETPOS, 0, 0);
	RegSetValueEx(hkUtVideo, "WorkerThreadPriority", 0, REG_DWORD, (const BYTE *)&dwValue, sizeof(DWORD));

	RegCloseKey(hkUtVideo);
}

#endif
