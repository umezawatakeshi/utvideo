/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once
#include "TunedFunc.h"

struct HUFFMAN_ENCODE_TABLE
{
	DWORD dwTableMux[256];
};

#define HUFFMAN_DECODE_TABLELOOKUP_BITS 12

struct HUFFMAN_DECODE_TABLE
{
	struct
	{
		uint8_t bySymbol;
		uint8_t nCodeLength;
	} LookupSymbolAndCodeLength[1<<HUFFMAN_DECODE_TABLELOOKUP_BITS];
	uint8_t nCodeShift[32];
	DWORD dwSymbolBase[32];
	struct
	{
		uint8_t bySymbol;
		uint8_t nCodeLength;
	} SymbolAndCodeLength[1024];
};

void GenerateHuffmanCodeLengthTable(uint8_t *pCodeLengthTable, const DWORD *pCountTable);
void GenerateHuffmanEncodeTable(HUFFMAN_ENCODE_TABLE *pEncodeTable, const uint8_t *pCodeLengthTable);
void GenerateHuffmanDecodeTable(HUFFMAN_DECODE_TABLE *pDecodeTable, const uint8_t *pCodeLengthTable);

#define HuffmanEncode tfn.pfnHuffmanEncode
#define HuffmanDecode tfn.pfnHuffmanDecode
#define HuffmanDecodeAndAccum tfn.pfnHuffmanDecodeAndAccum
#define HuffmanDecodeAndAccumStep2 tfn.pfnHuffmanDecodeAndAccumStep2
#define HuffmanDecodeAndAccumStep4 tfn.pfnHuffmanDecodeAndAccumStep4
#define HuffmanDecodeAndAccumStep4ForBottomupRGB32Green tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32Green
#define HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32Blue
#define HuffmanDecodeAndAccumStep4ForBottomupRGB32Red tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32Red
#define HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha
#define HuffmanDecodeAndAccumStep3ForBottomupRGB24Green tfn.pfnHuffmanDecodeAndAccumStep3ForBottomupRGB24Green
#define HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue tfn.pfnHuffmanDecodeAndAccumStep3ForBottomupRGB24Blue
#define HuffmanDecodeAndAccumStep3ForBottomupRGB24Red tfn.pfnHuffmanDecodeAndAccumStep3ForBottomupRGB24Red

size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);

#ifdef _WIN32_X86
extern "C" size_t x86_i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" void x86_i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
#endif

#ifdef _WIN64_X64
extern "C" size_t x64_i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" void x64_i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
#endif
