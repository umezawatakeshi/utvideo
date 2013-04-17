/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Convert.h"

static void cpp_ConvertULY2ToBottomupBGRx(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth, int bypp)
{
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (uint8_t *pStrideBegin = pDstEnd - cbGrossWidth; pStrideBegin >= pDstBegin; pStrideBegin -= cbGrossWidth)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbNetWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			uint8_t *q = p + bypp;
			*(p+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(p+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(p+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			if (bypp == 4)
				*(p+3) = 0xff;
			y++;
			*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			if (bypp == 4)
				*(q+3) = 0xff;
			y++; u++; v++;
		}
	}
}

void cpp_ConvertULY2ToBottomupBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertULY2ToBottomupBGRx(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbGrossWidth, cbNetWidth, 3);
}

void cpp_ConvertULY2ToBottomupBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertULY2ToBottomupBGRx(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbGrossWidth, cbNetWidth, 4);
}


static void cpp_ConvertULY2ToTopdownBGRx(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth, int bypp)
{
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin < pDstEnd; pStrideBegin += cbGrossWidth)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbNetWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			uint8_t *q = p + bypp;
			*(p+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(p+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(p+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			if (bypp == 4)
				*(p+3) = 0xff;
			y++;
			*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			if (bypp == 4)
				*(q+3) = 0xff;
			y++; u++; v++;
		}
	}
}

void cpp_ConvertULY2ToTopdownBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertULY2ToTopdownBGRx(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbGrossWidth, cbNetWidth, 3);
}

void cpp_ConvertULY2ToTopdownBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertULY2ToTopdownBGRx(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbGrossWidth, cbNetWidth, 4);
}


static void cpp_ConvertULY2ToTopdownxRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth, int bypp)
{
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin < pDstEnd; pStrideBegin += cbGrossWidth)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbNetWidth;
		for (uint8_t *pp = pStrideBegin; pp < pStrideEnd; pp += bypp * 2)
		{
			uint8_t *p = bypp == 3 ? pp : pp + 1;
			uint8_t *q = p + bypp;
			*(p+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(p+2) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(p+0) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			if (bypp == 4)
				*(p-1) = 0xff;
			y++;
			*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(q+2) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(q+0) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			if (bypp == 4)
				*(q-1) = 0xff;
			y++; u++; v++;
		}
	}
}

void cpp_ConvertULY2ToTopdownRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertULY2ToTopdownxRGB(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbGrossWidth, cbNetWidth, 3);
}

void cpp_ConvertULY2ToTopdownXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertULY2ToTopdownxRGB(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbGrossWidth, cbNetWidth, 4);
}

//

static void cpp_ConvertBottomupBGRxToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth, int bypp)
{
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;

	for (const uint8_t *pStrideBegin = pSrcEnd - cbGrossWidth; pStrideBegin >= pSrcBegin; pStrideBegin -= cbGrossWidth)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbNetWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			const uint8_t *q = p + bypp;
			*(y+0) = min(max(int((*(p+0))*0.098 + (*(p+1))*0.504 + (*(p+2))*0.257 + 16.5), 16), 235);
			*(y+1) = min(max(int((*(q+0))*0.098 + (*(q+1))*0.504 + (*(q+2))*0.257 + 16.5), 16), 235);
			*u     = min(max(int(((*(p+0)+*(q+0))*0.439 + (*(p+1)+*(q+1))*-0.291 + (*(p+2)+*(q+2))*-0.148)/2 + 128.5), 16), 240);
			*v     = min(max(int(((*(p+0)+*(q+0))*-0.071 + (*(p+1)+*(q+1))*-0.368 + (*(p+2)+*(q+2))*0.439)/2 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
	}
}

void cpp_ConvertBottomupBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertBottomupBGRxToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbGrossWidth, cbNetWidth, 3);
}

void cpp_ConvertBottomupBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertBottomupBGRxToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbGrossWidth, cbNetWidth, 4);
}


static void cpp_ConvertTopdownBGRxToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth, int bypp)
{
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin < pSrcEnd; pStrideBegin += cbGrossWidth)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbNetWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			const uint8_t *q = p + bypp;
			*(y+0) = min(max(int((*(p+0))*0.098 + (*(p+1))*0.504 + (*(p+2))*0.257 + 16.5), 16), 235);
			*(y+1) = min(max(int((*(q+0))*0.098 + (*(q+1))*0.504 + (*(q+2))*0.257 + 16.5), 16), 235);
			*u     = min(max(int(((*(p+0)+*(q+0))*0.439 + (*(p+1)+*(q+1))*-0.291 + (*(p+2)+*(q+2))*-0.148)/2 + 128.5), 16), 240);
			*v     = min(max(int(((*(p+0)+*(q+0))*-0.071 + (*(p+1)+*(q+1))*-0.368 + (*(p+2)+*(q+2))*0.439)/2 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
	}
}

void cpp_ConvertTopdownBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertTopdownBGRxToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbGrossWidth, cbNetWidth, 3);
}

