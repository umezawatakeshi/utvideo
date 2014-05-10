/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "SymbolBits.h"

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

template<int B>
struct HUFFMAN_CODELEN_TABLE
{
	uint8_t codelen[1 << B];
};

template<int B>
struct HUFFMAN_ENCODE_TABLE
{
	uintenc_t dwTableMux[1 << B];
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

#define HUFFMAN_DECODE_TABLELOOKUP_BITS10 11

struct HUFFMAN_DECODE_TABLE10
{
	struct
	{
		uint16_t bySymbol;
		uint8_t nCodeLength;
		uint8_t dummy;
	} LookupSymbolAndCodeLength[1<<HUFFMAN_DECODE_TABLELOOKUP_BITS10];
	uint8_t nCodeShift[32];
	uint32_t dwSymbolBase[32];
	struct
	{
		uint16_t bySymbol;
		uint8_t nCodeLength;
		uint8_t dummy;
	} SymbolAndCodeLength[4096*4];
};

template<int B> void GenerateHuffmanCodeLengthTable(HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable, const uint32_t *pCountTable);
template<int B> void GenerateHuffmanEncodeTable(HUFFMAN_ENCODE_TABLE<B> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable);
void GenerateHuffmanDecodeTable(HUFFMAN_DECODE_TABLE *pDecodeTable, const HUFFMAN_CODELEN_TABLE<8> *pCodeLengthTable);
void GenerateHuffmanDecodeTable10(HUFFMAN_DECODE_TABLE10 *pDecodeTable, const HUFFMAN_CODELEN_TABLE<10> *pCodeLengthTable);

template<int B> size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const typename CSymbolBits<B>::symbol_t *pSrcBegin, const typename CSymbolBits<B>::symbol_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<B> *pEncodeTable);
void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
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
extern "C" size_t i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable);
extern "C" void i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
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

#if defined(__x86_64__)
extern "C" void bmi2_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
#endif
#endif

#define HuffmanEncode10 cpp_HuffmanEncode<10>

void cpp_HuffmanDecodeAndAccum10(uint16_t *pDstBegin, uint16_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE10 *pDecodeTable, size_t cbWidth, ssize_t scbStride, uint16_t initial);
