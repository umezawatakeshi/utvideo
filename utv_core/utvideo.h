/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#ifndef FCC
#define FCC(fcc) ( \
	(((DWORD)(fcc) & 0x000000ff) << 24) | \
	(((DWORD)(fcc) & 0x0000ff00) <<  8) | \
	(((DWORD)(fcc) & 0x00ff0000) >>  8) | \
	(((DWORD)(fcc) & 0xff000000) >> 24))
#endif

#define FCC4PRINTF(fcc) \
	(BYTE)(fcc), \
	(BYTE)(fcc >> 8), \
	(BYTE)(fcc >> 16), \
	(BYTE)(fcc >> 24)

#define UTVIDEO_VERSION_MASK                      0xffffff00
#define UTVIDEO_VERSION                           0x09000300
#define UTVIDEO_VERSION_STR                       "9.0.3"

/*
 * NOTE:
 *  If you want to implement original version of encoder,
 *  you should register "implementation ID".
 *  Otherwise, you must use 0xff as ID.
 */
#define UTVIDEO_IMPLEMENTATION_MASK               0x000000ff
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN32_X86 0x00000000
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN64_X64 0x00000001
#define UTVIDEO_IMPLEMENTATION_NOT_REGISTERED     0x000000ff

#ifdef _WIN64
 #ifdef _M_X64
  #ifndef _WIN64_X64
   #define _WIN64_X64
  #endif
 #else
  #error This platform is not supported.
 #endif
#else
#ifdef _WIN32
 #ifdef _M_IX86
  #ifndef _WIN32_X86
   #define _WIN32_X86
  #endif
 #else
  #error This platform is not supported.
 #endif
#endif
#endif

#ifdef _WIN64_X64
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN64_X64
 #define UTVIDEO_IMPLEMENTATION_STR "x64"
#endif
#ifdef _WIN32_X86
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN32_X86
 #define UTVIDEO_IMPLEMENTATION_STR "x86"
#endif

#define UTVIDEO_VERSION_AND_IMPLEMENTATION (UTVIDEO_VERSION | UTVIDEO_IMPLEMENTATION)


inline DWORD ROUNDUP(DWORD a, DWORD b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b ‚Í 2 ‚Ì—İæ‚Å‚ ‚éB
	return ((a + b - 1) / b) * b;
}

inline bool IS_ALIGNED(DWORD_PTR v, DWORD_PTR a)
{
	_ASSERT(a > 0 && (a & (a - 1)) == 0); // a ‚Í 2 ‚Ì—İæ‚Å‚ ‚éB
	return (v & (a - 1)) == 0; // v ‚Í a ‚Ì”{”‚Å‚ ‚éB
}

inline bool IS_ALIGNED(const void *p, DWORD_PTR a)
{
	return IS_ALIGNED((DWORD_PTR)p, a);
}

inline BOOL EnableDlgItem(HWND hwndParent, UINT nID, BOOL bEnable)
{
	return EnableWindow(GetDlgItem(hwndParent, nID), bEnable);
}

#ifdef _DEBUG
class DebugEnterLeave
{
private:
	char buf[256];
public:
	DebugEnterLeave(char *fmt, ...)
	{
		va_list argptr;
		va_start(argptr, fmt);
#pragma warning(push)
#pragma warning(disable:4996)
		vsnprintf(buf, _countof(buf), fmt, argptr);
#pragma warning(pop)
		va_end(argptr);
		_RPT1(_CRT_WARN, "enter %s\n", buf);
	}

	~DebugEnterLeave()
	{
		_RPT1(_CRT_WARN, "leave %s\n", buf);
	}
};
#define DEBUG_ENTER_LEAVE(...) class DebugEnterLeave debug_enter_leave_object(__VA_ARGS__)
#else
#define DEBUG_ENTER_LEAVE(...)
#endif
