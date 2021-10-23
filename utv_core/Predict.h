/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once

#include "SymbolBits.h"

template<int B> static constexpr int NUM_COUNT_TABLES_PER_CHANNEL = -1;
template<> static constexpr int NUM_COUNT_TABLES_PER_CHANNEL<8> = 8;
template<> static constexpr int NUM_COUNT_TABLES_PER_CHANNEL<10> = 2;

void cpp_PredictCylindricalWrongMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
void cpp_PredictCylindricalWrongMedian(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbStride);
void cpp_RestoreCylindricalWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);

template<int B> void cpp_PredictCylindricalLeftAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, uint32_t pCountTable[][1 << B]);
template<int B> void cpp_RestoreCylindricalLeft(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd);
template<int B> void cpp_PredictPlanarGradientAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride, uint32_t pCountTable[][1 << B]);
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
