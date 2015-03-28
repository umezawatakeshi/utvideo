/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
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

#if SIZE_MAX == 4294967295U
#define PRIp "08" PRIXSZT
#else
#define PRIp "016" PRIXSZT
#endif
