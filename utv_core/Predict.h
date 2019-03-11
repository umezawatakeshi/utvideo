/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "SymbolBits.h"

void cpp_PredictCylindricalWrongMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t *pCountTable);
void cpp_RestoreCylindricalWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);

template<int B> void cpp_PredictCylindricalLeftAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, uint32_t *pCountTable);
template<int B> void cpp_RestoreCylindricalLeft(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd);
template<int B> void cpp_PredictPlanarGradientAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride, uint32_t *pCountTable);
template<int B> void cpp_PredictPlanarGradient(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride);
template<int B> void cpp_RestorePlanarGradient(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride);

template<class T>
static inline T median(T a, T b, T c)
{
	return max(min(max(a, b), c), min(a, b));
}

enum PREDICTION_TYPE
{
	CYLINDRICAL_LEFT,
	PLANAR_GRADIENT,
	CYLINDRICAL_WRONG_MEDIAN,
};
