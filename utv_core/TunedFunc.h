/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

struct HUFFMAN_ENCODE_TABLE;
struct HUFFMAN_DECODE_TABLE;

struct TUNEDFUNC
{
	void (*pfnPredictMedianAndCount_align16)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
	void (*pfnPredictMedianAndCount_align1)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
	void (*pfnPredictLeftAndCount_align1)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t *pCountTable);
	void (*pfnRestoreMedian_align1)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride);
	size_t (*pfnHuffmanEncode)(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
	void (*pfnHuffmanDecode)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccum)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccumStep2)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccumStep4)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXGreen)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForBottomupBGRGreen)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForBottomupBGRBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForBottomupBGRRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBGreen)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForTopdownRGBGreen)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForTopdownRGBBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnHuffmanDecodeAndAccumStep3ForTopdownRGBRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbNetWidth, size_t cbGrossWidth);
	void (*pfnConvertULY2ToBottomupBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertULY2ToBottomupBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertULY2ToTopdownBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertULY2ToTopdownBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertULY2ToTopdownRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertULY2ToTopdownXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertBottomupBGRToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertBottomupBGRXToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertTopdownBGRToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertTopdownBGRXToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertTopdownRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnConvertTopdownXRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
	void (*pfnDummyTunedFunc)(TUNEDFUNC *);
};

#define DummyTunedFunc ((void (*)(TUNEDFUNC *))NULL)

extern TUNEDFUNC tfn;
