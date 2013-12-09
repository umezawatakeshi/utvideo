/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id $ */

#pragma once

#if defined(_MSC_VER)

#include <stdlib.h>

static inline uint16_t bswap16(uint16_t x) { return _byteswap_ushort(x); }
static inline uint32_t bswap32(uint32_t x) { return _byteswap_ulong(x); }
static inline uint64_t bswap64(uint64_t x) { return _byteswap_uint64(x); }

#elif defined(__GNUC__)

/* gcc do not have __builtin_bswap16() */
static inline uint16_t bswap16(uint16_t x) { return (x >> 8) | (x << 8); }
static inline uint32_t bswap32(uint32_t x) { return __builtin_bswap32(x); }
static inline uint64_t bswap64(uint64_t x) { return __builtin_bswap64(x); }

#endif


#if 1 /* XXX */

/* little endian */

static inline uint16_t htol16(uint16_t x) { return x; }
static inline uint16_t ltoh16(uint16_t x) { return x; }
static inline uint16_t htob16(uint16_t x) { return bswap16(x); }
static inline uint16_t btoh16(uint16_t x) { return bswap16(x); }

static inline uint32_t htol32(uint32_t x) { return x; }
static inline uint32_t ltoh32(uint32_t x) { return x; }
static inline uint32_t htob32(uint32_t x) { return bswap32(x); }
static inline uint32_t btoh32(uint32_t x) { return bswap32(x); }

static inline uint64_t htol64(uint64_t x) { return x; }
static inline uint64_t ltoh64(uint64_t x) { return x; }
static inline uint64_t htob64(uint64_t x) { return bswap64(x); }
static inline uint64_t btoh64(uint64_t x) { return bswap64(x); }

#elif 0 /* XXX */

/* big endian */

static inline uint16_t htol16(uint16_t x) { return bswap16(x); }
static inline uint16_t ltoh16(uint16_t x) { return bswap16(x); }
static inline uint16_t htob16(uint16_t x) { return x; }
static inline uint16_t btoh16(uint16_t x) { return x; }

static inline uint32_t htol32(uint32_t x) { return bswap32(x); }
static inline uint32_t ltoh32(uint32_t x) { return bswap32(x); }
static inline uint32_t htob32(uint32_t x) { return x; }
static inline uint32_t btoh32(uint32_t x) { return x; }

static inline uint64_t htol64(uint64_t x) { return bswap64(x); }
static inline uint64_t ltoh64(uint64_t x) { return bswap64(x); }
static inline uint64_t htob64(uint64_t x) { return x; }
static inline uint64_t btoh64(uint64_t x) { return x; }

#else

/* neither little nor big */

#error Host endian is neither little endian nor big endian

#endif
