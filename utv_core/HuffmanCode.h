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
#define HuffmanDecodeAndAccumStep3 tfn.pfnHuffmanDecodeAndAccumStep3
#define HuffmanDecodeAndAccumStep4 tfn.pfnHuffmanDecodeAndAccumStep4
#define HuffmanDecodeAndAccumStep3ForBGRBlue tfn.pfnHuffmanDecodeAndAccumStep3ForBGRBlue
#define HuffmanDecodeAndAccumStep3ForBGRRed tfn.pfnHuffmanDecodeAndAccumStep3ForBGRRed
#define HuffmanDecodeAndAccumStep4ForBGRXBlue tfn.pfnHuffmanDecodeAndAccumStep4ForBGRXBlue
#define HuffmanDecodeAndAccumStep4ForBGRXRed tfn.pfnHuffmanDecodeAndAccumStep4ForBGRXRed
#define HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha
#define HuffmanDecodeAndAccumStep3ForRGBBlue HuffmanDecodeAndAccumStep3ForBGRRed
#define HuffmanDecodeAndAccumStep3ForRGBRed HuffmanDecodeAndAccumStep3ForBGRBlue
#define HuffmanDecodeAndAccumStep4ForXRGBBlue HuffmanDecodeAndAccumStep4ForBGRXRed
#define HuffmanDecodeAndAccumStep4ForXRGBRed HuffmanDecodeAndAccumStep4ForBGRXBlue
#define HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha

size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);

#if defined(__i386__) || defined(__x86_64__)
extern "C" size_t i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
extern "C" void i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
#endif
