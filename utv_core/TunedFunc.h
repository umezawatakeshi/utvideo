/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "Coefficient.h"

#define FEATURESIZE 2
#define DECLARE_TUNEDFUNC_FRAGMENT_HEADER(T) \
	const T *pNext; \
	uint32_t dwRequiredFeatures[FEATURESIZE]


struct TUNEDFUNC_FRAGMENT
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_FRAGMENT);
};


template<int B> struct HUFFMAN_ENCODE_TABLE;
template<int B> struct HUFFMAN_DECODE_TABLE;

struct TUNEDFUNC_PREDICT
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_PREDICT);
	void (*pfnPredictWrongMedianAndCount)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
	void (*pfnPredictLeftAndCount)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t *pCountTable);
	void (*pfnRestoreWrongMedian)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride);
	void (*pfnRestoreWrongMedianBlock4)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
};

struct TUNEDFUNC_HUFFMAN_ENCODE
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_HUFFMAN_ENCODE);
	size_t (*pfnHuffmanEncode)(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable);
};

struct TUNEDFUNC_HUFFMAN_DECODE
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_HUFFMAN_DECODE);
	void (*pfnHuffmanDecode)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeStep4)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccum)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep2)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep3)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep3ForBGRBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep3ForBGRRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForBGRXBlue)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForBGRXRed)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
	void (*pfnHuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable, size_t cbWidth, ssize_t scbStride);
};

struct TUNEDFUNC_CONVERT_YUVRGB_COLORSPACE
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

struct TUNEDFUNC_CONVERT_YUVRGB
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_CONVERT_YUVRGB);
	TUNEDFUNC_CONVERT_YUVRGB_COLORSPACE bt601;
	TUNEDFUNC_CONVERT_YUVRGB_COLORSPACE bt709;
};

struct TUNEDFUNC_CONVERT_SHUFFLE
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_CONVERT_SHUFFLE);
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
};

struct TUNEDFUNC_CORRELATE
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_CORRELATE);
	void (*pfnEncorrelateInplaceBGRX)(uint8_t *pBegin, uint8_t *pEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnEncorrelateInplaceBGRA)(uint8_t *pBegin, uint8_t *pEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnEncorrelateInplaceXRGB)(uint8_t *pBegin, uint8_t *pEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnEncorrelateInplaceARGB)(uint8_t *pBegin, uint8_t *pEnd, size_t cbWidth, ssize_t scbStride);
};

struct TUNEDFUNC
{
	const TUNEDFUNC_PREDICT *pPredict;
	const TUNEDFUNC_HUFFMAN_ENCODE *pHuffmanEncode;
	const TUNEDFUNC_HUFFMAN_DECODE *pHuffmanDecode;
	const TUNEDFUNC_CONVERT_YUVRGB *pConvertYUVRGB;
	const TUNEDFUNC_CONVERT_SHUFFLE *pConvertShuffle;
	const TUNEDFUNC_CORRELATE *pCorrelate;
};

extern TUNEDFUNC tfn;
extern const TUNEDFUNC_PREDICT tfnPredictCPP;
extern const TUNEDFUNC_HUFFMAN_ENCODE tfnHuffmanEncodeCPP;
extern const TUNEDFUNC_HUFFMAN_DECODE tfnHuffmanDecodeCPP;
extern const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBCPP;
extern const TUNEDFUNC_CONVERT_SHUFFLE tfnConvertShuffleCPP;
extern const TUNEDFUNC_CORRELATE tfnCorrelateCPP;

void ResolveTunedFunc(const TUNEDFUNC *ptfnRoot, const uint32_t *pdwSupportedFeatures);


template<class C> class CYUVTunedFunc;

template<> class CYUVTunedFunc<CBT601Coefficient>
{
public:
	static const TUNEDFUNC_CONVERT_YUVRGB_COLORSPACE &tf() { return tfn.pConvertYUVRGB->bt601; }
};

template<> class CYUVTunedFunc<CBT709Coefficient>
{
public:
	static const TUNEDFUNC_CONVERT_YUVRGB_COLORSPACE &tf() { return tfn.pConvertYUVRGB->bt709; }
};


#define PredictWrongMedianAndCount tfn.pPredict->pfnPredictWrongMedianAndCount
#define PredictLeftAndCount tfn.pPredict->pfnPredictLeftAndCount
#define RestoreWrongMedian tfn.pPredict->pfnRestoreWrongMedian
#define RestoreWrongMedianBlock4 tfn.pPredict->pfnRestoreWrongMedianBlock4

