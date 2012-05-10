/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "TunedFunc.h"

#if defined(__x86_64__)
typedef uint64_t uintenc_t;
#define UINTENC_BITS 64
#define UINTENC_MSB  0x8000000000000000ULL
#define UINTENC_MASK 0xffffffffffffff00ULL
#else
typedef uint32_t uintenc_t;
#define UINTENC_BITS 32
#define UINTENC_MSB  0x80000000U
#define UINTENC_MASK 0xffffff00U
#endif

struct HUFFMAN_ENCODE_TABLE
{
	uintenc_t dwTableMux[256];
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
	uint32_t dwSymbolBase[32];
	struct
	{
		uint8_t bySymbol;
		uint8_t nCodeLength;
	} SymbolAndCodeLength[1024];
};

void GenerateHuffmanCodeLengthTable(uint8_t *pCodeLengthTable, const uint32_t *pCountTable);
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
#define HuffmanDecodeAndAccumStep4ForTopdownRGB32Green tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownRGB32Green
#define HuffmanDecodeAndAccumStep4ForTopdownRGB32Blue tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownRGB32Blue
#define HuffmanDecodeAndAccumStep4ForTopdownRGB32Red tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownRGB32Red
#define HuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha
#define HuffmanDecodeAndAccumStep3ForTopdownRGB24Green tfn.pfnHuffmanDecodeAndAccumStep3ForTopdownRGB24Green
#define HuffmanDecodeAndAccumStep3ForTopdownRGB24Blue tfn.pfnHuffmanDecodeAndAccumStep3ForTopdownRGB24Blue
#define HuffmanDecodeAndAccumStep3ForTopdownRGB24Red tfn.pfnHuffmanDecodeAndAccumStep3ForTopdownRGB24Red

size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForTopdownRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForTopdownRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForTopdownRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);

#ifdef __i386__
extern "C" size_t x86_i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" void x86_i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
#endif

#ifdef __x86_64__
extern "C" size_t x64_i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" void x64_i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Green(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Blue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Red(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
#endif
