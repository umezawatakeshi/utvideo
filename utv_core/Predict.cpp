/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Predict.h"
#include "TunedFunc.h"

template<class T>
static inline T median(T a, T b, T c)
{
	return max(min(max(a,b),c),min(a,b));
}

void cpp_PredictCylindricalWrongMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable)
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	// 最初のラインの最初のピクセルは 0x80 を予測しておく。
	*q = *p - 0x80;
	pCountTable[*q]++;
	p++;
	q++;

	// 最初のラインの残りのピクセルは predict left と同じ。
	for (; p < pSrcBegin + dwStride; p++, q++)
	{
		*q = *p - *(p - 1);
		pCountTable[*q]++;
	}

	// 1 ラインしかない場合はここで終了。
	if (p == pSrcEnd)
		return;

	// 次のラインの最初のピクセルは predict above。
	// こうしておくとアセンブラ化した時に処理が若干簡単になる。
	*q = *p - *(p - dwStride);
	pCountTable[*q]++;
	p++;
	q++;

	// 残りのピクセルが predict median の本番
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - median<uint8_t>(*(p - dwStride), *(p - 1), *(p - dwStride) + *(p - 1) - *(p - 1 - dwStride));
		pCountTable[*q]++;
	}
}

void cpp_RestoreCylindricalWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride)
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	*q++ = *p++ + 0x80;

	for (; p < pSrcBegin + dwStride; p++, q++)
	{
		*q = *p + *(q - 1);
	}

	if (p == pSrcEnd)
		return;

	*q = *p + *(q - dwStride);
	p++;
	q++;

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p + median<uint8_t>(*(q - dwStride), *(q - 1), *(q - dwStride) + *(q - 1) - *(q - 1 - dwStride));
	}
}


template<int B>
void cpp_PredictCylindricalLeftAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, uint32_t *pCountTable)
{
	const symbol_t<B> *p = pSrcBegin;
	symbol_t<B> *q = pDst;

	*q = (*p - CSymbolBits<B>::midval) & CSymbolBits<B>::maskval;
	pCountTable[*q]++;
	p++;
	q++;

	// 残りのピクセルが predict left の本番
	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*p - *(p-1)) & CSymbolBits<B>::maskval;
		pCountTable[*q]++;
	}
}

template void cpp_PredictCylindricalLeftAndCount<8>(symbol_t<8> *pDst, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd, uint32_t *pCountTable);
template void cpp_PredictCylindricalLeftAndCount<10>(symbol_t<10> *pDst, const symbol_t<10> *pSrcBegin, const symbol_t<10> *pSrcEnd, uint32_t *pCountTable);


template<int B>
void cpp_RestoreCylindricalLeft(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd)
{
	const symbol_t<B> *p = pSrcBegin;
	symbol_t<B> *q = pDst;

	*q = (*p + CSymbolBits<B>::midval) & CSymbolBits<B>::maskval;
	p++;
	q++;

	// 残りのピクセルが predict left の本番
	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*(q - 1) + *p) & CSymbolBits<B>::maskval;
	}
}

template void cpp_RestoreCylindricalLeft<8>(symbol_t<8> *pDst, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd);
template void cpp_RestoreCylindricalLeft<10>(symbol_t<10> *pDst, const symbol_t<10> *pSrcBegin, const symbol_t<10> *pSrcEnd);
