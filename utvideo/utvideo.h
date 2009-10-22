/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008-2009  UMEZAWA Takeshi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * 
 * Ut Video Codec Suite
 * Copyright (C) 2008-2009  梅澤 威志
 * 
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフ
 * トウェア財団によって発行された GNU 一般公衆利用許諾契約書(バージョ
 * ン2か、希望によってはそれ以降のバージョンのうちどれか)の定める条件
 * の下で再頒布または改変することができます。
 * 
 * このプログラムは有用であることを願って頒布されますが、*全くの無保
 * 証* です。商業可能性の保証や特定の目的への適合性は、言外に示された
 * ものも含め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご
 * 覧ください。
 * 
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を
 * 一部受け取ったはずです。もし受け取っていなければ、フリーソフトウェ
 * ア財団まで請求してください(宛先は the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)。
 */

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
#define UTVIDEO_VERSION                           0x07000000 /* 7.0.0 */

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
  #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN64_X64
  #define UTVIDEO_IMPLEMENTATION_STR "x64"
  #ifndef _WIN64_X64
   #define _WIN64_X64
  #endif
 #else
  #error This platform is not supported.
 #endif
#else
#ifdef _WIN32
 #ifdef _M_IX86
  #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN32_X86
  #define UTVIDEO_IMPLEMENTATION_STR "x86"
  #ifndef _WIN32_X86
   #define _WIN32_X86
  #endif
 #else
  #error This platform is not supported.
 #endif
#endif
#endif

#define UTVIDEO_VERSION_AND_IMPLEMENTATION (UTVIDEO_VERSION | UTVIDEO_IMPLEMENTATION)

extern HINSTANCE hModule;

struct BITMAPINFOEXT
{
	BITMAPINFOHEADER bih;
	DWORD dwEncoderVersionAndImplementation;
	DWORD fccOriginalFormat;
	DWORD dwFrameInfoSize;
	DWORD dwFlags0;
};

#define BIE_FLAGS0_DIVIDE_COUNT_MASK             0xff000000
#define BIE_FLAGS0_DIVIDE_COUNT_SHIFT            24

#define BIE_FLAGS0_COMPRESS_MASK                 0x00000001
#define BIE_FLAGS0_COMPRESS_NONE                 0x00000000
#define BIE_FLAGS0_COMPRESS_HUFFMAN_CODE         0x00000001

#define BIE_FLAGS0_ASSUME_INTERLACE              0x00000800

#define BIE_FLAGS0_RESERVED                      0x00fff7fe


struct FRAMEINFO
{
	DWORD dwFlags0;
};

#define FI_FLAGS0_INTRAFRAME_PREDICT_MASK        0x00000300
#define FI_FLAGS0_INTRAFRAME_PREDICT_NONE        0x00000000
#define FI_FLAGS0_INTRAFRAME_PREDICT_LEFT        0x00000100
#define FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT    0x00000200
#define FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN      0x00000300

#define FI_FLAGS0_RESERVED                       0xfffffcff


struct ENCODERCONF
{
	DWORD dwFlags0;
};

#define EC_FLAGS0_DIVIDE_COUNT_MASK              0x000000ff

#define EC_FLAGS0_INTRAFRAME_PREDICT_MASK        0x00000300
#define EC_FLAGS0_INTRAFRAME_PREDICT_RESERVED    0x00000000
#define EC_FLAGS0_INTRAFRAME_PREDICT_LEFT        0x00000100
#define EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN      0x00000300

#define EC_FLAGS0_ASSUME_INTERLACE               0x00000800
#define EC_FLAGS0_DIVIDE_COUNT_IS_NUM_PROCESSORS 0x00001000

#define EC_FLAGS0_RESERVED                       0xffffe400


inline DWORD ROUNDUP(DWORD a, DWORD b)
{
	return ((a + b - 1) / b) * b;
}

inline bool IS_ALIGNED(DWORD_PTR v, DWORD_PTR a)
{
	_ASSERT(
		a == 2 ||
		a == 4 ||
		a == 8 ||
		a == 16 ||
		a == 32 ||
		a == 64 ||
		a == 128 ||
		a == 256);
	return (v & (a - 1)) == 0;
}

inline bool IS_ALIGNED(const void *p, DWORD_PTR a)
{
	return IS_ALIGNED((DWORD_PTR)p, a);
}

inline BOOL EnableDlgItem(HWND hwndParent, UINT nID, BOOL bEnable)
{
	return EnableWindow(GetDlgItem(hwndParent, nID), bEnable);
}
