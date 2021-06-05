/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#ifdef _MSC_VER
#include <intrin.h>
#endif

#ifdef __GNUC__
#include <x86intrin.h>
#endif

template<typename VT> static inline VT _mmt_loadu(const void* p);
template<typename VT> static inline void _mmt_storeu(void* p, VT v);
template<typename VT> static inline VT _mmt_set1_epi8(char e0);
template<typename VT> static inline VT _mmt_set1_epi16(short e0);
template<typename VT> static inline VT _mmt_add_epi8(VT v0, VT v1);
template<typename VT> static inline VT _mmt_sub_epi8(VT v0, VT v1);


static inline FORCEINLINE __m128i _mm_setone_si128()
{
#if defined(_MSC_VER)
	__m128i x = _mm_undefined_si128();
	return _mm_cmpeq_epi32(x, x);
#elif defined(__GNUC__)
	// GCC Ç‚ Clang ÇÕÅAÇ±ÇÍÇ one idiom (pcmpeqd) Ç…ÇµÇƒÇ≠ÇÍÇÈ
	// Visual C++ ÇÕÉ_ÉÅ
	return _mm_set1_epi8(-1);
#else
#error
#endif
}

static inline FORCEINLINE __m128i _mm_set2_epi8(char a, char b)
{
	return _mm_set_epi8(a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b);
}

static inline FORCEINLINE __m128i _mm_set8_epi8(char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0)
{
	return _mm_set_epi8(
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0
	);
}

static inline FORCEINLINE __m128i _mm_set2_epi16(short a, short b)
{
	return _mm_set_epi16(a, b, a, b, a, b, a, b);
}

static inline FORCEINLINE __m128i _mm_set4_epi16(short a, short b, short c, short d)
{
	return _mm_set_epi16(a, b, c, d, a, b, c, d);
}

static inline FORCEINLINE __m128i _mm_set2_epi16_shift(double a, double b, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	return _mm_set2_epi16(aa, bb);
}

static inline FORCEINLINE __m128i _mm_set4_epi16_shift(double a, double b, double c, double d, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	short cc = short(c * (1 << shift));
	short dd = short(d * (1 << shift));
	return _mm_set4_epi16(aa, bb, cc, dd);
}


template<> static inline FORCEINLINE __m128i _mmt_loadu<__m128i>(const void* p)
{
	return _mm_loadu_si128((const __m128i*)p);
}

template<> static inline FORCEINLINE void _mmt_storeu<__m128i>(void* p, __m128i v)
{
	_mm_storeu_si128((__m128i*)p, v);
}

template<> static inline FORCEINLINE __m128i _mmt_set1_epi8<__m128i>(char e0)
{
	return _mm_set1_epi8(e0);
}

template<> static inline FORCEINLINE __m128i _mmt_set1_epi16<__m128i>(short e0)
{
	return _mm_set1_epi16(e0);
}

template<> static inline FORCEINLINE __m128i _mmt_add_epi8<__m128i>(__m128i v0, __m128i v1)
{
	return _mm_add_epi8(v0, v1);
}

template<> static inline FORCEINLINE __m128i _mmt_sub_epi8<__m128i>(__m128i v0, __m128i v1)
{
	return _mm_sub_epi8(v0, v1);
}


#if defined(__AVX2__)

static inline FORCEINLINE __m256i _mm256_setone_si256()
{
#if defined(_MSC_VER)
	__m256i x = _mm256_undefined_si256();
	return _mm256_cmpeq_epi32(x, x);
#elif defined(__GNUC__)
	return _mm256_set1_epi8(-1);
#else
#error
#endif
}

static inline FORCEINLINE __m256i _mm256_set2_epi16(short a, short b)
{
	return _mm256_set_epi16(a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b);
}

static inline FORCEINLINE __m256i _mm256_set4_epi16(short a, short b, short c, short d)
{
	return _mm256_set_epi16(a, b, c, d, a, b, c, d, a, b, c, d, a, b, c, d);
}

