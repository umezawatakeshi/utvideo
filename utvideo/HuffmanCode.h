/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

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
//size_t HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
#define HuffmanEncode tfn.pfnHuffmanEncode_align1
//void HuffmanDecode(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
#define HuffmanDecode tfn.pfnHuffmanDecode_align1
#define HuffmanDecodeAndAccum tfn.pfnHuffmanDecodeAndAccum_align1

size_t cpp_HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" size_t i686_HuffmanEncode_align1(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);

void cpp_HuffmanDecode(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecode_align1(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);

void cpp_HuffmanDecodeAndAccum(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecodeAndAccum_align1(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
