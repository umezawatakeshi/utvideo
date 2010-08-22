/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// utv_cfg.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "utv_cfg.h"

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnInitDialog(HWND hWnd);
void OnOK(HWND hWnd);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	return (int)DialogBox(hInstance, MAKEINTRESOURCE(IDD_GLOBAL_CONFIG_DIALOG), NULL, DialogProc);
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
