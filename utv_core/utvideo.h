/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#ifndef FCC
#define FCC(fcc) ( \
	(((uint32_t)(fcc) & 0x000000ff) << 24) | \
	(((uint32_t)(fcc) & 0x0000ff00) <<  8) | \
	(((uint32_t)(fcc) & 0x00ff0000) >>  8) | \
	(((uint32_t)(fcc) & 0xff000000) >> 24))
#endif

#define UNFCC(fcc) ( \
	(((uint32_t)(fcc) & 0x000000ff) << 24) | \
	(((uint32_t)(fcc) & 0x0000ff00) <<  8) | \
	(((uint32_t)(fcc) & 0x00ff0000) >>  8) | \
	(((uint32_t)(fcc) & 0xff000000) >> 24))

#ifdef _MSC_VER
#ifndef _SSIZE_T_DEFINED
typedef ptrdiff_t ssize_t;
#define _SSIZE_T_DEFINED
#endif
#endif


#define UTVIDEO_VERSION_MASK                      0xffffff00
#define UTVIDEO_VERSION                           0x0c000100
#define UTVIDEO_VERSION_STR                       "12.0.1"

/*
 * NOTE:
 *  If you want to implement original version of encoder,
 *  you should register "implementation ID".
 *  Otherwise, you must use 0xff as ID.
 */
#define UTVIDEO_IMPLEMENTATION_MASK                 0x000000ff
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN32_X86   0x00000000
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN64_X64   0x00000001
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_MACOSX_I386 0x00000011
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_POSIX       0x00000020
#define UTVIDEO_IMPLEMENTATION_LIBAVCODEC           0x000000f0
#define UTVIDEO_IMPLEMENTATION_NOT_REGISTERED       0x000000ff

#ifdef _WIN64
 #ifdef _M_X64
  #define _WIN64_X64 1
  #ifndef __x86_64__
   #define __x86_64__ 1
  #endif
 #endif
#else
#ifdef _WIN32
 #ifdef _M_IX86
  #define _WIN32_X86 1
  #ifndef __i386__
   #define __i386__ 1
  #endif
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
#ifdef __APPLE__ // XXX
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_MACOSX_I386
 #define UTVIDEO_IMPLEMENTATION_STR "i386"
#endif
#ifdef __unix__
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_POSIX
 #define UTVIDEO_IMPLEMENTATION_STR "posix"
#endif

#ifndef UTVIDEO_IMPLEMENTATION
#error This platform is not supported.
#endif

#define UTVIDEO_VERSION_AND_IMPLEMENTATION (UTVIDEO_VERSION | UTVIDEO_IMPLEMENTATION)

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif


typedef uint32_t utvf_t;

#define UTVF_INVALID 0

// Non-FourCC RGB formats
//      UTVF_NFCC_RGB_MASK_EFFECTIVE_BITCOUNT 0x000000ff
//      UTVF_NFCC_RGB_FLAG_HAVE_DUMMY_CHANNEL 0x00000100
//      UTVF_NFCC_RGB_FLAG_BOTTOMUP           0x00000200
//      UTVF_NFCC_RGB_FLAG_ORDER_BGR          0x00000400
#define UTVF_NFCC_RGB_TD             ((utvf_t)0x00000018)
#define UTVF_NFCC_ARGB_TD            ((utvf_t)0x00000020)
#define UTVF_NFCC_BGR_TD             ((utvf_t)0x00000418)
#define UTVF_NFCC_BGRX_TD            ((utvf_t)0x00000518)
#define UTVF_NFCC_BGRA_TD            ((utvf_t)0x00000420)
#define UTVF_NFCC_BGR_BU             ((utvf_t)0x00000618)
#define UTVF_NFCC_BGRX_BU            ((utvf_t)0x00000718)
#define UTVF_NFCC_BGRA_BU            ((utvf_t)0x00000620)

// FourCC formats
#define UTVF_ULRA ((utvf_t)'ULRA')
#define UTVF_ULRG ((utvf_t)'ULRG')
#define UTVF_ULY2 ((utvf_t)'ULY2')
#define UTVF_ULY0 ((utvf_t)'ULY0')

#define UTVF_YUY2 ((utvf_t)'YUY2')
#define UTVF_YUYV ((utvf_t)'YUYV')
#define UTVF_YUNV ((utvf_t)'YUNV')
#define UTVF_UYVY ((utvf_t)'UYVY')
#define UTVF_UYNV ((utvf_t)'UYNV')
#define UTVF_YV12 ((utvf_t)'YV12')


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
