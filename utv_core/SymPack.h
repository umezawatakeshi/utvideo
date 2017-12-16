/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

#include "SymbolBits.h"

// 最初の 8 はシンボルの個数、最後の 8 はシンボルのビット数（もし 10bit シンボルなら Pack8Sym10 になる）
void cpp_Pack8Sym8(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, size_t *cbControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
void cpp_Unpack8Sym8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbControl);
