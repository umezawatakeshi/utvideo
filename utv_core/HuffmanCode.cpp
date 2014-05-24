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

static void GenerateLengthLimitedHuffmanCodeLengthTable(uint8_t *pCodeLengthTable, int nSymbolBits)
{
	// とりあえずこれで逃げる。
	memset(pCodeLengthTable, nSymbolBits, 1 << nSymbolBits);
}

template<int B>
void GenerateHuffmanCodeLengthTable(HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable, const uint32_t *pCountTable)
{
	struct hufftree *huffsort[1 << B];
	struct hufftree huffleaf[1 << B];
	struct hufftree huffnode[1 << B];
	int nsym;

	nsym = 0;
	for (int i = 0; i < (1 << B); i++) {
		if (pCountTable[i] != 0) {
			huffleaf[nsym].left = NULL;
			huffleaf[nsym].right = NULL;
			huffleaf[nsym].count = pCountTable[i];
			huffleaf[nsym].symbol = i;
			huffsort[nsym] = &huffleaf[nsym];
			nsym++;
		}
		else
			pCodeLengthTable->codelen[i] = 255;
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

	if (generate_code_length(pCodeLengthTable->codelen, huffsort[0], 0))
		GenerateLengthLimitedHuffmanCodeLengthTable(pCodeLengthTable->codelen, B);
}

template void GenerateHuffmanCodeLengthTable<8>(HUFFMAN_CODELEN_TABLE<8> *pCodeLengthTable, const uint32_t *pCountTable);
template void GenerateHuffmanCodeLengthTable<10>(HUFFMAN_CODELEN_TABLE<10> *pCodeLengthTable, const uint32_t *pCountTable);

template<int B>
bool cls_less(const SYMBOL_AND_CODELEN<B> &a, const SYMBOL_AND_CODELEN<B> &b)
{
	if (a.codelen != b.codelen)
		return a.codelen < b.codelen;
	else
		return a.symbol < b.symbol;
}

template<int B>
void GenerateHuffmanEncodeTable(HUFFMAN_ENCODE_TABLE<B> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable)
{
	struct SYMBOL_AND_CODELEN<B> cls[1 << B];
	uintenc_t curcode;

	for (int i = 0; i < (1 << B); i++)
	{
		cls[i].symbol = i;
		cls[i].codelen = pCodeLengthTable->codelen[i];
	}

	sort(cls, cls + (1 << B), cls_less<B>);

	if (cls[0].codelen == 0)
	{
		memset(pEncodeTable, 0, sizeof(HUFFMAN_ENCODE_TABLE<B>));
		return;
	}

	memset(pEncodeTable, 0xff, sizeof(HUFFMAN_ENCODE_TABLE<B>));

	curcode = 0;
	for (int i = (1 << B) - 1; i >= 0; i--)
	{
		if (cls[i].codelen == 255)
			continue;
		pEncodeTable->dwTableMux[cls[i].symbol] = curcode | cls[i].codelen;
		curcode += UINTENC_MSB >> (cls[i].codelen - 1);
	}
}

template void GenerateHuffmanEncodeTable<8>(HUFFMAN_ENCODE_TABLE<8> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<8> *pCodeLengthTable);
template void GenerateHuffmanEncodeTable<10>(HUFFMAN_ENCODE_TABLE<10> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<10> *pCodeLengthTable);

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

template<int B>
void GenerateHuffmanDecodeTable(HUFFMAN_DECODE_TABLE<B> *pDecodeTable, const HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable)
{
	struct SYMBOL_AND_CODELEN<B> cls[1 << B];
	int nLastIndex;

	for (int i = 0; i < (1 << B); i++)
	{
		cls[i].symbol = i;
		cls[i].codelen = pCodeLengthTable->codelen[i];
	}

	sort(cls, cls + (1 << B), cls_less<B>);

	// 出現するシンボルが１種類しかない場合の処理
	if (cls[0].codelen == 0)
	{
		memset(pDecodeTable, 0, sizeof(HUFFMAN_DECODE_TABLE<B>));
		for (int i = 0; i < _countof(pDecodeTable->nCodeShift); i++)
			pDecodeTable->nCodeShift[i] = 31;
		for (int i = 0; i < _countof(pDecodeTable->SymbolAndCodeLength); i++)
		{
			pDecodeTable->SymbolAndCodeLength[i].codelen = 0;
			pDecodeTable->SymbolAndCodeLength[i].symbol  = cls[0].symbol;
		}
		for (int i = 0; i < _countof(pDecodeTable->LookupSymbolAndCodeLength); i++)
		{
			pDecodeTable->LookupSymbolAndCodeLength[i].codelen = 0;
			pDecodeTable->LookupSymbolAndCodeLength[i].symbol  = cls[0].symbol;
		}
		return;
	}

	// 最も長い符号長を持つシンボルの cls 上での位置を求める
	for (nLastIndex = (1 << B) - 1; nLastIndex >= 0; nLastIndex--)
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
			if (cls[i].codelen <= HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS)
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
				pDecodeTable->SymbolAndCodeLength[nextfillidx].symbol  = cls[i].symbol;
				pDecodeTable->SymbolAndCodeLength[nextfillidx].codelen = cls[i].codelen;
			}
			curcode += 0x80000000 >> (cls[i].codelen - 1);
			prevbsrval = bsrval;
		}
	}

	// 高速テーブルの生成（テーブル一発参照用）
	{
		uint32_t curcode = 0;

		for (int i = 0; i < _countof(pDecodeTable->LookupSymbolAndCodeLength); i++)
			pDecodeTable->LookupSymbolAndCodeLength[i].codelen = 255;

		for (int i = nLastIndex; i >= 0; i--)
		{
			if (cls[i].codelen <= HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS)
			{
				int idx = curcode >> (32 - HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS);
				for (int j = 0; j < (1 << (HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS - cls[i].codelen)); j++)
				{
					pDecodeTable->LookupSymbolAndCodeLength[idx+j].codelen = cls[i].codelen;
					pDecodeTable->LookupSymbolAndCodeLength[idx+j].symbol  = cls[i].symbol;
				}
			}
			curcode += 0x80000000 >> (cls[i].codelen - 1);
		}
	}
}

