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
#define HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXGreen
#define HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXBlue
#define HuffmanDecodeAndAccumStep4ForBottomupBGRXRed tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXRed
#define HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha
#define HuffmanDecodeAndAccumStep3ForBottomupBGRGreen tfn.pfnHuffmanDecodeAndAccumStep3ForBottomupBGRGreen
#define HuffmanDecodeAndAccumStep3ForBottomupBGRBlue tfn.pfnHuffmanDecodeAndAccumStep3ForBottomupBGRBlue
#define HuffmanDecodeAndAccumStep3ForBottomupBGRRed tfn.pfnHuffmanDecodeAndAccumStep3ForBottomupBGRRed
#define HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBGreen
#define HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBBlue
#define HuffmanDecodeAndAccumStep4ForTopdownXRGBRed tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBRed
#define HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha
#define HuffmanDecodeAndAccumStep3ForTopdownRGBGreen tfn.pfnHuffmanDecodeAndAccumStep3ForTopdownRGBGreen
#define HuffmanDecodeAndAccumStep3ForTopdownRGBBlue tfn.pfnHuffmanDecodeAndAccumStep3ForTopdownRGBBlue
#define HuffmanDecodeAndAccumStep3ForTopdownRGBRed tfn.pfnHuffmanDecodeAndAccumStep3ForTopdownRGBRed
#define HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha

size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForBottomupBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep3ForTopdownRGBRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
void cpp_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);

#if defined(__i386__) || defined(__x86_64__)
extern "C" size_t i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" void i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForBottomupBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForTopdownRGBRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
#endif
