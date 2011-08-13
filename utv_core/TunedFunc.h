/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

struct HUFFMAN_ENCODE_TABLE;
struct HUFFMAN_DECODE_TABLE;

struct TUNEDFUNC
{
	void (*pfnPredictMedianAndCount_align16)(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
	void (*pfnPredictMedianAndCount_align1)(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
	void (*pfnPredictLeftAndCount_align1)(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD *pCountTable);
	void (*pfnRestoreMedian_align1)(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
	size_t (*pfnHuffmanEncode)(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
	void (*pfnHuffmanDecode)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccum)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccumStep2)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccumStep4)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32Green)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32Blue)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32Red)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForBottomupRGB24Green)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForBottomupRGB24Blue)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForBottomupRGB24Red)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t dwNetWidth, size_t dwGrossWidth);
	void (*pfnConvertULY2ToBottomupRGB24)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
	void (*pfnConvertULY2ToBottomupRGB32)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
	void (*pfnConvertBottomupRGB24ToULY2)(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);
	void (*pfnConvertBottomupRGB32ToULY2)(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);
};

extern TUNEDFUNC tfn;

void InitializeTunedFunc(void);