template void GenerateHuffmanDecodeTable<8>(HUFFMAN_DECODE_TABLE<8> *pDecodeTable, const HUFFMAN_CODELEN_TABLE<8> *pCodeLengthTable);
template void GenerateHuffmanDecodeTable<10>(HUFFMAN_DECODE_TABLE<10> *pDecodeTable, const HUFFMAN_CODELEN_TABLE<10> *pCodeLengthTable);

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

template<int B>
size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const typename CSymbolBits<B>::symbol_t *pSrcBegin, const typename CSymbolBits<B>::symbol_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<B> *pEncodeTable)
{
	int nBits;
	uintenc_t dwTmpEncoded;
	uint32_t *pDst;
	const typename CSymbolBits<B>::symbol_t *p;

	if (pEncodeTable->dwTableMux[0] == 0)
		return 0;

	nBits = 0;
	dwTmpEncoded = 0;
	pDst = (uint32_t *)pDstBegin;

	for (p = pSrcBegin; p < pSrcEnd; p++)
	{
		_ASSERT(*p <= CSymbolBits<B>::maxval);

		int nCurBits = (int)(pEncodeTable->dwTableMux[*p] & 0xff);
		_ASSERT(nCurBits > 0 && nCurBits != 0xff);
		uintenc_t dwCurEncoded = pEncodeTable->dwTableMux[*p] & UINTENC_MASK;

		dwTmpEncoded |= dwCurEncoded >> nBits;
		nBits += nCurBits;
		if (nBits >= UINTENC_BITS)
		{
			FlushEncoded(pDst, dwTmpEncoded, nBits);
			nBits -= UINTENC_BITS;
			dwTmpEncoded = dwCurEncoded << (nCurBits - nBits);
		}
	}

	FlushEncoded(pDst, dwTmpEncoded, nBits);

	return ((uint8_t *)pDst) - pDstBegin;
}

template size_t cpp_HuffmanEncode<8>(uint8_t *pDstBegin, const CSymbolBits<8>::symbol_t *pSrcBegin, const CSymbolBits<8>::symbol_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable);
template size_t cpp_HuffmanEncode<10>(uint8_t *pDstBegin, const CSymbolBits<10>::symbol_t *pSrcBegin, const CSymbolBits<10>::symbol_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<10> *pEncodeTable);

