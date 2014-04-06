/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
//#include <windows.h>
//#include <algorithm>
//using namespace std;
#include "HuffmanCode.h"

struct hufftree {
	struct hufftree *left;
	struct hufftree *right;
	uint32_t count;
	uint32_t symbol;	// uint8_t ではないシンボルが入ることがある
};

inline bool hufftree_gt(const struct hufftree *a, const struct hufftree *b)
{
	return (a->count > b->count);
}

bool generate_code_length(uint8_t *codelen, const struct hufftree *node, uint8_t curlen)
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

static void GenerateLengthLimitedHuffmanCodeLengthTable(uint8_t *pCodeLengthTable)
{
	// とりあえずこれで逃げる。
	memset(pCodeLengthTable, 8, 256);
}

void GenerateHuffmanCodeLengthTable(uint8_t *pCodeLengthTable, const uint32_t *pCountTable)
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
	uint32_t symbol;
	uint8_t codelen;
};

bool cls_less(const CODE_LENGTH_SORT &a, const CODE_LENGTH_SORT &b)
{
	if (a.codelen != b.codelen)
		return a.codelen < b.codelen;
	else
		return a.symbol < b.symbol;
}

void GenerateHuffmanEncodeTable(HUFFMAN_ENCODE_TABLE *pEncodeTable, const uint8_t *pCodeLengthTable)
{
	struct CODE_LENGTH_SORT cls[256];
	uintenc_t curcode;

	for (int i = 0; i < 256; i++)
	{
		cls[i].symbol = i;
		cls[i].codelen = pCodeLengthTable[i];
	}

	sort(cls, cls + 256, cls_less);

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
		curcode += UINTENC_MSB >> (cls[i].codelen - 1);
	}
}

// IA-32 の BSR 命令
// 本物の BSR 命令では入力が 0 の場合に出力が不定になる。
inline int bsr(uint32_t x)
{
	_ASSERT(x != 0);

	for (int i = 31; i >= 0; i--)
		if (x & (1 << i))
			return i;
	return rand() % 32;
}

// LZCNT あるいは CLZ と呼ばれる命令
inline int lzcnt(uint32_t x)
{
	for (int i = 31; i >= 0; i--)
		if (x & (1 << i))
			return 31 - i;
	return 32;
}

