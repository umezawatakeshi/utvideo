/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

#include <stdint.h>
#include <inttypes.h>

#if defined(_MSC_VER)
#define PRISZT_PREFIX "I"
#elif defined(__GNUC__)
#define PRISZT_PREFIX "z"
#else
#error
#endif

#define PRIdSZT PRISZT_PREFIX "d"
#define PRIuSZT PRISZT_PREFIX "u"
#define PRIoSZT PRISZT_PREFIX "o"
#define PRIxSZT PRISZT_PREFIX "x"
#define PRIXSZT PRISZT_PREFIX "X"
