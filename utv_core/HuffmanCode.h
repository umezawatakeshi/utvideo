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
struct SYMBOL_AND_CODELEN
{
	symbol_t<B> symbol;
	uint8_t codelen;
};

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

template<int B>
struct HUFFMAN_DECODE_TABLE
{
	static const int LOOKUP_BITS = (B == 8) ? 12 : 11; /* XXX */

	SYMBOL_AND_CODELEN<B> LookupSymbolAndCodeLength[1 << LOOKUP_BITS];
	uint8_t nCodeShift[32];
	uint32_t dwSymbolBase[32];
	SYMBOL_AND_CODELEN<B> SymbolAndCodeLength[(B == 8) ? 1024 : 4096*4 /* XXX */];
};

template<int B> void GenerateHuffmanCodeLengthTable(HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable, const uint32_t *pCountTable);
template<int B> void GenerateHuffmanEncodeTable(HUFFMAN_ENCODE_TABLE<B> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable);
template<int B> void GenerateHuffmanDecodeTable(HUFFMAN_DECODE_TABLE<B> *pDecodeTable, const HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable);

template<int B> size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, const HUFFMAN_ENCODE_TABLE<B> *pEncodeTable);
void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
void cpp_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);

#if defined(__i386__) || defined(__x86_64__)
extern "C" size_t i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable);
extern "C" void i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void i686_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);

#if defined(__x86_64__)
extern "C" void bmi2_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
extern "C" void bmi2_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
#endif
#endif

#define HuffmanEncode10 cpp_HuffmanEncode<10>

void cpp_HuffmanDecodeAndAccum10(uint16_t *pDstBegin, uint16_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable, size_t cbWidth, ssize_t scbStride, uint16_t initial);
