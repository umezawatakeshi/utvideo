/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "Predict.h"
#include "TunedFunc.h"

inline uint8_t median(uint8_t a, uint8_t b, uint8_t c)
{
	return max(min(max(a,b),c),min(a,b));
}

void PredictMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, DWORD *pCountTable)
{
	if (IS_ALIGNED(pDst, 16) && IS_ALIGNED(pSrcBegin, 16) && IS_ALIGNED(dwStride, 16))
		tfn.pfnPredictMedianAndCount_align16(pDst, pSrcBegin, pSrcEnd, dwStride, pCountTable);
	else
		tfn.pfnPredictMedianAndCount_align1(pDst, pSrcBegin, pSrcEnd, dwStride, pCountTable);
}

void cpp_PredictMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, DWORD *pCountTable)
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
		*q = *p - median(*(p - dwStride), *(p - 1), *(p - dwStride) + *(p - 1) - *(p - 1 - dwStride));
		pCountTable[*q]++;
	}
}

void cpp_PredictLeftAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, DWORD *pCountTable)
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	// 最初のラインの最初のピクセルは 0x80 を予測しておく。
	*q = *p - 0x80;
	pCountTable[*q]++;
	p++;
	q++;

	// 残りのピクセルが predict left の本番
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - *(p-1);
		pCountTable[*q]++;
	}
}

void cpp_RestoreMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride)
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
		*q = *p + median(*(q - dwStride), *(q - 1), *(q - dwStride) + *(q - 1) - *(q - 1 - dwStride));
	}
}
