/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

template<int F, class T> void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template<int F, class T> void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template<int F, class T> void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template<int F, class T> void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);
template<int F, class T> void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256]);
template<int F, class T> void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][256], uint32_t pBCountTable[][256], uint32_t pRCountTable[][256], uint32_t pACountTable[][256]);

template<int F, class T> void tuned_ConvertULRGToRGB_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULRGToRGB_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULRAToRGBA_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);

template<int F, class T> void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template<int F, class T> void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template<int F, class T> void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pYCountTable[][256], uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);

template<int F, class T> void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);

template<int F> void tuned_ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template<int F> void tuned_ConvertPackedUVToPlanar_PredictPlanarGradientAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);
template<int F> void tuned_ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][256], uint32_t pVCountTable[][256]);

template<int F> void tuned_ConvertPlanarToPackedUV_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template<int F> void tuned_ConvertPlanarToPackedUV_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template<int F> void tuned_ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);

template<int F> void tuned_ConvertB64aToUQRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template<int F> void tuned_ConvertB64aToUQRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);
template<int F> void tuned_ConvertB64aToUQRG_PredictPlanarGradientAndCount(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template<int F> void tuned_ConvertB64aToUQRA_PredictPlanarGradientAndCount(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, uint8_t* pABegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024], uint32_t pACountTable[][1024]);

template<int F> void tuned_ConvertUQRGToB64a_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template<int F> void tuned_ConvertUQRAToB64a_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template<int F> void tuned_ConvertUQRGToB64a_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, size_t cbWidth, ssize_t scbStride);
template<int F> void tuned_ConvertUQRAToB64a_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, const uint8_t* pABegin, size_t cbWidth, ssize_t scbStride);

template<int F> void tuned_ConvertR210ToUQRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);
template<int F> void tuned_ConvertR210ToUQRG_PredictPlanarGradientAndCount(uint8_t* pGBegin, uint8_t* pBBegin, uint8_t* pRBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, unsigned int nWidth, ssize_t scbStride, uint32_t pGCountTable[][1024], uint32_t pBCountTable[][1024], uint32_t pRCountTable[][1024]);

template<int F> void tuned_ConvertUQRGToR210_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);
template<int F> void tuned_ConvertUQRGToR210_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pGBegin, const uint8_t* pBBegin, const uint8_t* pRBegin, unsigned int nWidth, ssize_t scbStride);

template<int F, VALUERANGE VR> void tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template<int F, VALUERANGE VR> void tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount(uint8_t* pDstBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pCountTable[][1024]);
template<int F> void tuned_ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);
template<int F> void tuned_ConvertHostEndian16ToLittleEndian16_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrcBegin, size_t cbWidth, ssize_t scbStride);

template<int F, VALUERANGE VR> void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template<int F, VALUERANGE VR> void tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t pUCountTable[][1024], uint32_t pVCountTable[][1024]);
template<int F> void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
template<int F> void tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin, size_t cbWidth, ssize_t scbStride);
