/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "SymPack.h"
#include "TunedFunc.h"

void cpp_Pack8Sym8(uint8_t *pPacked, size_t *cbPacked, uint8_t *pControl, size_t *cbControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;
	memset(pControl, 0, (pSrcEnd - pSrcBegin) / 64 * 3);

	for (auto p = pSrcBegin; p != pSrcEnd; p += 8)
	{
		uint64_t w = *(uint64_t*)p;

		int mode;
		int bits = 0;

		if (w == 0)
		{
			mode = 0;
			bits = 0;
		}
		else
		{
			auto signs = w & 0x8080808080808080ULL; // •„†bit‚¾‚¯‚ğ’Šo
			signs = (signs << 1) - (signs >> 7); // •„†bit‚ğbyte‚É“WŠJ

			uint64_t z = w ^ signs;
			z = z | (z >> 32);
			z = z | (z >> 16);
			z = z | (z >> 8);
			z &= 0xff;
			if (z & 0x40)
				bits = 8;
			else if (z & 0x20)
				bits = 7;
			else if (z & 0x10)
				bits = 6;
			else if (z & 0x08)
				bits = 5;
			else if (z & 0x04)
				bits = 4;
			else if (z & 0x02)
				bits = 3;
			else
				bits = 2;
			int rembits = 8 - bits;
			mode = bits - 1;

			// ’l‚Ì’²®

			uint64_t offset = 0x8080808080808080ULL >> rembits;
			uint64_t offadj = (w & 0x8080808080808080ULL) << 1; // packed byte Œ¸Z‚Å‚Í‚È‚¢‚Ì‚Åˆø‚«‚·‚¬‚½•ª‚ğ•â³‚·‚é
			w += offset;
			w -= offadj;

			w = (w & 0x00ff00ff00ff00ffULL) | ((w & 0xff00ff00ff00ff00ULL) >> rembits);
			w = (w & 0x0000ffff0000ffffULL) | ((w & 0xffff0000ffff0000ULL) >> rembits * 2);
			w = (w & 0x00000000ffffffffULL) | ((w & 0xffffffff00000000ULL) >> rembits * 4);
		}
		*(uint64_t*)q = w;
		q += bits;
		*(uint32_t *)r |= (mode << shift);
		shift += 3;
		if (shift == 24)
		{
			r += 3;
			shift = 0;
		}
	}

	*cbPacked = q - pPacked;
	*cbControl = r - pControl;
}

void cpp_Unpack8Sym8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pPacked, const uint8_t *pControl, size_t cbControl)
{
	int shift = 0;
	auto q = pPacked;
	auto r = pControl;

	for (auto p = pDstBegin; p != pDstEnd; p += 8)
	{
		uint64_t w;
		int bits = ((*(uint32_t *)r) >> shift) & 7;
		if (bits == 0)
			w = 0;
		else
		{
			uint64_t z, mask;

			w = *(uint64_t *)q;
			bits++;
			int rembits = 8 - bits;

			z = w << rembits * 4;
			w = (w & 0x00000000ffffffffULL) | (z & 0xffffffff00000000ULL);

			z = w << rembits * 2;
			w = (w & 0x0000ffff0000ffffULL) | (z & 0xffff0000ffff0000ULL);

			z = w << rembits;
			w = (w & 0x00ff00ff00ff00ffULL) | (z & 0xff00ff00ff00ff00ULL);

			mask = (0x101010101010101ULL << bits) - 0x101010101010101ULL;
			w &= mask;

			uint64_t offset = 0x8080808080808080ULL >> rembits;
			uint64_t offadj = (~w & offset) << (rembits + 1); // packed byte Œ¸Z‚Å‚Í‚È‚¢‚Ì‚Åˆø‚«‚·‚¬‚½•ª‚ğ•â³‚·‚é
			w -= offset;
			w += offadj;
		}

		q += bits;
		*(uint64_t*)p = w;

		shift += 3;
		if (shift == 24)
		{
			r += 3;
			shift = 0;
		}
	}
}
