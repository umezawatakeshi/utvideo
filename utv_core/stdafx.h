/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once

#ifdef _WIN32

#define NOMINMAX 1

// ���Ŏw�肳�ꂽ��`�̑O�ɑΏۃv���b�g�t�H�[�����w�肵�Ȃ���΂Ȃ�Ȃ��ꍇ�A�ȉ��̒�`��ύX���Ă��������B
// �قȂ�v���b�g�t�H�[���ɑΉ�����l�Ɋւ���ŐV���ɂ��ẮAMSDN ���Q�Ƃ��Ă��������B
#ifndef WINVER				// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define WINVER 0x0501		// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINNT		// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B                   
#define _WIN32_WINNT 0x0501	// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_WINDOWS 0x0410 // ����� Windows Me �܂��͂���ȍ~�̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_IE			// IE 6.0 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_IE 0x0600	// ����� IE. �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#define WIN32_LEAN_AND_MEAN		// Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
#define _CRT_SECURE_NO_WARNINGS

// Windows �w�b�_�[ �t�@�C��:
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
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b �� 2 �̗ݏ�ł���B
	return ((a + b - 1) / b) * b;
}

static inline unsigned long ROUNDUP(unsigned long a, unsigned long b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b �� 2 �̗ݏ�ł���B
	return ((a + b - 1) / b) * b;
}

static inline unsigned long long ROUNDUP(unsigned long long a, unsigned long long b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b �� 2 �̗ݏ�ł���B
	return ((a + b - 1) / b) * b;
}

static inline bool IS_ALIGNED(const void *p, uintptr_t a)
{
	_ASSERT(a > 0 && (a & (a - 1)) == 0); // a �� 2 �̗ݏ�ł���B
	return (((uintptr_t)p) & (a - 1)) == 0; // v �� a �̔{���ł���B
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
