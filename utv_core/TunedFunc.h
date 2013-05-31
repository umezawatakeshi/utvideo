/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

struct HUFFMAN_ENCODE_TABLE;
struct HUFFMAN_DECODE_TABLE;

struct TUNEDFUNC_YUV
{
	void (*pfnConvertULY2ToBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRXToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertXRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
};

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
	TUNEDFUNC_YUV bt601;
	TUNEDFUNC_YUV bt709;
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


#define PredictWrongMedianAndCount_align16 tfn.pfnPredictWrongMedianAndCount_align16
#define PredictWrongMedianAndCount_align1 tfn.pfnPredictWrongMedianAndCount_align1
#define PredictLeftAndCount tfn.pfnPredictLeftAndCount_align1
#define RestoreWrongMedian tfn.pfnRestoreWrongMedian_align1

#define HuffmanEncode tfn.pfnHuffmanEncode
#define HuffmanDecode tfn.pfnHuffmanDecode
#define HuffmanDecodeAndAccum tfn.pfnHuffmanDecodeAndAccum
#define HuffmanDecodeAndAccumStep2 tfn.pfnHuffmanDecodeAndAccumStep2
#define HuffmanDecodeAndAccumStep3 tfn.pfnHuffmanDecodeAndAccumStep3
#define HuffmanDecodeAndAccumStep4 tfn.pfnHuffmanDecodeAndAccumStep4
#define HuffmanDecodeAndAccumStep3ForBGRBlue tfn.pfnHuffmanDecodeAndAccumStep3ForBGRBlue
#define HuffmanDecodeAndAccumStep3ForBGRRed tfn.pfnHuffmanDecodeAndAccumStep3ForBGRRed
#define HuffmanDecodeAndAccumStep4ForBGRXBlue tfn.pfnHuffmanDecodeAndAccumStep4ForBGRXBlue
#define HuffmanDecodeAndAccumStep4ForBGRXRed tfn.pfnHuffmanDecodeAndAccumStep4ForBGRXRed
#define HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha
#define HuffmanDecodeAndAccumStep3ForRGBBlue HuffmanDecodeAndAccumStep3ForBGRRed
#define HuffmanDecodeAndAccumStep3ForRGBRed HuffmanDecodeAndAccumStep3ForBGRBlue
#define HuffmanDecodeAndAccumStep4ForXRGBBlue HuffmanDecodeAndAccumStep4ForBGRXRed
#define HuffmanDecodeAndAccumStep4ForXRGBRed HuffmanDecodeAndAccumStep4ForBGRXBlue
#define HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha tfn.pfnHuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha

#define ConvertULY2ToBGR tfn.bt601.pfnConvertULY2ToBGR
#define ConvertULY2ToBGRX tfn.bt601.pfnConvertULY2ToBGRX
#define ConvertULY2ToRGB tfn.bt601.pfnConvertULY2ToRGB
#define ConvertULY2ToXRGB tfn.bt601.pfnConvertULY2ToXRGB
#define ConvertBGRToULY2 tfn.bt601.pfnConvertBGRToULY2
#define ConvertBGRXToULY2 tfn.bt601.pfnConvertBGRXToULY2
#define ConvertRGBToULY2 tfn.bt601.pfnConvertRGBToULY2
#define ConvertXRGBToULY2 tfn.bt601.pfnConvertXRGBToULY2

#define ConvertBGRToULRG tfn.pfnConvertBGRToULRG
#define ConvertRGBToULRG(g, b, r, p, q, w, s) ConvertBGRToULRG(g, r, b, p, q, w, s)
#define ConvertBGRXToULRG tfn.pfnConvertBGRXToULRG
#define ConvertXRGBToULRG tfn.pfnConvertXRGBToULRG
#define ConvertBGRAToULRA tfn.pfnConvertBGRAToULRA
#define ConvertARGBToULRA tfn.pfnConvertARGBToULRA
#define ConvertYUYVToULY2 tfn.pfnConvertYUYVToULY2
#define ConvertUYVYToULY2 tfn.pfnConvertUYVYToULY2
#define ConvertULRGToBGR tfn.pfnConvertULRGToBGR
#define ConvertULRGToRGB(p, q, g, b, r, w, s) ConvertULRGToBGR(p, q, g, r, b, w, s)
#define ConvertULRGToBGRX tfn.pfnConvertULRGToBGRX
#define ConvertULRGToXRGB tfn.pfnConvertULRGToXRGB
#define ConvertULRAToBGRA tfn.pfnConvertULRAToBGRA
#define ConvertULRAToARGB tfn.pfnConvertULRAToARGB
#define ConvertULY2ToYUYV tfn.pfnConvertULY2ToYUYV
#define ConvertULY2ToUYVY tfn.pfnConvertULY2ToUYVY