#define HuffmanEncode tfn.pHuffmanEncode->pfnHuffmanEncode
#define HuffmanDecode tfn.pHuffmanDecode->pfnHuffmanDecode
#define HuffmanDecodeStep4 tfn.pHuffmanDecode->pfnHuffmanDecodeStep4
#define HuffmanDecodeAndAccum tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccum
#define HuffmanDecodeAndAccumStep2 tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep2
#define HuffmanDecodeAndAccumStep3 tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep3
#define HuffmanDecodeAndAccumStep4 tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep4
#define HuffmanDecodeAndAccumStep3ForBGRBlue tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep3ForBGRBlue
#define HuffmanDecodeAndAccumStep3ForBGRRed tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep3ForBGRRed
#define HuffmanDecodeAndAccumStep4ForBGRXBlue tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep4ForBGRXBlue
#define HuffmanDecodeAndAccumStep4ForBGRXRed tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep4ForBGRXRed
#define HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha
#define HuffmanDecodeAndAccumStep3ForRGBBlue HuffmanDecodeAndAccumStep3ForBGRRed
#define HuffmanDecodeAndAccumStep3ForRGBRed HuffmanDecodeAndAccumStep3ForBGRBlue
#define HuffmanDecodeAndAccumStep4ForXRGBBlue HuffmanDecodeAndAccumStep4ForBGRXRed
#define HuffmanDecodeAndAccumStep4ForXRGBRed HuffmanDecodeAndAccumStep4ForBGRXBlue
#define HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha tfn.pHuffmanDecode->pfnHuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha

#define ConvertULY2ToBGR(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToBGR
#define ConvertULY2ToBGRX(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToBGRX
#define ConvertULY2ToRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToRGB
#define ConvertULY2ToXRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToXRGB
#define ConvertBGRToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertBGRToULY2
#define ConvertBGRXToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertBGRXToULY2
#define ConvertRGBToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertRGBToULY2
#define ConvertXRGBToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertXRGBToULY2

#define ConvertBGRToULRG tfn.pConvertShuffle->pfnConvertBGRToULRG
#define ConvertRGBToULRG(g, b, r, p, q, w, s) ConvertBGRToULRG(g, r, b, p, q, w, s)
#define ConvertBGRXToULRG tfn.pConvertShuffle->pfnConvertBGRXToULRG
#define ConvertXRGBToULRG tfn.pConvertShuffle->pfnConvertXRGBToULRG
#define ConvertBGRAToULRA tfn.pConvertShuffle->pfnConvertBGRAToULRA
#define ConvertARGBToULRA tfn.pConvertShuffle->pfnConvertARGBToULRA
#define ConvertYUYVToULY2 tfn.pConvertShuffle->pfnConvertYUYVToULY2
#define ConvertUYVYToULY2 tfn.pConvertShuffle->pfnConvertUYVYToULY2
#define ConvertULRGToBGR tfn.pConvertShuffle->pfnConvertULRGToBGR
#define ConvertULRGToRGB(p, q, g, b, r, w, s) ConvertULRGToBGR(p, q, g, r, b, w, s)
#define ConvertULRGToBGRX tfn.pConvertShuffle->pfnConvertULRGToBGRX
#define ConvertULRGToXRGB tfn.pConvertShuffle->pfnConvertULRGToXRGB
#define ConvertULRAToBGRA tfn.pConvertShuffle->pfnConvertULRAToBGRA
#define ConvertULRAToARGB tfn.pConvertShuffle->pfnConvertULRAToARGB
#define ConvertULY2ToYUYV tfn.pConvertShuffle->pfnConvertULY2ToYUYV
#define ConvertULY2ToUYVY tfn.pConvertShuffle->pfnConvertULY2ToUYVY

#define EncorrelateInplaceBGRX tfn.pCorrelate->pfnEncorrelateInplaceBGRX
#define EncorrelateInplaceBGRA tfn.pCorrelate->pfnEncorrelateInplaceBGRA
#define EncorrelateInplaceXRGB tfn.pCorrelate->pfnEncorrelateInplaceXRGB
#define EncorrelateInplaceARGB tfn.pCorrelate->pfnEncorrelateInplaceARGB
