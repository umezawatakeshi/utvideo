/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008  UMEZAWA Takeshi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * 
 * Ut Video Codec Suite
 * Copyright (C) 2008  梅澤 威志
 * 
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフ
 * トウェア財団によって発行された GNU 一般公衆利用許諾契約書(バージョ
 * ン2か、希望によってはそれ以降のバージョンのうちどれか)の定める条件
 * の下で再頒布または改変することができます。
 * 
 * このプログラムは有用であることを願って頒布されますが、*全くの無保
 * 証* です。商業可能性の保証や特定の目的への適合性は、言外に示された
 * ものも含め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご
 * 覧ください。
 * 
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を
 * 一部受け取ったはずです。もし受け取っていなければ、フリーソフトウェ
 * ア財団まで請求してください(宛先は the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)。
 */

#include "StdAfx.h"
#include "utvideo.h"
#include "Predict.h"
#include "TunedFunc.h"

inline BYTE median(BYTE a, BYTE b, BYTE c)
{
	return max(min(max(a,b),c),min(a,b));
}

void PredictMedian(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride)
{
	if (IS_ALIGNED(pDst, 16) && IS_ALIGNED(pSrcBegin, 16) && IS_ALIGNED(dwStride, 16))
		tfn.pfnPredictMedian_align16(pDst, pSrcBegin, pSrcEnd, dwStride);
	else
		cpp_PredictMedian(pDst, pSrcBegin, pSrcEnd, dwStride);
}

void PredictMedianAndCount(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD *pCountTable)
{
	if (IS_ALIGNED(pDst, 16) && IS_ALIGNED(pSrcBegin, 16) && IS_ALIGNED(dwStride, 16))
		tfn.pfnPredictMedianAndCount_align16(pDst, pSrcBegin, pSrcEnd, dwStride, pCountTable);
	else
		tfn.pfnPredictMedianAndCount_align1(pDst, pSrcBegin, pSrcEnd, dwStride, pCountTable);
}

void cpp_PredictMedian(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride)
{
	const BYTE *p = pSrcBegin;
	BYTE *q = pDst;

	// 最初のラインの最初のピクセルは 0x80 を予測しておく。
	*q++ = *p++ - 0x80;

	// 最初のラインの残りのピクセルは predict left と同じ。
	for (; p < pSrcBegin + dwStride; p++, q++)
	{
		*q = *p - *(p - 1);
	}

	// 1 ラインしかない場合はここで終了。
	if (p == pSrcEnd)
		return;

	// 次のラインの最初のピクセルは predict above。
	// こうしておくとアセンブラ化した時に処理が若干簡単になる。
	*q = *p - *(p - dwStride);
	p++;
	q++;

	// 残りのピクセルが predict median の本番
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - median(*(p - dwStride), *(p - 1), *(p - dwStride) + *(p - 1) - *(p - 1 - dwStride));
	}
}

void cpp_PredictMedianAndCount(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD *pCountTable)
{
	const BYTE *p = pSrcBegin;
	BYTE *q = pDst;

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

void cpp_RestoreMedian(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride)
{
	const BYTE *p = pSrcBegin;
	BYTE *q = pDst;

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
