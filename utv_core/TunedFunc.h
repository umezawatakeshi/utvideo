/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

struct HUFFMAN_ENCODE_TABLE;
struct HUFFMAN_DECODE_TABLE;

struct TUNEDFUNC
{
	void (*pfnPredictWrongMedianAndCount_align16)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
	void (*pfnPredictWrongMedianAndCount_align1)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
	void (*pfnPredictLeftAndCount_align1)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t *pCountTable);
	void (*pfnRestoreWrongMedian_align1)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride);
	size_t (*pfnHuffmanEncode)(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable);
	void (*pfnHuffmanDecode)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccum)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep2)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep3)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep3ForBGRBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep3ForBGRRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForBGRXBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForBGRXRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRXToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertXRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRToULRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRXToULRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertXRGBToULRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRAToULRA)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertARGBToULRA)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertYUYVToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
	void (*pfnConvertUYVYToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
	void (*pfnConvertULRGToBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToBGRA)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToARGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToYUYV)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin);
	void (*pfnConvertULY2ToUYVY)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin);
	void (*pfnDummyTunedFunc)(TUNEDFUNC *);
};

#define DummyTunedFunc ((void (*)(TUNEDFUNC *))NULL)

extern TUNEDFUNC tfn;
