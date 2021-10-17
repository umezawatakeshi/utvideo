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
	void (*pfnPredictCylindricalWrongMedianAndCount8)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
	void (*pfnPredictCylindricalWrongMedian8)(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbStride);
	void (*pfnPredictCylindricalLeftAndCount8)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, uint32_t pCountTable[][256]);
	void (*pfnRestoreCylindricalWrongMedian8)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
	void(*pfnRestoreCylindricalLeft8)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
	void(*pfnPredictCylindricalLeftAndCount10)(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, uint32_t pCountTable[][1024]);
	void(*pfnRestoreCylindricalLeft10)(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
	void(*pfnPredictPlanarGradientAndCount8)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
	void(*pfnPredictPlanarGradient8)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
	void(*pfnRestorePlanarGradient8)(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
	void(*pfnPredictPlanarGradientAndCount10)(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride, uint32_t pCountTable[][1024]);
	void(*pfnRestorePlanarGradient10)(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride);
};

struct TUNEDFUNC_HUFFMAN_ENCODE
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_HUFFMAN_ENCODE);
	size_t (*pfnHuffmanEncode8)(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable);
	size_t (*pfnHuffmanEncode10)(uint8_t *pDstBegin, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<10> *pEncodeTable);
};

struct TUNEDFUNC_HUFFMAN_DECODE
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_HUFFMAN_DECODE);
	uint8_t* (*pfnHuffmanDecode8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable);
	uint16_t* (*pfnHuffmanDecode10)(uint16_t *pDstBegin, uint16_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<10> *pDecodeTable);
};

struct TUNEDFUNC_CONVERT_YUVRGB_COLORSPACE
{
	void (*pfnConvertULY2ToBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertULY2ToBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertULY2ToRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertULY2ToXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertBGRToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertBGRXToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertXRGBToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void(*pfnConvertULY4ToBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertULY4ToBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertULY4ToRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertULY4ToXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertBGRToULY4)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertBGRXToULY4)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertRGBToULY4)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertXRGBToULY4)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void(*pfnConvertULY0ToBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
	void(*pfnConvertULY0ToBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
	void(*pfnConvertULY0ToRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
	void(*pfnConvertULY0ToXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
	void(*pfnConvertBGRToULY0)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
	void(*pfnConvertBGRXToULY0)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
	void(*pfnConvertRGBToULY0)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
	void(*pfnConvertXRGBToULY0)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
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
	void (*pfnConvertBGRToULRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertBGRXToULRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertXRGBToULRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertBGRAToULRA)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertARGBToULRA)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertYUYVToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertUYVYToULY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertULRGToBGR)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertULRGToBGRX)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertULRGToXRGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertULRAToBGRA)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertULRAToARGB)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
	void (*pfnConvertULY2ToYUYV)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertULY2ToUYVY)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
	void (*pfnConvertB48rToUQRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertB64aToUQRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertB64aToUQRA)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUQRGToB48r)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUQRGToB64a)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUQRAToB64a)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertV210ToUQY2)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
	void (*pfnConvertUQY2ToV210)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, unsigned int nWidth, ssize_t scbStride);
	void (*pfnConvertR210ToUQRG)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
	void (*pfnConvertUQRGToR210)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
	void (*pfnConvertLittleEndian16ToHostEndian10Limited)(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
	void (*pfnConvertLittleEndian16ToHostEndian10Noround)(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
	void (*pfnConvertHostEndian10ToLittleEndian16Limited)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrc);
	void (*pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
	void (*pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
	void (*pfnConvertPlanarHostEndian10ToPackedUVLittleEndian16Limited)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin);
};

struct TUNEDFUNC_SYMPACK
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_SYMPACK);
	void (*pfnPack8SymAfterPredictPlanarGradient8)(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
	void (*pfnUnpack8SymAndRestorePlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbStride);
	void (*pfnPack8SymWithDiff8)(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const uint8_t *pPrevBegin, size_t cbStride);
	void (*pfnUnpack8SymWithDiff8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, const uint8_t *pPrevBegin, size_t cbStride);
};

