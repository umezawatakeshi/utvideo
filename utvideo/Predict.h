/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "TunedFunc.h"

// intra-frame prediction
void PredictMedian(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
void PredictMedianAndCount(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
#define PredictLeftAndCount tfn.pfnPredictLeftAndCount_align1
//void RestoreMedian(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
#define RestoreMedian tfn.pfnRestoreMedian_align1

void cpp_PredictMedian(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
void cpp_PredictMedianAndCount(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
void cpp_PredictLeftAndCount(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD *pCountTable);
void cpp_RestoreMedian(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);

#ifdef _WIN32_X86
extern "C" void x86_sse2_PredictMedian_align16(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
extern "C" void x86_sse2_PredictMedianAndCount_align16(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
extern "C" void x86_sse2_PredictMedianAndCount_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
extern "C" void x86_sse2_PredictLeftAndCount_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD *pCountTable);
extern "C" void x86_i686_RestoreMedian_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
extern "C" void x86_sse1mmx_RestoreMedian_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
#endif

#ifdef _WIN64_X64
extern "C" void x64_sse2_PredictMedianAndCount_align16(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
extern "C" void x64_sse2_PredictMedianAndCount_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, DWORD *pCountTable);
extern "C" void x64_sse2_PredictLeftAndCount_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD *pCountTable);
extern "C" void x64_sse1mmx_RestoreMedian_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride);
#endif
