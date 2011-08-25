/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "TunedFunc.h"

#define ConvertULY2ToBottomupRGB24 tfn.pfnConvertULY2ToBottomupRGB24
#define ConvertULY2ToBottomupRGB32 tfn.pfnConvertULY2ToBottomupRGB32
#define ConvertBottomupRGB24ToULY2 tfn.pfnConvertBottomupRGB24ToULY2
#define ConvertBottomupRGB32ToULY2 tfn.pfnConvertBottomupRGB32ToULY2

void cpp_ConvertULY2ToBottomupRGB24(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t dwStride, size_t dwDataStride);
void cpp_ConvertULY2ToBottomupRGB32(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t dwStride, size_t dwDataStride);
void cpp_ConvertBottomupRGB24ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, size_t dwDataStride);
void cpp_ConvertBottomupRGB32ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, size_t dwDataStride);

#ifdef _WIN32_X86
extern "C" void x86_sse2_ConvertULY2ToBottomupRGB24(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x86_sse2_ConvertULY2ToBottomupRGB32(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x86_sse2_ConvertBottomupRGB24ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, size_t dwDataStride);
extern "C" void x86_sse2_ConvertBottomupRGB32ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, size_t dwDataStride);
#endif

#ifdef _WIN64_X64
extern "C" void x64_sse2_ConvertULY2ToBottomupRGB24(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x64_sse2_ConvertULY2ToBottomupRGB32(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x64_sse2_ConvertBottomupRGB24ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, size_t dwDataStride);
extern "C" void x64_sse2_ConvertBottomupRGB32ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, size_t dwDataStride);
#endif