static inline FORCEINLINE __m256i _mm256_set2_epi16_shift(double a, double b, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	return _mm256_set2_epi16(aa, bb);
}

static inline FORCEINLINE __m256i _mm256_set4_epi16_shift(double a, double b, double c, double d, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	short cc = short(c * (1 << shift));
	short dd = short(d * (1 << shift));
	return _mm256_set4_epi16(aa, bb, cc, dd);
}

static inline FORCEINLINE __m256i _mm256_set16_epi8(char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0)
{
	return _mm256_set_epi8(
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0,
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}


template<> static inline FORCEINLINE __m256i _mmt_loadu<__m256i>(const void* p)
{
	return _mm256_loadu_si256((const __m256i*)p);
}

template<> static inline FORCEINLINE void _mmt_storeu<__m256i>(void* p, __m256i v)
{
	_mm256_storeu_si256((__m256i*)p, v);
}

template<> static inline FORCEINLINE __m256i _mmt_set1_epi8<__m256i>(char e0)
{
	return _mm256_set1_epi8(e0);
}

template<> static inline FORCEINLINE __m256i _mmt_set1_epi16<__m256i>(short e0)
{
	return _mm256_set1_epi16(e0);
}

template<> static inline FORCEINLINE __m256i _mmt_add_epi8<__m256i>(__m256i v0, __m256i v1)
{
	return _mm256_add_epi8(v0, v1);
}

template<> static inline FORCEINLINE __m256i _mmt_sub_epi8<__m256i>(__m256i v0, __m256i v1)
{
	return _mm256_sub_epi8(v0, v1);
}

#endif

#if defined(__AVX512F__)

static inline FORCEINLINE __m512i _mm512_setone_si512()
{
	return _mm512_set1_epi8(-1);
}

static inline FORCEINLINE __m512i _mm512_set2_epi16(short a, short b)
{
	return _mm512_set_epi16(
		a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b,
		a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b
	);
}

static inline FORCEINLINE __m512i _mm512_set4_epi16(short a, short b, short c, short d)
{
	return _mm512_set_epi16(
		a, b, c, d, a, b, c, d, a, b, c, d, a, b, c, d,
		a, b, c, d, a, b, c, d, a, b, c, d, a, b, c, d
	);
}

static inline FORCEINLINE __m512i _mm512_set2_epi16_shift(double a, double b, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	return _mm512_set2_epi16(aa, bb);
}

static inline FORCEINLINE __m512i _mm512_set4_epi16_shift(double a, double b, double c, double d, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	short cc = short(c * (1 << shift));
	short dd = short(d * (1 << shift));
	return _mm512_set4_epi16(aa, bb, cc, dd);
}

static inline FORCEINLINE __m512i _mm512_set8_epi8(char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0)
{
	return _mm512_set_epi8(
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0,
		e7, e6, e5, e4, e3, e2, e1, e0
	);
}

static inline FORCEINLINE __m512i _mm512_set16_epi8(char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0)
{
	return _mm512_set_epi8(
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0,
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0,
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0,
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0
	);
}


template<> static inline FORCEINLINE __m512i _mmt_loadu<__m512i>(const void* p)
{
	return _mm512_loadu_si512((const __m512i*)p);
}

template<> static inline FORCEINLINE void _mmt_storeu<__m512i>(void* p, __m512i v)
{
	_mm512_storeu_si512((__m512i*)p, v);
}

template<> static inline FORCEINLINE __m512i _mmt_set1_epi8<__m512i>(char e0)
{
	return _mm512_set1_epi8(e0);
}

template<> static inline FORCEINLINE __m512i _mmt_set1_epi16<__m512i>(short e0)
{
	return _mm512_set1_epi16(e0);
}

template<> static inline FORCEINLINE __m512i _mmt_add_epi8<__m512i>(__m512i v0, __m512i v1)
{
	return _mm512_add_epi8(v0, v1);
}

template<> static inline FORCEINLINE __m512i _mmt_sub_epi8<__m512i>(__m512i v0, __m512i v1)
{
	return _mm512_sub_epi8(v0, v1);
}

#endif