void cpp_ConvertTopdownBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertTopdownBGRxToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbGrossWidth, cbNetWidth, 4);
}


static void cpp_ConvertTopdownxRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth, int bypp)
{
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;
	
	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin < pSrcEnd; pStrideBegin += cbGrossWidth)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbNetWidth;
		for (const uint8_t *pp = pStrideBegin; pp < pStrideEnd; pp += bypp * 2)
		{
			const uint8_t *p = bypp == 3 ? pp : pp + 1;
			const uint8_t *q = p + bypp;
			*(y+0) = min(max(int((*(p+2))*0.098 + (*(p+1))*0.504 + (*(p+0))*0.257 + 16.5), 16), 235);
			*(y+1) = min(max(int((*(q+2))*0.098 + (*(q+1))*0.504 + (*(q+0))*0.257 + 16.5), 16), 235);
			*u     = min(max(int(((*(p+2)+*(q+2))*0.439 + (*(p+1)+*(q+1))*-0.291 + (*(p+0)+*(q+0))*-0.148)/2 + 128.5), 16), 240);
			*v     = min(max(int(((*(p+2)+*(q+2))*-0.071 + (*(p+1)+*(q+1))*-0.368 + (*(p+0)+*(q+0))*0.439)/2 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
	}
}

void cpp_ConvertTopdownRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertTopdownxRGBToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbGrossWidth, cbNetWidth, 3);
}

void cpp_ConvertTopdownXRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbGrossWidth, size_t cbNetWidth)
{
	cpp_ConvertTopdownxRGBToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbGrossWidth, cbNetWidth, 4);
}

void cpp_ConvertBGRToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pStrideBegin, *p;
	uint8_t *g = pGBegin;
	uint8_t *b = pBBegin;
	uint8_t *r = pRBegin;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 3)
		{
			*g++ = *(p+1);
			*b++ = *(p+0) - *(p+1) + 0x80;
			*r++ = *(p+2) - *(p+1) + 0x80;
		}
	}
}

void cpp_ConvertBGRXToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pStrideBegin, *p;
	uint8_t *g = pGBegin;
	uint8_t *b = pBBegin;
	uint8_t *r = pRBegin;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*g++ = *(p+1);
			*b++ = *(p+0) - *(p+1) + 0x80;
			*r++ = *(p+2) - *(p+1) + 0x80;
		}
	}
}

void cpp_ConvertXRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pStrideBegin, *p;
	uint8_t *g = pGBegin;
	uint8_t *b = pBBegin;
	uint8_t *r = pRBegin;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*g++ = *(p+2);
			*b++ = *(p+3) - *(p+2) + 0x80;
			*r++ = *(p+1) - *(p+2) + 0x80;
		}
	}
}

void cpp_ConvertBGRAToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pStrideBegin, *p;
	uint8_t *g = pGBegin;
	uint8_t *b = pBBegin;
	uint8_t *r = pRBegin;
	uint8_t *a = pABegin;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*g++ = *(p+1);
			*b++ = *(p+0) - *(p+1) + 0x80;
			*r++ = *(p+2) - *(p+1) + 0x80;
			*a++ = *(p+3);
		}
	}
}

void cpp_ConvertARGBToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pStrideBegin, *p;
	uint8_t *g = pGBegin;
	uint8_t *b = pBBegin;
	uint8_t *r = pRBegin;
	uint8_t *a = pABegin;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*g++ = *(p+2);
			*b++ = *(p+3) - *(p+2) + 0x80;
			*r++ = *(p+1) - *(p+2) + 0x80;
			*a++ = *(p+0);
		}
	}
}

void cpp_ConvertYUYVToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	const uint8_t *p;
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;

	for (p = pSrcBegin; p < pSrcEnd; p += 4)
	{
		*y++ = *p;
		*u++ = *(p+1);
		*y++ = *(p+2);
		*v++ = *(p+3);
	}
}

void cpp_ConvertUYVYToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd)
{
	const uint8_t *p;
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;

	for (p = pSrcBegin; p < pSrcEnd; p += 4)
	{
		*u++ = *p;
		*y++ = *(p+1);
		*v++ = *(p+2);
		*y++ = *(p+3);
	}
}
