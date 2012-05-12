/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

/* minimal stdint.h (not C99 compliant) for Visual C++ 2008 and older */

#pragma once

#if !defined(_MSC_VER) || _MSC_VER >= 1600 /* Visual C++ 2010 or later */
#error This stdint.h is for Visual C++ 2008 or older
#endif

typedef signed char      int8_t;
typedef signed short     int16_t;
typedef signed int       int32_t;
typedef signed __int64   int64_t;

#define INT8_MAX  (0x7f)
#define INT16_MAX (0x7fff)
#define INT32_MAX (0x7fffffff)
#define INT64_MAX (0x7fffffffffffffffLL)

#define INT8_MIN  (-0x7f-1)
#define INT16_MIN (-0x7fff-1)
#define INT32_MIN (-0x7fffffff-1)
#define INT64_MIN (-0x7fffffffffffffffLL-1)

typedef unsigned char    uint8_t;
typedef unsigned short   uint16_t;
typedef unsigned int     uint32_t;
typedef unsigned __int64 uint64_t;

#define UINT8_MAX  (0xffU)
#define UINT16_MAX (0xffffU)
#define UINT32_MAX (0xffffffffU)
#define UINT64_MAX (0xffffffffffffffffULL)

#ifdef _WIN64
#define INTPTR_MAX  INT64_MAX
#define INTPTR_MIN  INT64_MIN
#define UINTPTR_MAX UINT64_MAX
#else
#define INTPTR_MAX  INT32_MAX
#define INTPTR_MIN  INT32_MIN
#define UINTPTR_MAX UINT32_MAX
#endif
