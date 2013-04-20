/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "TunedFunc.h"

#define ConvertULY2ToBottomupBGR tfn.pfnConvertULY2ToBottomupBGR
#define ConvertULY2ToBottomupBGRX tfn.pfnConvertULY2ToBottomupBGRX
#define ConvertULY2ToTopdownBGR tfn.pfnConvertULY2ToTopdownBGR
#define ConvertULY2ToTopdownBGRX tfn.pfnConvertULY2ToTopdownBGRX
#define ConvertULY2ToTopdownRGB tfn.pfnConvertULY2ToTopdownRGB
#define ConvertULY2ToTopdownXRGB tfn.pfnConvertULY2ToTopdownXRGB
#define ConvertBottomupBGRToULY2 tfn.pfnConvertBottomupBGRToULY2
#define ConvertBottomupBGRXToULY2 tfn.pfnConvertBottomupBGRXToULY2
#define ConvertTopdownBGRToULY2 tfn.pfnConvertTopdownBGRToULY2
#define ConvertTopdownBGRXToULY2 tfn.pfnConvertTopdownBGRXToULY2
#define ConvertTopdownRGBToULY2 tfn.pfnConvertTopdownRGBToULY2
#define ConvertTopdownXRGBToULY2 tfn.pfnConvertTopdownXRGBToULY2
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

void cpp_ConvertULY2ToBottomupBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertULY2ToBottomupBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertULY2ToTopdownBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertULY2ToTopdownBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertULY2ToTopdownRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertULY2ToTopdownXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertBottomupBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertBottomupBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertTopdownBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertTopdownBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertTopdownRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertTopdownXRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
void cpp_ConvertBGRToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertBGRXToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertXRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertBGRAToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertARGBToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertYUYVToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
void cpp_ConvertUYVYToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
void cpp_ConvertULRGToBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertULRGToBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertULRGToXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertULRAToBGRA(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertULRAToARGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertULY2ToYUYV(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin);
void cpp_ConvertULY2ToUYVY(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin);

#ifdef __i386__
extern "C" void x86_sse2_ConvertULY2ToBottomupBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertULY2ToBottomupBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertULY2ToTopdownRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertULY2ToTopdownXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertULY2ToTopdownBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertULY2ToTopdownBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertBottomupBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertBottomupBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertTopdownBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertTopdownBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertTopdownRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_sse2_ConvertTopdownXRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x86_avx1_ConvertBGRToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertBGRXToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertXRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertBGRAToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertARGBToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertYUYVToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
extern "C" void x86_avx1_ConvertUYVYToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
extern "C" void x86_avx1_ConvertULRGToBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertULRGToBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertULRGToXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertULRAToBGRA(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertULRAToARGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
extern "C" void x86_avx1_ConvertULY2ToYUYV(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin);
extern "C" void x86_avx1_ConvertULY2ToUYVY(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin);
#endif

#ifdef __x86_64__
extern "C" void x64_sse2_ConvertULY2ToBottomupBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertULY2ToBottomupBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertULY2ToTopdownBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertULY2ToTopdownBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertULY2ToTopdownRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertULY2ToTopdownXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertBottomupBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertBottomupBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertTopdownBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertTopdownBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertTopdownRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
extern "C" void x64_sse2_ConvertTopdownXRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth);
#endif
