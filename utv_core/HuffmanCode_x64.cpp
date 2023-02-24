/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include <stdlib.h>
#include <stdint.h>
#include <type_traits>
#include "HuffmanCode.h"
#include "TunedFunc_x86x64.h"

extern "C" size_t i686_HuffmanEncode8(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable)
{
#define ARGS "byte, 1, word, 2, 1"
#include "HuffmanCode_x64_HuffmanEncode.cpp"
}

extern "C" size_t i686noshld_HuffmanEncode8(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable)
{
#define ARGS "byte, 1, word, 2, 0"
#include "HuffmanCode_x64_HuffmanEncode.cpp"
}

extern "C" size_t i686hybrid_HuffmanEncode8(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable)
{
#ifdef _WIN32
	DWORD iProcessorNumber = GetCurrentProcessorNumber();
#else
#error
#endif
	if (iProcessorNumber >= sizeof(u8CoreTypeMap) || u8CoreTypeMap[iProcessorNumber] != CORE_TYPE_INTEL_ATOM)
		return i686_HuffmanEncode8(pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable);
	else
		return i686noshld_HuffmanEncode8(pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable);
}

extern "C" size_t i686_HuffmanEncode10(uint8_t *pDstBegin, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<10> *pEncodeTable)
{
#define ARGS "word, 2, word, 2, 1"
#include "HuffmanCode_x64_HuffmanEncode.cpp"
}

extern "C" size_t i686noshld_HuffmanEncode10(uint8_t *pDstBegin, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<10> *pEncodeTable)
{
#define ARGS "word, 2, word, 2, 0"
#include "HuffmanCode_x64_HuffmanEncode.cpp"
}

extern "C" size_t i686hybrid_HuffmanEncode10(uint8_t *pDstBegin, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<10> *pEncodeTable)
{
#ifdef _WIN32
	DWORD iProcessorNumber = GetCurrentProcessorNumber();
#else
#error
#endif
	if (iProcessorNumber >= sizeof(u8CoreTypeMap) || u8CoreTypeMap[iProcessorNumber] != CORE_TYPE_INTEL_ATOM)
		return i686_HuffmanEncode10(pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable);
	else
		return i686noshld_HuffmanEncode10(pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable);
}

extern "C" uint8_t *i686_HuffmanDecode8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable)
{
#define ARGS "0"
#include "HuffmanCode_x64_HuffmanDecode.cpp"
}

extern "C" uint8_t *bmi2_HuffmanDecode8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable)
{
#define ARGS "1"
#include "HuffmanCode_x64_HuffmanDecode.cpp"
}

extern "C" uint16_t *i686_HuffmanDecode10(uint16_t *pDstBegin, uint16_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable)
{
#define ARGS "0"
#include "HuffmanCode_x64_HuffmanDecode.cpp"
}

extern "C" uint16_t *bmi2_HuffmanDecode10(uint16_t *pDstBegin, uint16_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable)
{
#define ARGS "1"
#include "HuffmanCode_x64_HuffmanDecode.cpp"
}
