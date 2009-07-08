/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008-2009  UMEZAWA Takeshi
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
 * Copyright (C) 2008-2009  梅澤 威志
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
//#include <windows.h>
//#include <algorithm>
//using namespace std;
#include "HuffmanCode.h"

struct hufftree {
	struct hufftree *left;
	struct hufftree *right;
	DWORD count;
	DWORD symbol;	// BYTE ではないシンボルが入ることがある
};

inline bool hufftree_gt(const struct hufftree *a, const struct hufftree *b)
{
	return (a->count > b->count);
}

bool generate_code_length(BYTE *codelen, const struct hufftree *node, BYTE curlen)
{
	if (node->left == NULL) {
		codelen[node->symbol] = curlen;
		return (curlen > 24);
	} else {
		return
			generate_code_length(codelen, node->left, curlen+1) ||
			generate_code_length(codelen, node->right, curlen+1);
	}
}

static void GenerateLengthLimitedHuffmanCodeLengthTable(BYTE *pCodeLengthTable)
{
	// とりあえずこれで逃げる。
	memset(pCodeLengthTable, 8, 256);
}

void GenerateHuffmanCodeLengthTable(BYTE *pCodeLengthTable, const DWORD *pCountTable)
{
	struct hufftree *huffsort[256];
	struct hufftree huffleaf[256];
	struct hufftree huffnode[256];
	int nsym;

	nsym = 0;
	for (int i = 0; i < 256; i++) {
		if (pCountTable[i] != 0) {
			huffleaf[nsym].left = NULL;
			huffleaf[nsym].right = NULL;
			huffleaf[nsym].count = pCountTable[i];
			huffleaf[nsym].symbol = i;
			huffsort[nsym] = &huffleaf[nsym];
			nsym++;
		}
		else
			pCodeLengthTable[i] = 255;
	}

	sort(huffsort, huffsort+nsym, hufftree_gt);
	for (int i = nsym - 2; i >= 0; i--) {
		huffnode[i].left = huffsort[i];
		huffnode[i].right = huffsort[i+1];
		huffnode[i].count = huffsort[i]->count + huffsort[i+1]->count;

		struct hufftree **insptr = upper_bound(huffsort, huffsort+i, &huffnode[i], hufftree_gt);
		struct hufftree **movptr;
		for (movptr = huffsort+i-1; movptr >= insptr; movptr--)
			*(movptr+1) = *movptr;
		*insptr = &huffnode[i];
	}

	if (generate_code_length(pCodeLengthTable, huffsort[0], 0))
		GenerateLengthLimitedHuffmanCodeLengthTable(pCodeLengthTable);
}

struct CODE_LENGTH_SORT
{
	BYTE symbol;
	BYTE codelen;
};

inline void sort_codelength(struct CODE_LENGTH_SORT *p)
{
	_ASSERT(sizeof(CODE_LENGTH_SORT) == sizeof(WORD));
	sort((WORD *)p, (WORD *)p+256);
}

void GenerateHuffmanEncodeTable(HUFFMAN_ENCODE_TABLE *pEncodeTable, const BYTE *pCodeLengthTable)
{
	struct CODE_LENGTH_SORT cls[256];
	DWORD curcode;

	for (int i = 0; i < 256; i++)
	{
		cls[i].symbol = i;
		cls[i].codelen = pCodeLengthTable[i];
	}

	sort_codelength(cls);

	if (cls[0].codelen == 0)
	{
		memset(pEncodeTable, 0, sizeof(HUFFMAN_ENCODE_TABLE));
		return;
	}

	memset(pEncodeTable, 0xff, sizeof(HUFFMAN_ENCODE_TABLE));

	curcode = 0;
	for (int i = 255; i >= 0; i--)
	{
		if (cls[i].codelen == 255)
			continue;
		pEncodeTable->dwTableMux[cls[i].symbol] = curcode | cls[i].codelen;
		curcode += 0x80000000 >> (cls[i].codelen - 1);
	}
}

// IA-32 の BSR 命令
// 本物の BSR 命令では入力が 0 の場合に出力が不定になる。
inline int bsr(DWORD curcode)
{
	_ASSERT(curcode != 0);

	for (int i = 31; i >= 0; i--)
		if (curcode & (1 << i))
			return i;
	return rand() % 32;
}