void GenerateHuffmanDecodeTable(HUFFMAN_DECODE_TABLE *pDecodeTable, const uint8_t *pCodeLengthTable)
{
	struct CODE_LENGTH_SORT cls[256];
	int nLastIndex;

	for (int i = 0; i < 256; i++)
	{
		cls[i].symbol = i;
		cls[i].codelen = pCodeLengthTable[i];
	}

	sort(cls, cls + 256, cls_less);

	// 出現するシンボルが１種類しかない場合の処理
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

	// 低速テーブルの生成
	{
		uint32_t curcode = 1; // bsr 対策で 0 ではなく 1。符号語長は 24 以下なので 1 にしてあっても問題ない。
		int j = 0;
		int base = 0;
		int nextfillidx = 0;
		int prevbsrval = 0;

		for (int i = nLastIndex; i >= 0; i--)
		{
			// 短い符号語の場合は高速テーブルでデコードされるので、低速テーブルの生成は不要である。
			if (cls[i].codelen <= HUFFMAN_DECODE_TABLELOOKUP_BITS)
				break;

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
			int lastfillidx = nextfillidx + (1 << (32 - pDecodeTable->nCodeShift[bsrval] - cls[i].codelen));
			for (; nextfillidx < lastfillidx; nextfillidx++)
			{
				pDecodeTable->SymbolAndCodeLength[nextfillidx].bySymbol    = cls[i].symbol;
				pDecodeTable->SymbolAndCodeLength[nextfillidx].nCodeLength = cls[i].codelen;
			}
			curcode += 0x80000000 >> (cls[i].codelen - 1);
			prevbsrval = bsrval;
		}
	}

	// 高速テーブルの生成（テーブル一発参照用）
	{
		uint32_t curcode = 0;

		for (int i = 0; i < _countof(pDecodeTable->LookupSymbolAndCodeLength); i++)
			pDecodeTable->LookupSymbolAndCodeLength[i].nCodeLength = 255;

		for (int i = nLastIndex; i >= 0; i--)
		{
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
}

inline void FlushEncoded(uint32_t *&pDst, uintenc_t &dwTmpEncoded, int &nBits)
{
#if defined(__x86_64__)
	if (nBits > 0)
		*pDst++ = (uint32_t)(dwTmpEncoded >> 32);
	if (nBits > 32)
		*pDst++ = (uint32_t)(dwTmpEncoded & 0xffffffff);
#else
	if (nBits > 0)
		*pDst++ = dwTmpEncoded;
#endif
}

inline void EncodeSymbol(uint8_t bySymbol, const HUFFMAN_ENCODE_TABLE *pEncodeTable, uint32_t *&pDst, uintenc_t &dwTmpEncoded, int &nBits)
{
	int nCurBits = (int)(pEncodeTable->dwTableMux[bySymbol] & 0xff);
	uintenc_t dwCurEncoded = pEncodeTable->dwTableMux[bySymbol] & UINTENC_MASK;

	dwTmpEncoded |= dwCurEncoded >> nBits;
	nBits += nCurBits;
	if (nBits >= UINTENC_BITS)
	{
		FlushEncoded(pDst, dwTmpEncoded, nBits);
		nBits -= UINTENC_BITS;
		dwTmpEncoded = dwCurEncoded << (nCurBits - nBits);
	}
}

size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
{
	int nBits;
	uintenc_t dwTmpEncoded;
	uint32_t *pDst;
	const uint8_t *p;

	if (pEncodeTable->dwTableMux[0] == 0)
		return 0;

	nBits = 0;
	dwTmpEncoded = 0;
	pDst = (uint32_t *)pDstBegin;

	for (p = pSrcBegin; p < pSrcEnd; p++)
		EncodeSymbol(*p, pEncodeTable, pDst, dwTmpEncoded, nBits);

	FlushEncoded(pDst, dwTmpEncoded, nBits);

	return ((uint8_t *)pDst) - pDstBegin;
}

inline void DecodeSymbol(uint32_t *&pSrc, int &nBits, const HUFFMAN_DECODE_TABLE *pDecodeTable, bool bAccum, uint8_t &byPrevSymbol, uint8_t *pDst, int nCorrPos, int nDummyAlphaPos)
{
	uint32_t code;
	uint8_t symbol;
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
	if (nDummyAlphaPos != 0)
		*(pDst + nDummyAlphaPos) = 0xff;
	nBits += codelen;

	if (nBits >= 32)
	{
		nBits -= 32;
		pSrc++;
	}
}

static void cpp_HuffmanDecode_common(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, bool bAccum, int nStep, size_t cbWidth, ssize_t scbStride, int nCorrPos, int nDummyAlphaPos)
{
	int nBits;
	uint32_t *pSrc;
	uint8_t *p;
	uint8_t prevsym;
	uint8_t *pStripeBegin;

	nBits = 0;
	pSrc = (uint32_t *)pSrcBegin;
	prevsym = 0x80;

	for (pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += scbStride)
	{
		uint8_t *pStripeEnd = pStripeBegin + cbWidth;
		for (p = pStripeBegin; p < pStripeEnd; p += nStep)
			DecodeSymbol(pSrc, nBits, pDecodeTable, bAccum, prevsym, p, nCorrPos, nDummyAlphaPos);
	}
}

void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, false, 1, cbWidth, scbStride, 0, 0);
}

void cpp_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, false, 4, cbWidth, scbStride, 0, 0);
}

void cpp_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 1, cbWidth, scbStride, 0, 0);
}

void cpp_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 2, cbWidth, scbStride, 0, 0);
}

void cpp_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, cbWidth, scbStride, 0, 0);
}

void cpp_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, 0, 0);
}

void cpp_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, cbWidth, scbStride, +1, 0);
}

void cpp_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, cbWidth, scbStride, -1, 0);
}

void cpp_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, +1, 0);
}

void cpp_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, -1, 0);
}

void cpp_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, -1, 1);
}

void cpp_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, +1, -1);
}
