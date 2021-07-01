/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

#ifdef _WIN32

#define NOMINMAX 1

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

// Windows ヘッダー ファイル:
#include <windows.h>
#include <commctrl.h>

inline BOOL EnableDlgItem(HWND hwndParent, UINT nID, BOOL bEnable)
{
	return EnableWindow(GetDlgItem(hwndParent, nID), bEnable);
}

#include <stdio.h>

#endif

#if defined(__APPLE__) || defined(__unix__)
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

#ifndef _MSC_VER // XXX
#define _countof(x) (sizeof(x) / sizeof((x)[0]))
#define _ASSERT(x) do {} while(/*CONSTCOND*/0)
#endif

#include <queue>
#include <algorithm>
#include <memory>
#include <mutex>

#include <stdint.h>
#include <myinttypes.h>

static inline unsigned int ROUNDUP(unsigned int a, unsigned int b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b は 2 の累乗である。
	return ((a + b - 1) / b) * b;
}

static inline unsigned long ROUNDUP(unsigned long a, unsigned long b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b は 2 の累乗である。
	return ((a + b - 1) / b) * b;
}

static inline unsigned long long ROUNDUP(unsigned long long a, unsigned long long b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b は 2 の累乗である。
	return ((a + b - 1) / b) * b;
}

static inline bool IS_ALIGNED(const void *p, uintptr_t a)
{
	_ASSERT(a > 0 && (a & (a - 1)) == 0); // a は 2 の累乗である。
	return (((uintptr_t)p) & (a - 1)) == 0; // v は a の倍数である。
}

extern void* enabler;

template<typename T, typename U, std::enable_if_t<std::is_integral_v<T>&& std::is_integral_v<U>>*& = enabler>
static inline bool IS_MULTIPLE(T x, U a)
{
	_ASSERT(a > 0);
	return x % a == 0;
}

template<typename U, std::enable_if_t<std::is_integral_v<U>>*& = enabler>
static inline bool IS_MULTIPLE(const void* x, const void* y, U a)
{
	return IS_MULTIPLE((char*)x - (char*)y, a);
}

template<typename Iterator>
static inline bool is_not_all_zero(Iterator begin, Iterator end)
{
	return std::find_if_not(begin, end, [](auto x) { return x == 0; }) != end;
}

template<typename Container>
static inline bool is_not_all_zero(const Container& c)
{
	return is_not_all_zero(std::begin(c), std::end(c));
}

#define LOG_MODULE_NAME "utv_core"
#define LOGWRITER_OWNER 1
#include <LogWriter.h>
#include <LogUtil.h>


using std::max;
using std::min;

#if defined(_MSC_VER)
#define FORCEINLINE __forceinline
#define VECTORCALL __vectorcall
#elif defined(__GNUC__)
#define FORCEINLINE __attribute__((always_inline))
#define VECTORCALL
#else
#error
#endif