struct TUNEDFUNC_CONVERT_PREDICT
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_CONVERT_PREDICT);
	void (*pfnConvertBGRToULRG_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertBGRXToULRG_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertXRGBToULRG_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertBGRToULRG_PredictPlanarGradientAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertBGRXToULRG_PredictPlanarGradientAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertXRGBToULRG_PredictPlanarGradientAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertBGRToULRG_PredictCylindricalWrongMedianAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertBGRXToULRG_PredictCylindricalWrongMedianAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertXRGBToULRG_PredictCylindricalWrongMedianAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
	void (*pfnConvertBGRToULRG_PredictCylindricalWrongMedian)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRXToULRG_PredictCylindricalWrongMedian)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertXRGBToULRG_PredictCylindricalWrongMedian)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRAToULRA_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
	void (*pfnConvertARGBToULRA_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
	void (*pfnConvertBGRAToULRA_PredictPlanarGradientAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
	void (*pfnConvertARGBToULRA_PredictPlanarGradientAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
	void (*pfnConvertBGRAToULRA_PredictCylindricalWrongMedianAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
	void (*pfnConvertARGBToULRA_PredictCylindricalWrongMedianAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
	void (*pfnConvertBGRAToULRA_PredictCylindricalWrongMedian)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, uint8_t* pABegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertARGBToULRA_PredictCylindricalWrongMedian)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, uint8_t* pABegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGR_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGRX_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToXRGB_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGR_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGRX_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToXRGB_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGR_RestoreCylindricalWrongMedian)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGRX_RestoreCylindricalWrongMedian)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToXRGB_RestoreCylindricalWrongMedian)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToBGRA_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToARGB_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToBGRA_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToARGB_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToBGRA_RestoreCylindricalWrongMedian)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToARGB_RestoreCylindricalWrongMedian)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertYUYVToULY2_PredictCylindricalLeftAndCount)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertUYVYToULY2_PredictCylindricalLeftAndCount)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertYUYVToULY2_PredictPlanarGradientAndCount)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertUYVYToULY2_PredictPlanarGradientAndCount)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertYUYVToULY2_PredictCylindricalWrongMedianAndCount)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertUYVYToULY2_PredictCylindricalWrongMedianAndCount)(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertYUYVToULY2_PredictCylindricalWrongMedian)(uint8_t* pYBegin, uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUYVYToULY2_PredictCylindricalWrongMedian)(uint8_t* pYBegin, uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToYUYV_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToUYVY_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToYUYV_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToUYVY_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToYUYV_RestoreCylindricalWrongMedian)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToUYVY_RestoreCylindricalWrongMedian)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertPackedUVToPlanar_PredictCylindricalLeftAndCount)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertPackedUVToPlanar_PredictPlanarGradientAndCount)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
	void (*pfnConvertPackedUVToPlanar_PredictCylindricalWrongMedian)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertPlanarToPackedUV_RestoreCylindricalLeft)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertPlanarToPackedUV_RestorePlanarGradient)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertPlanarToPackedUV_RestoreCylindricalWrongMedian)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertB64aToUQRG_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
	void (*pfnConvertB64aToUQRA_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);
	void (*pfnConvertB64aToUQRG_PredictPlanarGradientAndCount)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
	void (*pfnConvertB64aToUQRA_PredictPlanarGradientAndCount)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, uint8_t* pABegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);
	void (*pfnConvertUQRGToB64a_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUQRAToB64a_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUQRGToB64a_RestorePlanarGradient)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUQRAToB64a_RestorePlanarGradient)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, const uint8_t* pABegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertR210ToUQRG_PredictCylindricalLeftAndCount)(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
	void (*pfnConvertR210ToUQRG_PredictPlanarGradientAndCount)(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
	void (*pfnConvertUQRGToR210_RestoreCylindricalLeft)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
	void (*pfnConvertUQRGToR210_RestorePlanarGradient)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
	void (*pfnConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount)(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
	void (*pfnConvertLittleEndian16ToHostEndian10Noround_PredictCylindricalLeftAndCount)(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
	void (*pfnConvertLittleEndian16ToHostEndian10Limited_PredictPlanarGradientAndCount)(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
	void (*pfnConvertLittleEndian16ToHostEndian10Noround_PredictPlanarGradientAndCount)(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
	void (*pfnConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertHostEndian16ToLittleEndian16_RestorePlanarGradient)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictCylindricalLeftAndCount)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
	void (*pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictCylindricalLeftAndCount)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
	void (*pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictPlanarGradientAndCount)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
	void (*pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictPlanarGradientAndCount)(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
	void (*pfnConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient)(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
};

struct TUNEDFUNC_CONVERT_SYMPACK
{
	DECLARE_TUNEDFUNC_FRAGMENT_HEADER(TUNEDFUNC_CONVERT_SYMPACK);
	void (*pfnConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8)(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRXToULRG_Pack8SymAfterPredictPlanarGradient8)(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertXRGBToULRG_Pack8SymAfterPredictPlanarGradient8)(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertBGRAToULRA_Pack8SymAfterPredictPlanarGradient8)(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertARGBToULRA_Pack8SymAfterPredictPlanarGradient8)(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGR_Unpack8SymAndRestorePlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToBGRX_Unpack8SymAndRestorePlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRGToXRGB_Unpack8SymAndRestorePlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToBGRA_Unpack8SymAndRestorePlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, const uint8_t *pAPacked, const uint8_t *pAControl, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULRAToARGB_Unpack8SymAndRestorePlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, const uint8_t *pAPacked, const uint8_t *pAControl, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertYUYVToULY2_Pack8SymAfterPredictPlanarGradient8)(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertUYVYToULY2_Pack8SymAfterPredictPlanarGradient8)(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToYUYV_Unpack8SymAndRestorePredictPlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYPacked, const uint8_t *pYControl, const uint8_t *pUPacked, const uint8_t *pUControl, const uint8_t *pVPacked, const uint8_t *pVControl, size_t cbWidth, ssize_t scbStride);
	void (*pfnConvertULY2ToUYVY_Unpack8SymAndRestorePredictPlanarGradient8)(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYPacked, const uint8_t *pYControl, const uint8_t *pUPacked, const uint8_t *pUControl, const uint8_t *pVPacked, const uint8_t *pVControl, size_t cbWidth, ssize_t scbStride);
};

struct TUNEDFUNC
{
	const TUNEDFUNC_PREDICT *pPredict;
	const TUNEDFUNC_HUFFMAN_ENCODE *pHuffmanEncode;
	const TUNEDFUNC_HUFFMAN_DECODE *pHuffmanDecode;
	const TUNEDFUNC_CONVERT_YUVRGB *pConvertYUVRGB;
	const TUNEDFUNC_CONVERT_SHUFFLE *pConvertShuffle;
	const TUNEDFUNC_SYMPACK *pSymPack;
	const TUNEDFUNC_CONVERT_PREDICT *pConvertPredict;
	const TUNEDFUNC_CONVERT_SYMPACK *pConvertSymPack;
};

extern TUNEDFUNC tfn;
extern const TUNEDFUNC_PREDICT tfnPredictCPP;
extern const TUNEDFUNC_HUFFMAN_ENCODE tfnHuffmanEncodeCPP;
extern const TUNEDFUNC_HUFFMAN_DECODE tfnHuffmanDecodeCPP;
extern const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBCPP;
extern const TUNEDFUNC_CONVERT_SHUFFLE tfnConvertShuffleCPP;
extern const TUNEDFUNC_SYMPACK tfnSymPackCPP;
extern const TUNEDFUNC_CONVERT_PREDICT tfnConvertPredictCPP;
extern const TUNEDFUNC_CONVERT_SYMPACK tfnConvertSymPackCPP;

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


#define PredictCylindricalWrongMedianAndCount8 tfn.pPredict->pfnPredictCylindricalWrongMedianAndCount8
#define PredictCylindricalWrongMedian8 tfn.pPredict->pfnPredictCylindricalWrongMedian8
#define PredictCylindricalLeftAndCount8 tfn.pPredict->pfnPredictCylindricalLeftAndCount8
#define RestoreCylindricalWrongMedian8 tfn.pPredict->pfnRestoreCylindricalWrongMedian8
#define RestoreCylindricalLeft8 tfn.pPredict->pfnRestoreCylindricalLeft8
#define PredictCylindricalLeftAndCount10 tfn.pPredict->pfnPredictCylindricalLeftAndCount10
#define RestoreCylindricalLeft10 tfn.pPredict->pfnRestoreCylindricalLeft10
#define PredictPlanarGradientAndCount8 tfn.pPredict->pfnPredictPlanarGradientAndCount8
#define PredictPlanarGradient8 tfn.pPredict->pfnPredictPlanarGradient8
#define RestorePlanarGradient8 tfn.pPredict->pfnRestorePlanarGradient8
#define PredictPlanarGradientAndCount10 tfn.pPredict->pfnPredictPlanarGradientAndCount10
#define RestorePlanarGradient10 tfn.pPredict->pfnRestorePlanarGradient10

#define HuffmanEncode8 tfn.pHuffmanEncode->pfnHuffmanEncode8
#define HuffmanEncode10 tfn.pHuffmanEncode->pfnHuffmanEncode10
#define HuffmanDecode8 tfn.pHuffmanDecode->pfnHuffmanDecode8
#define HuffmanDecode10 tfn.pHuffmanDecode->pfnHuffmanDecode10

#define ConvertULY2ToBGR(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToBGR
#define ConvertULY2ToBGRX(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToBGRX
#define ConvertULY2ToRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToRGB
#define ConvertULY2ToXRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY2ToXRGB
#define ConvertBGRToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertBGRToULY2
#define ConvertBGRXToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertBGRXToULY2
#define ConvertRGBToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertRGBToULY2
#define ConvertXRGBToULY2(C) CYUVTunedFunc<C>::tf().pfnConvertXRGBToULY2

#define ConvertULY4ToBGR(C) CYUVTunedFunc<C>::tf().pfnConvertULY4ToBGR
#define ConvertULY4ToBGRX(C) CYUVTunedFunc<C>::tf().pfnConvertULY4ToBGRX
#define ConvertULY4ToRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY4ToRGB
#define ConvertULY4ToXRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY4ToXRGB
#define ConvertBGRToULY4(C) CYUVTunedFunc<C>::tf().pfnConvertBGRToULY4
#define ConvertBGRXToULY4(C) CYUVTunedFunc<C>::tf().pfnConvertBGRXToULY4
#define ConvertRGBToULY4(C) CYUVTunedFunc<C>::tf().pfnConvertRGBToULY4
#define ConvertXRGBToULY4(C) CYUVTunedFunc<C>::tf().pfnConvertXRGBToULY4

#define ConvertULY0ToBGR(C) CYUVTunedFunc<C>::tf().pfnConvertULY0ToBGR
#define ConvertULY0ToBGRX(C) CYUVTunedFunc<C>::tf().pfnConvertULY0ToBGRX
#define ConvertULY0ToRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY0ToRGB
#define ConvertULY0ToXRGB(C) CYUVTunedFunc<C>::tf().pfnConvertULY0ToXRGB
#define ConvertBGRToULY0(C) CYUVTunedFunc<C>::tf().pfnConvertBGRToULY0
#define ConvertBGRXToULY0(C) CYUVTunedFunc<C>::tf().pfnConvertBGRXToULY0
#define ConvertRGBToULY0(C) CYUVTunedFunc<C>::tf().pfnConvertRGBToULY0
#define ConvertXRGBToULY0(C) CYUVTunedFunc<C>::tf().pfnConvertXRGBToULY0

#define ConvertBGRToULRG tfn.pConvertShuffle->pfnConvertBGRToULRG
#define ConvertRGBToULRG(g, b, r, p, q, w, s, pw) ConvertBGRToULRG(g, r, b, p, q, w, s, pw)
#define ConvertBGRXToULRG tfn.pConvertShuffle->pfnConvertBGRXToULRG
#define ConvertXRGBToULRG tfn.pConvertShuffle->pfnConvertXRGBToULRG
#define ConvertBGRAToULRA tfn.pConvertShuffle->pfnConvertBGRAToULRA
#define ConvertARGBToULRA tfn.pConvertShuffle->pfnConvertARGBToULRA
#define ConvertYUYVToULY2 tfn.pConvertShuffle->pfnConvertYUYVToULY2
#define ConvertUYVYToULY2 tfn.pConvertShuffle->pfnConvertUYVYToULY2
#define ConvertULRGToBGR tfn.pConvertShuffle->pfnConvertULRGToBGR
#define ConvertULRGToRGB(p, q, g, b, r, w, s, pw) ConvertULRGToBGR(p, q, g, r, b, w, s, pw)
#define ConvertULRGToBGRX tfn.pConvertShuffle->pfnConvertULRGToBGRX
#define ConvertULRGToXRGB tfn.pConvertShuffle->pfnConvertULRGToXRGB
#define ConvertULRAToBGRA tfn.pConvertShuffle->pfnConvertULRAToBGRA
#define ConvertULRAToARGB tfn.pConvertShuffle->pfnConvertULRAToARGB
#define ConvertULY2ToYUYV tfn.pConvertShuffle->pfnConvertULY2ToYUYV
#define ConvertULY2ToUYVY tfn.pConvertShuffle->pfnConvertULY2ToUYVY
#define ConvertB48rToUQRG tfn.pConvertShuffle->pfnConvertB48rToUQRG
#define ConvertB64aToUQRG tfn.pConvertShuffle->pfnConvertB64aToUQRG
#define ConvertB64aToUQRA tfn.pConvertShuffle->pfnConvertB64aToUQRA
#define ConvertUQRGToB48r tfn.pConvertShuffle->pfnConvertUQRGToB48r
#define ConvertUQRGToB64a tfn.pConvertShuffle->pfnConvertUQRGToB64a
#define ConvertUQRAToB64a tfn.pConvertShuffle->pfnConvertUQRAToB64a
#define ConvertV210ToUQY2 tfn.pConvertShuffle->pfnConvertV210ToUQY2
#define ConvertUQY2ToV210 tfn.pConvertShuffle->pfnConvertUQY2ToV210
#define ConvertR210ToUQRG tfn.pConvertShuffle->pfnConvertR210ToUQRG
#define ConvertUQRGToR210 tfn.pConvertShuffle->pfnConvertUQRGToR210
#define ConvertLittleEndian16ToHostEndian10Limited tfn.pConvertShuffle->pfnConvertLittleEndian16ToHostEndian10Limited
#define ConvertLittleEndian16ToHostEndian10Noround tfn.pConvertShuffle->pfnConvertLittleEndian16ToHostEndian10Noround
#define ConvertHostEndian10ToLittleEndian16Limited tfn.pConvertShuffle->pfnConvertHostEndian10ToLittleEndian16Limited
#define ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited tfn.pConvertShuffle->pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited
#define ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround tfn.pConvertShuffle->pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround
#define ConvertPlanarHostEndian10ToPackedUVLittleEndian16Limited tfn.pConvertShuffle->pfnConvertPlanarHostEndian10ToPackedUVLittleEndian16Limited

#define Pack8SymAfterPredictPlanarGradient8 tfn.pSymPack->pfnPack8SymAfterPredictPlanarGradient8
#define Unpack8SymAndRestorePlanarGradient8 tfn.pSymPack->pfnUnpack8SymAndRestorePlanarGradient8
#define Pack8SymWithDiff8 tfn.pSymPack->pfnPack8SymWithDiff8
#define Unpack8SymWithDiff8 tfn.pSymPack->pfnUnpack8SymWithDiff8

#define ConvertBGRToULRG_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertBGRToULRG_PredictCylindricalLeftAndCount
#define ConvertRGBToULRG_PredictCylindricalLeftAndCount(g, b, r, p, q, w, s, gc, bc, rc) ConvertBGRToULRG_PredictCylindricalLeftAndCount(g, r, b, p, q, w, s, gc, rc, bc)
#define ConvertBGRXToULRG_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertBGRXToULRG_PredictCylindricalLeftAndCount
#define ConvertXRGBToULRG_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertXRGBToULRG_PredictCylindricalLeftAndCount
#define ConvertBGRToULRG_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertBGRToULRG_PredictPlanarGradientAndCount
#define ConvertRGBToULRG_PredictPlanarGradientAndCount(g, b, r, p, q, w, s, gc, bc, rc) ConvertBGRToULRG_PredictPlanarGradientAndCount(g, r, b, p, q, w, s, gc, rc, bc)
#define ConvertBGRXToULRG_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertBGRXToULRG_PredictPlanarGradientAndCount
#define ConvertXRGBToULRG_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertXRGBToULRG_PredictPlanarGradientAndCount
#define ConvertBGRToULRG_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertBGRToULRG_PredictCylindricalWrongMedianAndCount
#define ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount(g, b, r, p, q, w, s, gc, bc, rc) ConvertBGRToULRG_PredictCylindricalWrongMedianAndCount(g, r, b, p, q, w, s, gc, rc, bc)
#define ConvertBGRXToULRG_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertBGRXToULRG_PredictCylindricalWrongMedianAndCount
#define ConvertXRGBToULRG_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertXRGBToULRG_PredictCylindricalWrongMedianAndCount
#define ConvertBGRToULRG_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertBGRToULRG_PredictCylindricalWrongMedian
#define ConvertRGBToULRG_PredictCylindricalWrongMedian(g, b, r, p, q, w, s) ConvertBGRToULRG_PredictCylindricalWrongMedian(g, r, b, p, q, w, s)
#define ConvertBGRXToULRG_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertBGRXToULRG_PredictCylindricalWrongMedian
#define ConvertXRGBToULRG_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertXRGBToULRG_PredictCylindricalWrongMedian
#define ConvertBGRAToULRA_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertBGRAToULRA_PredictCylindricalLeftAndCount
#define ConvertARGBToULRA_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertARGBToULRA_PredictCylindricalLeftAndCount
#define ConvertBGRAToULRA_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertBGRAToULRA_PredictPlanarGradientAndCount
#define ConvertARGBToULRA_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertARGBToULRA_PredictPlanarGradientAndCount
#define ConvertBGRAToULRA_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertBGRAToULRA_PredictCylindricalWrongMedianAndCount
#define ConvertARGBToULRA_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertARGBToULRA_PredictCylindricalWrongMedianAndCount
#define ConvertBGRAToULRA_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertBGRAToULRA_PredictCylindricalWrongMedian
#define ConvertARGBToULRA_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertARGBToULRA_PredictCylindricalWrongMedian
#define ConvertULRGToBGR_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertULRGToBGR_RestoreCylindricalLeft
#define ConvertULRGToRGB_RestoreCylindricalLeft(p, q, g, b, r, w, s) ConvertULRGToBGR_RestoreCylindricalLeft(p, q, g, r, b, w, s)
#define ConvertULRGToBGRX_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertULRGToBGRX_RestoreCylindricalLeft
#define ConvertULRGToXRGB_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertULRGToXRGB_RestoreCylindricalLeft
#define ConvertULRGToBGR_RestorePlanarGradient tfn.pConvertPredict->pfnConvertULRGToBGR_RestorePlanarGradient
#define ConvertULRGToRGB_RestorePlanarGradient(p, q, g, b, r, w, s) ConvertULRGToBGR_RestorePlanarGradient(p, q, g, r, b, w, s)
#define ConvertULRGToBGRX_RestorePlanarGradient tfn.pConvertPredict->pfnConvertULRGToBGRX_RestorePlanarGradient
#define ConvertULRGToXRGB_RestorePlanarGradient tfn.pConvertPredict->pfnConvertULRGToXRGB_RestorePlanarGradient
#define ConvertULRGToBGR_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertULRGToBGR_RestoreCylindricalWrongMedian
#define ConvertULRGToRGB_RestoreCylindricalWrongMedian(p, q, g, b, r, w, s) ConvertULRGToBGR_RestoreCylindricalWrongMedian(p, q, g, r, b, w, s)
#define ConvertULRGToBGRX_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertULRGToBGRX_RestoreCylindricalWrongMedian
#define ConvertULRGToXRGB_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertULRGToXRGB_RestoreCylindricalWrongMedian
#define ConvertULRAToBGRA_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertULRAToBGRA_RestoreCylindricalLeft
#define ConvertULRAToARGB_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertULRAToARGB_RestoreCylindricalLeft
#define ConvertULRAToBGRA_RestorePlanarGradient tfn.pConvertPredict->pfnConvertULRAToBGRA_RestorePlanarGradient
#define ConvertULRAToARGB_RestorePlanarGradient tfn.pConvertPredict->pfnConvertULRAToARGB_RestorePlanarGradient
#define ConvertULRAToBGRA_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertULRAToBGRA_RestoreCylindricalWrongMedian
#define ConvertULRAToARGB_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertULRAToARGB_RestoreCylindricalWrongMedian
#define ConvertYUYVToULY2_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertYUYVToULY2_PredictCylindricalLeftAndCount
#define ConvertUYVYToULY2_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertUYVYToULY2_PredictCylindricalLeftAndCount
#define ConvertYUYVToULY2_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertYUYVToULY2_PredictPlanarGradientAndCount
#define ConvertUYVYToULY2_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertUYVYToULY2_PredictPlanarGradientAndCount
#define ConvertYUYVToULY2_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertYUYVToULY2_PredictCylindricalWrongMedianAndCount
#define ConvertUYVYToULY2_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertUYVYToULY2_PredictCylindricalWrongMedianAndCount
#define ConvertYUYVToULY2_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertYUYVToULY2_PredictCylindricalWrongMedian
#define ConvertUYVYToULY2_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertUYVYToULY2_PredictCylindricalWrongMedian
#define ConvertULY2ToYUYV_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertULY2ToYUYV_RestoreCylindricalLeft
#define ConvertULY2ToUYVY_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertULY2ToUYVY_RestoreCylindricalLeft
#define ConvertULY2ToYUYV_RestorePlanarGradient tfn.pConvertPredict->pfnConvertULY2ToYUYV_RestorePlanarGradient
#define ConvertULY2ToUYVY_RestorePlanarGradient tfn.pConvertPredict->pfnConvertULY2ToUYVY_RestorePlanarGradient
#define ConvertULY2ToYUYV_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertULY2ToYUYV_RestoreCylindricalWrongMedian
#define ConvertULY2ToUYVY_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertULY2ToUYVY_RestoreCylindricalWrongMedian
#define ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertPackedUVToPlanar_PredictCylindricalLeftAndCount
#define ConvertPackedUVToPlanar_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertPackedUVToPlanar_PredictPlanarGradientAndCount
#define ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount tfn.pConvertPredict->pfnConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount
#define ConvertPackedUVToPlanar_PredictCylindricalWrongMedian tfn.pConvertPredict->pfnConvertPackedUVToPlanar_PredictCylindricalWrongMedian
#define ConvertPlanarToPackedUV_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertPlanarToPackedUV_RestoreCylindricalLeft
#define ConvertPlanarToPackedUV_RestorePlanarGradient tfn.pConvertPredict->pfnConvertPlanarToPackedUV_RestorePlanarGradient
#define ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian tfn.pConvertPredict->pfnConvertPlanarToPackedUV_RestoreCylindricalWrongMedian
#define ConvertB64aToUQRG_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertB64aToUQRG_PredictCylindricalLeftAndCount
#define ConvertB64aToUQRA_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertB64aToUQRA_PredictCylindricalLeftAndCount
#define ConvertB64aToUQRG_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertB64aToUQRG_PredictPlanarGradientAndCount
#define ConvertB64aToUQRA_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertB64aToUQRA_PredictPlanarGradientAndCount
#define ConvertUQRGToB64a_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertUQRGToB64a_RestoreCylindricalLeft
#define ConvertUQRAToB64a_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertUQRAToB64a_RestoreCylindricalLeft
#define ConvertUQRGToB64a_RestorePlanarGradient tfn.pConvertPredict->pfnConvertUQRGToB64a_RestorePlanarGradient
#define ConvertUQRAToB64a_RestorePlanarGradient tfn.pConvertPredict->pfnConvertUQRAToB64a_RestorePlanarGradient
#define ConvertR210ToUQRG_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertR210ToUQRG_PredictCylindricalLeftAndCount
#define ConvertR210ToUQRG_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertR210ToUQRG_PredictPlanarGradientAndCount
#define ConvertUQRGToR210_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertUQRGToR210_RestoreCylindricalLeft
#define ConvertUQRGToR210_RestorePlanarGradient tfn.pConvertPredict->pfnConvertUQRGToR210_RestorePlanarGradient
#define ConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertLittleEndian16ToHostEndian10Limited_PredictCylindricalLeftAndCount
#define ConvertLittleEndian16ToHostEndian10Noround_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertLittleEndian16ToHostEndian10Noround_PredictCylindricalLeftAndCount
#define ConvertLittleEndian16ToHostEndian10Limited_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertLittleEndian16ToHostEndian10Limited_PredictPlanarGradientAndCount
#define ConvertLittleEndian16ToHostEndian10Noround_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertLittleEndian16ToHostEndian10Noround_PredictPlanarGradientAndCount
#define ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft
#define ConvertHostEndian16ToLittleEndian16_RestorePlanarGradient tfn.pConvertPredict->pfnConvertHostEndian16ToLittleEndian16_RestorePlanarGradient
#define ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictCylindricalLeftAndCount
#define ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictCylindricalLeftAndCount tfn.pConvertPredict->pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictCylindricalLeftAndCount
#define ConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Limited_PredictPlanarGradientAndCount
#define ConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictPlanarGradientAndCount tfn.pConvertPredict->pfnConvertPackedUVLittleEndian16ToPlanarHostEndian10Noround_PredictPlanarGradientAndCount
#define ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft tfn.pConvertPredict->pfnConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft
#define ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient tfn.pConvertPredict->pfnConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient

#define ConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8
#define ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8(gp, gcb, gc, bp, bcb, bc, rp, rcb, rc, p, q, w, s) ConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8(gp, gcb, gc, rp, rcb, rc, bp, bcb, bc, p, q, w, s)
#define ConvertBGRXToULRG_Pack8SymAfterPredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertBGRXToULRG_Pack8SymAfterPredictPlanarGradient8
#define ConvertXRGBToULRG_Pack8SymAfterPredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertXRGBToULRG_Pack8SymAfterPredictPlanarGradient8
#define ConvertBGRAToULRA_Pack8SymAfterPredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertBGRAToULRA_Pack8SymAfterPredictPlanarGradient8
#define ConvertARGBToULRA_Pack8SymAfterPredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertARGBToULRA_Pack8SymAfterPredictPlanarGradient8
#define ConvertULRGToBGR_Unpack8SymAndRestorePlanarGradient8 tfn.pConvertSymPack->pfnConvertULRGToBGR_Unpack8SymAndRestorePlanarGradient8
#define ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8(p, q, gp, gc, bp, bc, rp, rc, w, s) ConvertULRGToBGR_Unpack8SymAndRestorePlanarGradient8(p, q, gp, gc, rp, rc, bp, bc, w, s)
#define ConvertULRGToBGRX_Unpack8SymAndRestorePlanarGradient8 tfn.pConvertSymPack->pfnConvertULRGToBGRX_Unpack8SymAndRestorePlanarGradient8
#define ConvertULRGToXRGB_Unpack8SymAndRestorePlanarGradient8 tfn.pConvertSymPack->pfnConvertULRGToXRGB_Unpack8SymAndRestorePlanarGradient8
#define ConvertULRAToBGRA_Unpack8SymAndRestorePlanarGradient8 tfn.pConvertSymPack->pfnConvertULRAToBGRA_Unpack8SymAndRestorePlanarGradient8
#define ConvertULRAToARGB_Unpack8SymAndRestorePlanarGradient8 tfn.pConvertSymPack->pfnConvertULRAToARGB_Unpack8SymAndRestorePlanarGradient8
#define ConvertYUYVToULY2_Pack8SymAfterPredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertYUYVToULY2_Pack8SymAfterPredictPlanarGradient8
#define ConvertUYVYToULY2_Pack8SymAfterPredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertUYVYToULY2_Pack8SymAfterPredictPlanarGradient8
#define ConvertULY2ToYUYV_Unpack8SymAndRestorePredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertULY2ToYUYV_Unpack8SymAndRestorePredictPlanarGradient8
#define ConvertULY2ToUYVY_Unpack8SymAndRestorePredictPlanarGradient8 tfn.pConvertSymPack->pfnConvertULY2ToUYVY_Unpack8SymAndRestorePredictPlanarGradient8
