/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

template<int B>
class CSymbolBits;

template<>
class CSymbolBits<8>
{
public:
	typedef uint8_t symbol_t;

	static const symbol_t maskval = 0xff;
	static const symbol_t minval  = 0x00;
	static const symbol_t maxval  = 0xff;
	static const symbol_t midval  = 0x80;
};

template<>
class CSymbolBits<10>
{
public:
	typedef uint16_t symbol_t;

	static const symbol_t maskval = 0x3ff;
	static const symbol_t minval  = 0x000;
	static const symbol_t maxval  = 0x3ff;
	static const symbol_t midval  = 0x200;
};

template<int B>
using symbol_t = typename CSymbolBits<B>::symbol_t;
