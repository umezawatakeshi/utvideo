/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

template<int F> void tuned_Pack8SymAfterPredictPlanarGradient8(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride);
template<int F> void tuned_Unpack8SymAndRestorePlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbStride);
