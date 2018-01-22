/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

#ifdef _MSC_VER
#include <intrin.h>
#endif

#ifdef __GNUC__
#include <x86intrin.h>
#endif

static inline __m128i _mm_setone_si128()
{
	/*
	 * Clang には _mm_undefined_si128 は無いらしい。
	 * Visual C++ では _mm_setzero_si128 に置き換えられる。
	 * どうせ処理の他の部分で all 0 を使うので、この処理でも問題なかろう。
	 */
	__m128i x = _mm_setzero_si128(); // _mm_undefined_si128();
	return _mm_cmpeq_epi32(x, x);
}

static inline __m128i _mm_set2_epi8(char a, char b)
{
	return _mm_set_epi8(a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b);
}

static inline __m128i _mm_set2_epi16(short a, short b)
{
	return _mm_set_epi16(a, b, a, b, a, b, a, b);
}

static inline __m128i _mm_set4_epi16(short a, short b, short c, short d)
{
	return _mm_set_epi16(a, b, c, d, a, b, c, d);
}

static inline __m128i _mm_set2_epi16_shift(double a, double b, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	return _mm_set2_epi16(aa, bb);
}

static inline __m128i _mm_set4_epi16_shift(double a, double b, double c, double d, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	short cc = short(c * (1 << shift));
	short dd = short(d * (1 << shift));
	return _mm_set4_epi16(aa, bb, cc, dd);
}


#if defined(__AVX2__)

static inline __m256i _mm256_setone_si256()
{
	__m256i x = _mm256_setzero_si256();
	return _mm256_cmpeq_epi32(x, x);
}

static inline __m256i _mm256_set2_epi16(short a, short b)
{
	return _mm256_set_epi16(a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b);
}

static inline __m256i _mm256_set4_epi16(short a, short b, short c, short d)
{
	return _mm256_set_epi16(a, b, c, d, a, b, c, d, a, b, c, d, a, b, c, d);
}

static inline __m256i _mm256_set2_epi16_shift(double a, double b, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	return _mm256_set2_epi16(aa, bb);
}

static inline __m256i _mm256_set4_epi16_shift(double a, double b, double c, double d, int shift)
{
	short aa = short(a * (1 << shift));
	short bb = short(b * (1 << shift));
	short cc = short(c * (1 << shift));
	short dd = short(d * (1 << shift));
	return _mm256_set4_epi16(aa, bb, cc, dd);
}

static inline __m256i _mm256_set16_epi8(char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0)
{
	return _mm256_set_epi8(
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0,
		e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}

#endif