inline void DecodeSymbol(uint32_t *&pSrc, int &nBits, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, bool bAccum, uint8_t &byPrevSymbol, uint8_t *pDst, int nCorrPos, int nDummyAlphaPos)
{
	uint32_t code;
	uint8_t symbol;
	int codelen;

	if (nBits == 0)
		code = (*pSrc) | 0x00000001;
	else
		code = ((*pSrc) << nBits) | ((*(pSrc+1)) >> (32 - nBits)) | 0x00000001;

	int tableidx = code >> (32 - HUFFMAN_DECODE_TABLE<8>::LOOKUP_BITS);
	if (pDecodeTable->LookupSymbolAndCodeLength[tableidx].codelen != 255)
	{
		symbol = pDecodeTable->LookupSymbolAndCodeLength[tableidx].symbol;
		codelen = pDecodeTable->LookupSymbolAndCodeLength[tableidx].codelen;
	}
	else
	{
		int bsrval = bsr(code);
		int codeshift = pDecodeTable->nCodeShift[bsrval];
		code >>= codeshift;
		tableidx = pDecodeTable->dwSymbolBase[bsrval] + code;
		symbol = pDecodeTable->SymbolAndCodeLength[tableidx].symbol;
		codelen = pDecodeTable->SymbolAndCodeLength[tableidx].codelen;
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

static void cpp_HuffmanDecode_common(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, bool bAccum, int nStep, size_t cbWidth, ssize_t scbStride, int nCorrPos, int nDummyAlphaPos, uint8_t initial)
{
	int nBits;
	uint32_t *pSrc;
	uint8_t *p;
	uint8_t prevsym;
	uint8_t *pStripeBegin;

	nBits = 0;
	pSrc = (uint32_t *)pSrcBegin;
	prevsym = initial;

	for (pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += scbStride)
	{
		uint8_t *pStripeEnd = pStripeBegin + cbWidth;
		for (p = pStripeBegin; p < pStripeEnd; p += nStep)
			DecodeSymbol(pSrc, nBits, pDecodeTable, bAccum, prevsym, p, nCorrPos, nDummyAlphaPos);
	}
}

inline void DecodeSymbol10(uint32_t *&pSrc, int &nBits, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable, bool bAccum, uint16_t &byPrevSymbol, uint16_t *pDst, int nCorrPos, int nDummyAlphaPos)
{
	uint32_t code;
	uint16_t symbol;
	int codelen;

	if (nBits == 0)
		code = (*pSrc) | 0x00000001;
	else
		code = ((*pSrc) << nBits) | ((*(pSrc+1)) >> (32 - nBits)) | 0x00000001;

	int tableidx = code >> (32 - HUFFMAN_DECODE_TABLE<10>::LOOKUP_BITS);
	if (pDecodeTable->LookupSymbolAndCodeLength[tableidx].codelen != 255)
	{
		symbol = pDecodeTable->LookupSymbolAndCodeLength[tableidx].symbol;
		codelen = pDecodeTable->LookupSymbolAndCodeLength[tableidx].codelen;
	}
	else
	{
		int bsrval = bsr(code);
		int codeshift = pDecodeTable->nCodeShift[bsrval];
		code >>= codeshift;
		tableidx = pDecodeTable->dwSymbolBase[bsrval] + code;
		symbol = pDecodeTable->SymbolAndCodeLength[tableidx].symbol;
		codelen = pDecodeTable->SymbolAndCodeLength[tableidx].codelen;
	}

	_ASSERT(symbol < 0x400);

	if (bAccum)
	{
		symbol = (symbol + byPrevSymbol) & 0x3ff;
		byPrevSymbol = symbol;
	}

	if (nCorrPos != 0)
		symbol += (*(pDst + nCorrPos) + 0x200) & 0x3ff;

	*pDst = symbol;
	if (nDummyAlphaPos != 0)
		*(pDst + nDummyAlphaPos) = 0x3ff;
	nBits += codelen;

	if (nBits >= 32)
	{
		nBits -= 32;
		pSrc++;
	}
}

static void cpp_HuffmanDecode_common10(uint16_t *pDstBegin, uint16_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable, bool bAccum, int nStep, size_t cbWidth, ssize_t scbStride, int nCorrPos, int nDummyAlphaPos, uint16_t initial)
{
	int nBits;
	uint32_t *pSrc;
	uint16_t *p;
	uint16_t prevsym;
	uint16_t *pStripeBegin;

	nBits = 0;
	pSrc = (uint32_t *)pSrcBegin;
	prevsym = initial;

	for (pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; *(uint8_t **)&pStripeBegin += scbStride)
	{
		uint16_t *pStripeEnd = (uint16_t*)((uint8_t *)pStripeBegin + cbWidth);
		for (p = pStripeBegin; p < pStripeEnd; p += nStep)
			DecodeSymbol10(pSrc, nBits, pDecodeTable, bAccum, prevsym, p, nCorrPos, nDummyAlphaPos);
	}
}

void cpp_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, false, 1, cbWidth, scbStride, 0, 0, 0x80);
}

void cpp_HuffmanDecodeStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, false, 4, cbWidth, scbStride, 0, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccum(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 1, cbWidth, scbStride, 0, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep2(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 2, cbWidth, scbStride, 0, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep3(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, cbWidth, scbStride, 0, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep4(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, 0, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep3ForBGRBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, cbWidth, scbStride, +1, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep3ForBGRRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 3, cbWidth, scbStride, -1, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep4ForBGRXBlue(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, +1, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep4ForBGRXRed(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, -1, 0, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, -1, 1, 0x80);
}

void cpp_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride)
{
	cpp_HuffmanDecode_common(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 4, cbWidth, scbStride, +1, -1, 0x80);
}


void cpp_HuffmanDecodeAndAccum10(uint16_t *pDstBegin, uint16_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable, size_t cbWidth, ssize_t scbStride, uint16_t initial)
{
	cpp_HuffmanDecode_common10(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, true, 1, cbWidth, scbStride, 0, 0, initial);
}
