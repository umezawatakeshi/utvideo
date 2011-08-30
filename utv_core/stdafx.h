/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

// 下で指定された定義の前に対象プラットフォームを指定しなければならない場合、以下の定義を変更してください。
// 異なるプラットフォームに対応する値に関する最新情報については、MSDN を参照してください。
#ifndef WINVER				// Windows XP 以降のバージョンに固有の機能の使用を許可します。
#define WINVER 0x0501		// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_WINDOWS 0x0410 // これを Windows Me またはそれ以降のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_IE			// IE 6.0 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_IE 0x0600	// これを IE. の他のバージョン向けに適切な値に変更してください。
#endif

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから使用されていない部分を除外します。
#define _CRT_SECURE_NO_WARNINGS

// intrin.h をインクルードすると
// error C2733: オーバーロードされた関数 '_interlockedbittestandset' の C リンケージの 2 回以上の宣言は許されません。
// などとエラーが出るので、手書きで intrinsic 宣言を行う。
// #include <intrin.h>
extern "C" void __cpuid(int *, int);
#pragma intrinsic(__cpuid)

// Windows ヘッダー ファイル:
#include <windows.h>

#include <uuids.h> // for MEDIATYPE_* and MEDIASUBTYPE_*

inline BOOL EnableDlgItem(HWND hwndParent, UINT nID, BOOL bEnable)
{
	return EnableWindow(GetDlgItem(hwndParent, nID), bEnable);
}

#include <queue>
#include <algorithm>
using namespace std;

#include <stdint.h>

inline uint32_t ROUNDUP(uint32_t a, uint32_t b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b は 2 の累乗である。
	return ((a + b - 1) / b) * b;
}

inline uint64_t ROUNDUP(uint64_t a, uint64_t b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b は 2 の累乗である。
	return ((a + b - 1) / b) * b;
}

inline bool IS_ALIGNED(uintptr_t v, uintptr_t a)
{
	_ASSERT(a > 0 && (a & (a - 1)) == 0); // a は 2 の累乗である。
	return (v & (a - 1)) == 0; // v は a の倍数である。
}

inline bool IS_ALIGNED(const void *p, uintptr_t a)
{
	return IS_ALIGNED((uintptr_t)p, a);
}
