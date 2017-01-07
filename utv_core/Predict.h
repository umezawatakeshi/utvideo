/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "SymbolBits.h"

void cpp_PredictWrongMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
void cpp_RestoreWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride);
void cpp_RestoreWrongMedianBlock4(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

#if defined(__i386__) || defined(__x86_64__)
extern "C" void sse2_PredictWrongMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
extern "C" void sse2_PredictLeftAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t *pCountTable);
extern "C" void i686_RestoreWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride);
extern "C" void sse1mmx_RestoreWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride);

extern "C" void sse2_RestoreWrongMedianBlock4(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

template<int B> void cpp_PredictCylindricalLeftAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, uint32_t *pCountTable);
template<int B> void cpp_RestoreCylindricalLeft(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd);

#define PredictCylindricalLeftAndCount10 cpp_PredictCylindricalLeftAndCount<10>
