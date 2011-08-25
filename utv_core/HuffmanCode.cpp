/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

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
	_ASSERT(sizeof(CODE_LENGTH_SORT) == sizeof(uint16_t));
	sort((uint16_t *)p, (uint16_t *)p+256);
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
		{
			pDecodeTable->SymbolAndCodeLength[i].nCodeLength = 0;
			pDecodeTable->SymbolAndCodeLength[i].bySymbol    = cls[0].symbol;
		}
		for (int i = 0; i < _countof(pDecodeTable->LookupSymbolAndCodeLength); i++)
		{
			pDecodeTable->LookupSymbolAndCodeLength[i].nCodeLength = 0;
			pDecodeTable->LookupSymbolAndCodeLength[i].bySymbol    = cls[0].symbol;
		}
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


	// テーブル一発参照用

	for (int i = 0; i < _countof(pDecodeTable->LookupSymbolAndCodeLength); i++)
		pDecodeTable->LookupSymbolAndCodeLength[i].nCodeLength = 255;

	curcode = 0;
	for (int i = 255; i >= 0; i--)
	{
		if (cls[i].codelen == 255)
			continue;
		if (cls[i].codelen <= HUFFMAN_DECODE_TABLELOOKUP_BITS)
		{
			int idx = curcode >> (32 - HUFFMAN_DECODE_TABLELOOKUP_BITS);
			for (int j = 0; j < (1 << (HUFFMAN_DECODE_TABLELOOKUP_BITS - cls[i].codelen)); j++)
			{
				pDecodeTable->LookupSymbolAndCodeLength[idx+j].nCodeLength = cls[i].codelen;
				pDecodeTable->LookupSymbolAndCodeLength[idx+j].bySymbol = cls[i].symbol;
			}
		}
		curcode += 0x80000000 >> (cls[i].codelen - 1);
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

size_t cpp_HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
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

inline void DecodeSymbol(DWORD *&pSrc, int &nBits, const HUFFMAN_DECODE_TABLE *pDecodeTable, bool bAccum, BYTE &byPrevSymbol, BYTE *pDst, int nCorrPos, bool bDummyAlpha)
{
	DWORD code;
	BYTE symbol;
	int codelen;

	if (nBits == 0)
		code = (*pSrc) | 0x00000001;
	else
		code = ((*pSrc) << nBits) | ((*(pSrc+1)) >> (32 - nBits)) | 0x00000001;

	int tableidx = code >> (32 - HUFFMAN_DECODE_TABLELOOKUP_BITS);
	if (pDecodeTable->LookupSymbolAndCodeLength[tableidx].nCodeLength != 255)
	{
		symbol = pDecodeTable->LookupSymbolAndCodeLength[tableidx].bySymbol;
		codelen = pDecodeTable->LookupSymbolAndCodeLength[tableidx].nCodeLength;
	}
	else
	{
		int bsrval = bsr(code);
		int codeshift = pDecodeTable->nCodeShift[bsrval];
		code >>= codeshift;
		tableidx = pDecodeTable->dwSymbolBase[bsrval] + code;
		symbol = pDecodeTable->SymbolAndCodeLength[tableidx].bySymbol;
		codelen = pDecodeTable->SymbolAndCodeLength[tableidx].nCodeLength;
	}

	if (bAccum)
	{
		symbol += byPrevSymbol;
		byPrevSymbol = symbol;
	}

	if (nCorrPos != 0)
		symbol += *(pDst + nCorrPos) + 0x80;

	*pDst = symbol;
	if (bDummyAlpha)
		*(pDst+1) = 0xff;
	nBits += codelen;

	if (nBits >= 32)
	{
		nBits -= 32;
		pSrc++;
	}
}

static void cpp_HuffmanDecode_common(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, bool bAccum, int nStep, bool bBottomup, size_t dwNetWidth, size_t dwGrossWidth, int nCorrPos, bool bDummyAlpha)
{
	int nBits;
	DWORD *pSrc;
	BYTE *p;
	BYTE prevsym;
	BYTE *pStripeBegin;

	nBits = 0;
	pSrc = (DWORD *)pSrcBegin;
	prevsym = 0x80;

	if (!bBottomup)
	{
		for (pStripeBegin = pDstBegin; pStripeBegin < pDstEnd; pStripeBegin += dwGrossWidth)
		{
			BYTE *pStripeEnd = pStripeBegin + dwNetWidth;
			for (p = pStripeBegin; p < pStripeEnd; p += nStep)
				DecodeSymbol(pSrc, nBits, pDecodeTable, bAccum, prevsym, p, nCorrPos, bDummyAlpha);
		}
	}
	else
	{
		for (pStripeBegin = pDstEnd - dwGrossWidth; pStripeBegin >= pDstBegin; pStripeBegin -= dwGrossWidth)
		{
			BYTE *pStripeEnd = pStripeBegin + dwNetWidth;
			for (p = pStripeBegin; p < pStripeEnd; p += nStep)
				DecodeSymbol(pSrc, nBits, pDecodeTable, bAccum, prevsym, p, nCorrPos, bDummyAlpha);
		}
	}
}

void cpp_HuffmanDecode(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, false, 1, false, pDstEnd - pDstBegin, pDstEnd - pDstBegin, 0, false);
}

void cpp_HuffmanDecodeAndAccum(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 1, false, pDstEnd - pDstBegin, pDstEnd - pDstBegin, 0, false);
}

void cpp_HuffmanDecodeAndAccumStep2(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 2, false, pDstEnd - pDstBegin, pDstEnd - pDstBegin, 0, false);
}

void cpp_HuffmanDecodeAndAccumStep4(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, false, pDstEnd - pDstBegin, pDstEnd - pDstBegin, 0, false);
}

void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, true, dwNetWidth, dwGrossWidth, 0, false);
}

void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, true, dwNetWidth, dwGrossWidth, +1, false);
}

void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, true, dwNetWidth, dwGrossWidth, -1, false);
}

void cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, true, dwNetWidth, dwGrossWidth, -1, true);
}

void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, true, dwNetWidth, dwGrossWidth, 0, false);
}

void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, true, dwNetWidth, dwGrossWidth, +1, false);
}

void cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, true, dwNetWidth, dwGrossWidth, -1, false);
}
