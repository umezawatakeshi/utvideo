/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

#if defined(_MSC_VER)

#include <stdlib.h>

static inline uint16_t bswap16(uint16_t x) { return _byteswap_ushort(x); }
static inline uint32_t bswap32(uint32_t x) { return _byteswap_ulong(x); }
static inline uint64_t bswap64(uint64_t x) { return _byteswap_uint64(x); }

#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321
#define PDP_ENDIAN    3412
#define BYTE_ORDER    LITTLE_ENDIAN // 現在の Windows はリトルエンディアンの環境しかサポートしておらず、MSVC は Windows 用である。

#elif defined(__GNUC__)

static inline uint16_t bswap16(uint16_t x) { return __builtin_bswap16(x); }
static inline uint32_t bswap32(uint32_t x) { return __builtin_bswap32(x); }
static inline uint64_t bswap64(uint64_t x) { return __builtin_bswap64(x); }

#include <endian.h>

#endif


#if BYTE_ORDER == LITTLE_ENDIAN

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

#elif BYTE_ORDER == BIG_ENDIAN

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
