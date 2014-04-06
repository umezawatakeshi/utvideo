/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Predict.h"
#include "TunedFunc.h"

template<class T>
inline T median(T a, T b, T c)
{
	return max(min(max(a,b),c),min(a,b));
}

void cpp_PredictWrongMedianAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable)
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

void cpp_PredictLeftAndCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t *pCountTable)
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

void cpp_RestoreWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride)
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

void cpp_RestoreWrongMedianBlock4(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pSrcStripeBegin;
	uint8_t *pDstStripeBegin;
	const uint8_t *p;
	uint8_t *q;
	uint8_t left[4] = { 0, 0, 0, 0 };
	uint8_t topleft[4] = { 0, 0, 0, 0 };

	pDst[0] = pSrcBegin[0] + 0x80;
	pDst[1] = pSrcBegin[1] + 0x80;
	pDst[2] = pSrcBegin[2] + 0x80;
	pDst[3] = pSrcBegin[3] + 0x80;

	for (p = pSrcBegin + 4, q = pDst + 4; p < pSrcBegin + cbWidth; p += 4, q += 4)
	{
		q[0] = p[0] + q[-4];
		q[1] = p[1] + q[-3];
		q[2] = p[2] + q[-2];
		q[3] = p[3] + q[-1];
	}

	for (pSrcStripeBegin = pSrcBegin + scbStride, pDstStripeBegin = pDst + scbStride; pSrcStripeBegin != pSrcEnd; pSrcStripeBegin += scbStride, pDstStripeBegin += scbStride)
	{
		for (p = pSrcStripeBegin, q = pDstStripeBegin; p < pSrcStripeBegin + cbWidth; p += 4, q += 4)
		{
			q[0] = p[0] + median<uint8_t>(q[0-scbStride], left[0], q[0-scbStride] + left[0] - topleft[0]); left[0] = q[0]; topleft[0] = q[0-scbStride];
			q[1] = p[1] + median<uint8_t>(q[1-scbStride], left[1], q[1-scbStride] + left[1] - topleft[1]); left[1] = q[1]; topleft[1] = q[1-scbStride];
			q[2] = p[2] + median<uint8_t>(q[2-scbStride], left[2], q[2-scbStride] + left[2] - topleft[2]); left[2] = q[2]; topleft[2] = q[2-scbStride];
			q[3] = p[3] + median<uint8_t>(q[3-scbStride], left[3], q[3-scbStride] + left[3] - topleft[3]); left[3] = q[3]; topleft[3] = q[3-scbStride];
		}
	}
}


void PredictCylindricalLeftAndCount10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, uint32_t *pCountTable)
{
	const uint16_t *p = pSrcBegin;
	uint16_t *q = pDst;

	*q = *p;
	pCountTable[*q]++;
	p++;
	q++;

	// 残りのピクセルが predict left の本番
	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*p - *(p-1)) & 0x3ff;
		pCountTable[*q]++;
	}
}
