/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

template<int F> void tuned_PredictCylindricalLeftAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t*pSrcEnd, uint32_t pCountTable[][256]);
template<int F> void tuned_RestoreCylindricalLeft8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
template<int F> void tuned_PredictCylindricalLeftAndCount10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t*pSrcEnd, uint32_t pCountTable[][1024]);
template<int F> void tuned_RestoreCylindricalLeft10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
template<int F> void tuned_PredictCylindricalWrongMedianAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
template<int F> void tuned_PredictCylindricalWrongMedian8(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbStride);
template<int F> void tuned_RestoreCylindricalWrongMedian8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
template<int F> void tuned_PredictPlanarGradientAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
template<int F> void tuned_PredictPlanarGradient8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
template<int F> void tuned_RestorePlanarGradient8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
template<int F> void tuned_PredictPlanarGradientAndCount10(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride, uint32_t pCountTable[][1024]);
template<int F> void tuned_RestorePlanarGradient10(uint16_t* pDst, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd, size_t cbStride);
