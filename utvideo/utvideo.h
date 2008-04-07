/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008  UMEZAWA Takeshi
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
 * Copyright (C) 2008  梅澤 威志
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

#define UTVIDEO_ENCODER_VERSION 0x01000000 /* 1.0.0 */

struct BITMAPINFOEXT
{
	BITMAPINFOHEADER bih;
	DWORD dwEncoderVersion;
	DWORD fccOriginalFormat;
	DWORD dwFlags0;
	DWORD dwFlags1;
	DWORD dwFlags2;
	DWORD dwFlags3;
	DWORD dwFlags4;
	DWORD dwFlags5;
	DWORD dwFlags6;
	DWORD dwFlags7;
};

#define BIE_MIN_SIZE (sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 3)

#define BIE_FLAGS0_COMPRESS_MASK                0x00000001
#define BIE_FLAGS0_COMPRESS_NONE                0x00000000
#define BIE_FLAGS0_COMPRESS_HUFFMAN_CODE        0x00000001

#define BIE_FLAGS0_RESERVED                     0xfffffffe
#define BIE_FLAGS1_RESERVED                     0xffffffff
#define BIE_FLAGS2_RESERVED                     0xffffffff
#define BIE_FLAGS3_RESERVED                     0xffffffff
#define BIE_FLAGS4_RESERVED                     0xffffffff
#define BIE_FLAGS5_RESERVED                     0xffffffff
#define BIE_FLAGS6_RESERVED                     0xffffffff
#define BIE_FLAGS7_RESERVED                     0xffffffff


struct FRAMEHEADER
{
	DWORD dwFlags0;
	DWORD dwFlags1;
	DWORD dwFlags2;
	DWORD dwFlags3;
};

#define FH_FLAGS0_DELTAFRAME                    0x80000000

#define FH_FLAGS0_DIVIDE_COUNT_MASK             0x000000ff

#define FH_FLAGS0_INTRAFRAME_PREDICT_MASK       0x00000300
#define FH_FLAGS0_INTRAFRAME_PREDICT_NONE       0x00000000
#define FH_FLAGS0_INTRAFRAME_PREDICT_LEFT       0x00000100
#define FH_FLAGS0_INTRAFRAME_PREDICT_GRADIENT   0x00000200
#define FH_FLAGS0_INTRAFRAME_PREDICT_MEDIAN     0x00000300

#define FH_FLAGS0_RESERVED                      0x7ffffc00
#define FH_FLAGS1_RESERVED                      0xffffffff
#define FH_FLAGS2_RESERVED                      0xffffffff
#define FH_FLAGS3_RESERVED                      0xffffffff


inline DWORD ROUNDUP(DWORD a, DWORD b)
{
	return ((a + b - 1) / b) * b;
}
