/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once

#include "TunedFunc.h"

#define ConvertULY2ToBottomupRGB24 tfn.pfnConvertULY2ToBottomupRGB24
#define ConvertULY2ToBottomupRGB32 tfn.pfnConvertULY2ToBottomupRGB32
#define ConvertBottomupRGB24ToULY2 tfn.pfnConvertBottomupRGB24ToULY2
#define ConvertBottomupRGB32ToULY2 tfn.pfnConvertBottomupRGB32ToULY2

void cpp_ConvertULY2ToBottomupRGB24(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
void cpp_ConvertULY2ToBottomupRGB32(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
void cpp_ConvertBottomupRGB24ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);
void cpp_ConvertBottomupRGB32ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);

#ifdef _WIN32_X86
extern "C" void x86_sse2_ConvertULY2ToBottomupRGB24(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x86_sse2_ConvertULY2ToBottomupRGB32(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x86_sse2_ConvertBottomupRGB24ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);
extern "C" void x86_sse2_ConvertBottomupRGB32ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);
#endif

#ifdef _WIN64_X64
extern "C" void x64_sse2_ConvertULY2ToBottomupRGB24(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x64_sse2_ConvertULY2ToBottomupRGB32(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride);
extern "C" void x64_sse2_ConvertBottomupRGB24ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);
extern "C" void x64_sse2_ConvertBottomupRGB32ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride);
#endif