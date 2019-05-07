/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

template<int F, class T> void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

template<int F, class T> void tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, const uint8_t *pAPacked, const uint8_t *pAControl, size_t cbWidth, ssize_t scbStride);
template<int F, class T> void tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYPacked, const uint8_t *pYControl, const uint8_t *pUPacked, const uint8_t *pUControl, const uint8_t *pVPacked, const uint8_t *pVControl, size_t cbWidth, ssize_t scbStride);
