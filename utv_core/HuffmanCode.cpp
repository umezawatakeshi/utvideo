/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
//#include <windows.h>
//#include <algorithm>
//using namespace std;
#include "HuffmanCode.h"
#include "TunedFunc.h"

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
	memset(pCodeLengthTable, nSymbolBits, (size_t)1 << nSymbolBits);
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

	std::sort(huffsort, huffsort+nsym, hufftree_gt);
	for (int i = nsym - 2; i >= 0; i--) {
		huffnode[i].left = huffsort[i];
		huffnode[i].right = huffsort[i+1];
		huffnode[i].count = huffsort[i]->count + huffsort[i+1]->count;

		struct hufftree **insptr = std::upper_bound(huffsort, huffsort+i, &huffnode[i], hufftree_gt);
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

	std::sort(cls, cls + (1 << B), cls_less<B>);

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

#ifdef ENABLE_MULTI_SYMBOL_HUFFMAN_ENCODE
template<>
void GenerateHuffmanEncodeTable<8>(HUFFMAN_ENCODE_TABLE<8> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<8> *pCodeLengthTable)
{
	constexpr int B = 8;
	struct SYMBOL_AND_CODELEN<B> cls[1 << B];
	uintenc_t curcode;

	for (int i = 0; i < (1 << B); i++)
	{
		cls[i].symbol = i;
		cls[i].codelen = pCodeLengthTable->codelen[i];
	}

	std::sort(cls, cls + (1 << B), cls_less<B>);

	if (cls[0].codelen == 0)
	{
		memset(pEncodeTable, 0, sizeof(HUFFMAN_ENCODE_TABLE<B>));
		return;
	}

	memset(pEncodeTable, 0xff, sizeof(HUFFMAN_ENCODE_TABLE<B>));

	int ub;
	for (ub = (1 << B) - 1; ub >= 0; ub--)
	{
		if (cls[ub].codelen != 255)
			break;
	}

	curcode = 0;
	for (int i = ub; i >= 0; i--)
	{
		uintenc_t curcodej = 0;
		for (int j = ub; j >= 0; j--)
		{
			pEncodeTable->dwTableMuxUnrolled[cls[i].symbol + (cls[j].symbol << 8)] = curcode | (curcodej >> cls[i].codelen) | (cls[i].codelen + cls[j].codelen);
			curcodej += UINTENC_MSB >> (cls[j].codelen - 1);
		}
		pEncodeTable->dwTableMux[cls[i].symbol] = curcode | cls[i].codelen;
		curcode += UINTENC_MSB >> (cls[i].codelen - 1);
	}
}
#endif

template void GenerateHuffmanEncodeTable<8>(HUFFMAN_ENCODE_TABLE<8> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<8> *pCodeLengthTable);
template void GenerateHuffmanEncodeTable<10>(HUFFMAN_ENCODE_TABLE<10> *pEncodeTable, const HUFFMAN_CODELEN_TABLE<10> *pCodeLengthTable);

// LZCNT あるいは CLZ と呼ばれる命令
static inline int lzcnt32(uint32_t x)
{
	for (int i = 31; i >= 0; i--)
		if (x & (1 << i))
			return 31 - i;
	return 32;
}

template<int B, int syms>
int GenerateHuffmanDecodeTable0(HUFFMAN_DECODE_TABLE<B> *pDecodeTable, const SYMBOL_AND_CODELEN<B> *cls, const int *clsidx, const uint32_t *codes, uint32_t prefix, int preflen, int nFirstIndex, typename HUFFMAN_DECODE_TABLE<B>::combined_t cursym)
{
	// syms == 0 の場合、preflen は 0 以下である。
	// syms >  0 の場合、preflen は 0 より大きい。
	// このあたりを考慮してシフト式を書く必要がある

	if (syms == 0)
		_ASSERT(preflen <= 0);
	else
		_ASSERT(preflen > 0);

	using combined_t = typename HUFFMAN_DECODE_TABLE<B>::combined_t;

	int i = clsidx[HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS - preflen];
	if (syms == 0)
		_ASSERT(i >= 0);
	// syms == 0 の場合は絶対に i < 0 にはならない。（なるような値の LOOKUP_BITS にしたり preflen を渡したりはしない）

	int selffilllastidx;
	if (syms == 0)
		selffilllastidx = (prefix | (codes[i] << -preflen)) >> (32 - HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS);
	else if (syms >= HUFFMAN_DECODE_TABLE<B>::NSYM || i < 0)
		selffilllastidx = (prefix >> (32 - HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS)) + (1 << (HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS - preflen));
	else
		selffilllastidx = (prefix | (codes[i] >> preflen)) >> (32 - HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS);
	int nNextFirstIndex = i + 1;
	int curcodelen = preflen + (syms > 0 ? 0 : lzcnt32(codes[i + 1]));
	_ASSERT(codes[i] == 0 /* i.e. どの符号ビット列パターンでも次のシンボルが決まる */ || curcodelen >= 0);
	for (int j = prefix >> (32 - HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS); j < selffilllastidx; ++j)
	{
		pDecodeTable->cslen[j].codelen = curcodelen;
		pDecodeTable->cslen[j].symlen = syms;
		pDecodeTable->sym[j].combined = cursym;
	}

	if constexpr (syms < HUFFMAN_DECODE_TABLE<B>::NSYM)
	{
		for (; i >= nFirstIndex; i--)
		{
			uint32_t newprefix = prefix;
			if (syms == 0)
				newprefix |= (codes[i] << -preflen);
			else
				newprefix |= (codes[i] >> preflen);
			int newpreflen = preflen + cls[i].codelen;
			_ASSERT(newpreflen > 0);
			combined_t newsym = cursym | ((combined_t)cls[i].symbol << (sizeof(symbol_t<B>) * 8 * syms)); // XXX endian
			GenerateHuffmanDecodeTable0<B, syms + 1>(pDecodeTable, cls, clsidx, codes, newprefix, newpreflen, 0, newsym);
		}
	}
	
	return nNextFirstIndex;
}

template<int B, int shift>
void GenerateHuffmanDecodeTable(std::vector<HUFFMAN_DECODE_TABLE<B>>& vecDecodeTable, const HUFFMAN_CODELEN_TABLE<B> *pCodeLengthTable)
{
	static_assert(sizeof(symbol_t<B>) * 8 >= B + shift, "");

	struct SYMBOL_AND_CODELEN<B> cls[1 << B];
	int nLastIndex;

	uint32_t codes[1 << B];
	uint32_t curcode;
	int clsidx[64]; // 実際にいくつあればいいのか真面目に考察していない
	int previdx;

	for (int i = 0; i < (1 << B); i++)
	{
		cls[i].symbol = i << shift;
		cls[i].codelen = pCodeLengthTable->codelen[i];
	}

	std::sort(cls, cls + (1 << B), cls_less<B>);

	// 出現するシンボルが１種類しかない場合の処理
	if (cls[0].codelen == 0)
	{
		if (vecDecodeTable.empty())
			vecDecodeTable.resize(1);
		vecDecodeTable[0].cslen[0].codelen = 0;
		vecDecodeTable[0].sym[0].symbols[0] = cls[0].symbol;
		return;
	}

	// 最も長い符号長を持つシンボルの cls 上での位置を求める
	for (nLastIndex = (1 << B) - 1; nLastIndex >= 0; nLastIndex--)
	{
		if (cls[nLastIndex].codelen != 255)
			break;
	}

	// cls[0] が、最も符号語長の短いシンボル
	// cls[nLastIndex-1] が、最も符号語長の長いシンボル

	// 各シンボルに対応する符号語を計算する
	curcode = 0;
	for (int i = nLastIndex; i >= 0; i--)
	{
		codes[i] = curcode;
		curcode += 0x80000000 >> (cls[i].codelen - 1);
	}
	// codes[i] が cls[i] に対応する符号語

	// 各符号語長に対し、それ以下の符号語長を持つ最もインデックスが大きい（≒符号語長の長い）符号語を求める
	// それ以下の符号語長を持つ符号語が存在しない場合は -1 とする。
	previdx = nLastIndex;
	for (int i = _countof(clsidx) - 1; i >= 0; i--)
	{
		int j;
		for (j = previdx; j >= 0; j--)
		{
			if (cls[j].codelen <= i)
				break;
		}
		clsidx[i] = j;
		previdx = j;
	}

	int nFirstIndex = 0;
	for (unsigned i = 0; nFirstIndex < nLastIndex; ++i)
	{
		int skipbits = lzcnt32(codes[nFirstIndex]);
		if (vecDecodeTable.size() < i + 1)
			vecDecodeTable.resize(i + 1);
		nFirstIndex = GenerateHuffmanDecodeTable0<B, 0>(&vecDecodeTable[i], cls, clsidx, codes, 0, -skipbits, nFirstIndex, 0);
	}
}

template void GenerateHuffmanDecodeTable<8>(std::vector<HUFFMAN_DECODE_TABLE<8>>& vecDecodeTable, const HUFFMAN_CODELEN_TABLE<8> *pCodeLengthTable);
template void GenerateHuffmanDecodeTable<10>(std::vector<HUFFMAN_DECODE_TABLE<10>>& vecDecodeTable, const HUFFMAN_CODELEN_TABLE<10> *pCodeLengthTable);
template void GenerateHuffmanDecodeTable<10, 6>(std::vector<HUFFMAN_DECODE_TABLE<10>>& vecDecodeTable, const HUFFMAN_CODELEN_TABLE<10>* pCodeLengthTable);

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
size_t cpp_HuffmanEncode(uint8_t *pDstBegin, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, const HUFFMAN_ENCODE_TABLE<B> *pEncodeTable)
{
	int nBits;
	uintenc_t dwTmpEncoded;
	uint32_t *pDst;
	const symbol_t<B> *p;

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

template size_t cpp_HuffmanEncode<8>(uint8_t *pDstBegin, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable);
template size_t cpp_HuffmanEncode<10>(uint8_t *pDstBegin, const symbol_t<10> *pSrcBegin, const symbol_t<10> *pSrcEnd, const HUFFMAN_ENCODE_TABLE<10> *pEncodeTable);


template<int B>
symbol_t<B> * cpp_HuffmanDecode(symbol_t<B> *pDstBegin, symbol_t<B> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<B> *pDecodeTableHead)
{
	int nBits;
	uint32_t *pSrc;
	symbol_t<B> *pDst;

	nBits = 0;
	pSrc = (uint32_t *)pSrcBegin;
	int tablenum = 0;

	for (pDst = (symbol_t<B> *)pDstBegin; pDst < (symbol_t<B> *)pDstEnd;)
	{
		uint32_t code;
		typename HUFFMAN_DECODE_TABLE<B>::combined_t combined;
		int codelen;
		int symlen;

		if (nBits == 0)
			code = (*pSrc);
		else
			code = ((*pSrc) << nBits) | ((*(pSrc + 1)) >> (32 - nBits));

		const HUFFMAN_DECODE_TABLE<B>* pDecodeTable = pDecodeTableHead + tablenum;
		int tableidx = code >> (32 - HUFFMAN_DECODE_TABLE<B>::LOOKUP_BITS);
		combined = pDecodeTable->sym[tableidx].combined;
		codelen = pDecodeTable->cslen[tableidx].codelen;
		symlen = pDecodeTable->cslen[tableidx].symlen;
		if (symlen == 0)
			++tablenum;
		else
			tablenum = 0;

		for (int i = 0; i < symlen && pDst < (symbol_t<B> *)pDstEnd; i++)
		{
			*pDst++ = (symbol_t<B>)combined;
			combined >>= sizeof(symbol_t<B>) * 8;
		}
		nBits += codelen;

		if (nBits >= 32)
		{
			nBits -= 32;
			pSrc++;
		}
	}

	return pDst;
}

template symbol_t<8> * cpp_HuffmanDecode<8>(symbol_t<8> *pDstBegin, symbol_t<8> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable);
template symbol_t<10> * cpp_HuffmanDecode<10>(symbol_t<10> *pDstBegin, symbol_t<10> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable);


template<int B>
struct tfnHuffmanDecode
{
	static symbol_t<B> *f(symbol_t<B> *pDstBegin, symbol_t<B> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<B> *pDecodeTable);
};

template<>
struct tfnHuffmanDecode<8>
{
	static symbol_t<8> *f(symbol_t<8> *pDstBegin, symbol_t<8> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable)
	{
		return HuffmanDecode8(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable);
	}
};

template<>
struct tfnHuffmanDecode<10>
{
	static symbol_t<10> *f(symbol_t<10> *pDstBegin, symbol_t<10> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable)
	{
		return HuffmanDecode10(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable);
	}
};

template<int B>
symbol_t<B> *HuffmanDecode(symbol_t<B> *pDstBegin, symbol_t<B> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<B> *pDecodeTable)
{
	if (pDecodeTable->cslen[0].codelen == 0)
	{
		std::fill(pDstBegin, pDstEnd, pDecodeTable->sym[0].symbols[0]);
		return pDstEnd;
	}
	else
		return tfnHuffmanDecode<B>::f(pDstBegin, pDstEnd, pSrcBegin, pDecodeTable);
}

template symbol_t<8> *HuffmanDecode<8>(symbol_t<8> *pDstBegin, symbol_t<8> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable);
template symbol_t<10> *HuffmanDecode<10>(symbol_t<10> *pDstBegin, symbol_t<10> *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable);
