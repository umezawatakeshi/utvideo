/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

//#define CODEFEATURE_SSE2        1
//#define CODEFEATURE_SSE3        2
//#define CODEFEATURE_SSSE3       3
#define CODEFEATURE_SSE41       4
#define CODEFEATURE_SSE42       5
#define CODEFEATURE_AVX1        6
#define CODEFEATURE_AVX2        7
#define CODEFEATURE_AVX512_SKX  8 // Skylake-X:         F, CD, DQ, BW, VL
#define CODEFEATURE_AVX512_CNL  9 // Cannon Lake: SKX + IFMA, VBMI
#define CODEFEATURE_AVX512_ICL 10 // Ice Lake:    CNL + VNNI, VPOPCNTDQ, VBMI2, BITALG
#define CODEFEATURE_AVX2_ADL   11 // Alder Lake:  AVX2 + AVX-VNNI
