/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

/* minimal stdint.h (not C99 compliant) for Visual C++ 2008 and older */

#pragma once

#if !defined(_MSC_VER) || _MSC_VER >= 1600 /* Visual C++ 2010 or older */
#error This stdint.h is for Visual C++ 2008 or older
#endif

typedef signed char      int8_t;
typedef signed short     int16_t;
typedef signed int       int32_t;
typedef signed __int64   int64_t;

typedef unsigned char    uint8_t;
typedef unsigned short   uint16_t;
typedef unsigned int     uint32_t;
typedef unsigned __int64 uint64_t;

#ifdef _WIN64
typedef signed __int64   intptr_t;
typedef unsigned __int64 uintptr_t;
#else
typedef signed int       intptr_t;
typedef unsigned int     uintptr_t;
#endif
