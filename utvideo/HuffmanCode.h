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
#include "TunedFunc.h"

struct HUFFMAN_ENCODE_TABLE
{
	DWORD dwTableMux[256];
};

struct HUFFMAN_DECODE_TABLE
{
	BYTE nCodeShift[32];
	DWORD dwSymbolBase[32];
	struct
	{
		BYTE bySymbol;
		BYTE nCodeLength;
		WORD padding;
	} SymbolAndCodeLength[1024];
};

void GenerateHuffmanCodeLengthTable(BYTE *pCodeLengthTable, const DWORD *pCountTable);
void GenerateHuffmanEncodeTable(HUFFMAN_ENCODE_TABLE *pEncodeTable, const BYTE *pCodeLengthTable);
void GenerateHuffmanDecodeTable(HUFFMAN_DECODE_TABLE *pDecodeTable, const BYTE *pCodeLengthTable);
//DWORD HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
#define HuffmanEncode tfn.pfnHuffmanEncode_align1
//void HuffmanDecode(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
#define HuffmanDecode tfn.pfnHuffmanDecode_align1
#define HuffmanDecodeAndAccum tfn.pfnHuffmanDecodeAndAccum_align1

DWORD cpp_HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" DWORD i686_HuffmanEncode_align1(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);

void cpp_HuffmanDecode(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecode_align1(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);

void cpp_HuffmanDecodeAndAccum(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecodeAndAccum_align1(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);

DWORD cpp_HuffmanEncodeFirstRawYUY2(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
DWORD cpp_HuffmanEncodeFirstRawUYUV(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
DWORD cpp_HuffmanEncodeFirstRawYVYU(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
DWORD cpp_HuffmanEncodeFirstRawVYUY(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
DWORD cpp_HuffmanEncodeFirstRawWithDiffYUY2(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
DWORD cpp_HuffmanEncodeFirstRawWithDiffUYUV(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
DWORD cpp_HuffmanEncodeFirstRawWithDiffYVYU(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
DWORD cpp_HuffmanEncodeFirstRawWithDiffVYUY(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);

void cpp_HuffmanDecodeFirstRawYUY2(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeFirstRawUYVY(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeFirstRawYVYU(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeFirstRawVYUY(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeFirstRawWithAccumYUY2(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeFirstRawWithAccumUYVY(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeFirstRawWithAccumYVYU(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeFirstRawWithAccumVYUY(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