void GenerateHuffmanDecodeTable(HUFFMAN_DECODE_TABLE *pDecodeTable, const BYTE *pCodeLengthTable)
{
	struct CODE_LENGTH_SORT cls[256];
	int nLastIndex;
	int j;
	int base;
	DWORD curcode;
	int nextfillidx;
	int lastfillidx;
	int prevbsrval;

	for (int i = 0; i < 256; i++)
	{
		cls[i].symbol = i;
		cls[i].codelen = pCodeLengthTable[i];
	}

	sort_codelength(cls);

	if (cls[0].codelen == 0)
	{
		memset(pDecodeTable, 0, sizeof(HUFFMAN_DECODE_TABLE));
		for (int i = 0; i < _countof(pDecodeTable->nCodeShift); i++)
			pDecodeTable->nCodeShift[i] = 31;
		for (int i = 0; i < _countof(pDecodeTable->SymbolAndCodeLength); i++)
			pDecodeTable->SymbolAndCodeLength[i].bySymbol = cls[0].symbol;
		return;
	}

	// 最も長い符号長を持つシンボルの cls 上での位置を求める
	for (nLastIndex = 255; nLastIndex >= 0; nLastIndex--)
	{
		if (cls[nLastIndex].codelen != 255)
			break;
	}

	curcode = 1;
	j = 0;
	base = 0;
	nextfillidx = 0;
	prevbsrval = 0;
	for (int i = nLastIndex; i >= 0; i--)
	{
		int bsrval = bsr(curcode);
		if (bsrval != prevbsrval)
		{
			base = nextfillidx - (curcode >> (32 - cls[i].codelen));
		}
		for (; j <= bsrval; j++)
		{
			pDecodeTable->nCodeShift[j] = 32 - cls[i].codelen;
			pDecodeTable->dwSymbolBase[j] = base;
		}
		lastfillidx = nextfillidx + (1 << (32 - pDecodeTable->nCodeShift[bsrval] - cls[i].codelen));
		for (; nextfillidx < lastfillidx; nextfillidx++)
		{
			pDecodeTable->SymbolAndCodeLength[nextfillidx].bySymbol    = cls[i].symbol;
			pDecodeTable->SymbolAndCodeLength[nextfillidx].nCodeLength = cls[i].codelen;
		}
		curcode += 0x80000000 >> (cls[i].codelen - 1);
		prevbsrval = bsrval;
	}
}

inline void EncodeSymbol(BYTE bySymbol, const HUFFMAN_ENCODE_TABLE *pEncodeTable, DWORD *&pDst, DWORD &dwTmpEncoded, int &nBits)
{
	int nCurBits = pEncodeTable->dwTableMux[bySymbol] & 0xff;
	DWORD dwCurEncoded = pEncodeTable->dwTableMux[bySymbol] & 0xffffff00;

	dwTmpEncoded |= dwCurEncoded >> nBits;
	nBits += nCurBits;
	if (nBits >= 32)
	{
		*pDst++ = dwTmpEncoded;
		nBits -= 32;
		dwTmpEncoded = dwCurEncoded << (nCurBits - nBits);
	}
}

DWORD cpp_HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
{
	int nBits;
	DWORD dwTmpEncoded;
	DWORD *pDst;
	const BYTE *p;

	if (pEncodeTable->dwTableMux[0] == 0)
		return 0;

	nBits = 0;
	dwTmpEncoded = 0;
	pDst = (DWORD *)pDstBegin;

	for (p = pSrcBegin; p < pSrcEnd; p++)
		EncodeSymbol(*p, pEncodeTable, pDst, dwTmpEncoded, nBits);

	if (nBits != 0)
		*pDst++ = dwTmpEncoded;

	return ((BYTE *)pDst) - pDstBegin;
}

inline void DecodeSymbol(DWORD *&pSrc, int &nBits, const HUFFMAN_DECODE_TABLE *pDecodeTable, bool bAccum, BYTE &byPrevSymbol, BYTE *pDst)
{
	DWORD code;

	if (nBits == 0)
		code = (*pSrc) | 0x00000001;
	else
		code = ((*pSrc) << nBits) | ((*(pSrc+1)) >> (32 - nBits)) | 0x00000001;
	int bsrval = bsr(code);
	int codeshift = pDecodeTable->nCodeShift[bsrval];
	code >>= codeshift;
	BYTE symbol = pDecodeTable->SymbolAndCodeLength[pDecodeTable->dwSymbolBase[bsrval] + code].bySymbol;
	if (bAccum)
	{
		byPrevSymbol += symbol;
		*pDst = byPrevSymbol;
	}
	else
		*pDst = symbol;

	int codelen = pDecodeTable->SymbolAndCodeLength[pDecodeTable->dwSymbolBase[bsrval] + code].nCodeLength;
	nBits += codelen;
	if (nBits >= 32)
	{
		nBits -= 32;
		pSrc++;
	}
}

static void cpp_HuffmanDecode_common(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, bool bAccum)
{
	int nBits;
	DWORD *pSrc;
	BYTE *p;
	BYTE prevsym;

	if (pDecodeTable->SymbolAndCodeLength[0].nCodeLength == 0)
	{
		if (bAccum)
			memset(pDstBegin, pDecodeTable->SymbolAndCodeLength[0].bySymbol + 0x80, pDstEnd-pDstBegin);
		else
			memset(pDstBegin, pDecodeTable->SymbolAndCodeLength[0].bySymbol, pDstEnd-pDstBegin);
		return;
	}

	nBits = 0;
	pSrc = (DWORD *)pSrcBegin;
	prevsym = 0x80;

	for (p = pDstBegin; p < pDstEnd; p++)
		DecodeSymbol(pSrc, nBits, pDecodeTable, bAccum, prevsym, p);
}

void cpp_HuffmanDecode(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, false);
}

void cpp_HuffmanDecodeAndAccum(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true);
}
