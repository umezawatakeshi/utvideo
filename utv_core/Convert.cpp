/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Convert.h"

class CBGRPixelOrder
{
public:
	const static int A = INT_MIN;
	const static int R = 2;
	const static int G = 1;
	const static int B = 0;
	const static int BYPP = 3;
};

class CBGRAPixelOrder
{
public:
	const static int A = 3;
	const static int R = 2;
	const static int G = 1;
	const static int B = 0;
	const static int BYPP = 4;
};

class CRGBPixelOrder
{
public:
	const static int A = INT_MIN;
	const static int R = 0;
	const static int G = 1;
	const static int B = 2;
	const static int BYPP = 3;
};

class CARGBPixelOrder
{
public:
	const static int A = 0;
	const static int R = 1;
	const static int G = 2;
	const static int B = 3;
	const static int BYPP = 4;
};

class CYUYVPixelOrder
{
public:
	const static int Y0 = 0;
	const static int Y1 = 2;
	const static int U  = 1;
	const static int V  = 3;
};

class CUYVYPixelOrder
{
public:
	const static int Y0 = 1;
	const static int Y1 = 3;
	const static int U  = 0;
	const static int V  = 2;
};

static void cpp_ConvertULY2ToBGRx(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, int bypp)
{
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
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

void cpp_ConvertULY2ToBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToBGRx(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride, 3);
}

void cpp_ConvertULY2ToBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToBGRx(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride, 4);
}


static void cpp_ConvertULY2ToxRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, int bypp)
{
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
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

void cpp_ConvertULY2ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToxRGB(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride, 3);
}

void cpp_ConvertULY2ToXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertULY2ToxRGB(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride, 4);
}

//

static void cpp_ConvertBGRxToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, int bypp)
{
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
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

void cpp_ConvertBGRToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertBGRxToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, 3);
}

void cpp_ConvertBGRXToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertBGRxToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, 4);
}


static void cpp_ConvertxRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, int bypp)
{
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;
	
	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
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

void cpp_ConvertRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertxRGBToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, 3);
}

void cpp_ConvertXRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	cpp_ConvertxRGBToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, 4);
}

//

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

void cpp_ConvertULRGToBGR(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *pStrideBegin, *p;
	const uint8_t *g = pGBegin;
	const uint8_t *b = pBBegin;
	const uint8_t *r = pRBegin;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 3)
		{
			*(p+1) = *g;
			*(p+0) = *b + *g - 0x80;
			*(p+2) = *r + *g - 0x80;
			g++; b++; r++;
		}
	}
}

void cpp_ConvertULRGToBGRX(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *pStrideBegin, *p;
	const uint8_t *g = pGBegin;
	const uint8_t *b = pBBegin;
	const uint8_t *r = pRBegin;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*(p+1) = *g;
			*(p+0) = *b + *g - 0x80;
			*(p+2) = *r + *g - 0x80;
			*(p+3) = 0xff;
			g++; b++; r++;
		}
	}
}

void cpp_ConvertULRGToXRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *pStrideBegin, *p;
	const uint8_t *g = pGBegin;
	const uint8_t *b = pBBegin;
	const uint8_t *r = pRBegin;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*(p+2) = *g;
			*(p+3) = *b + *g - 0x80;
			*(p+1) = *r + *g - 0x80;
			*(p+0) = 0xff;
			g++; b++; r++;
		}
	}
}

void cpp_ConvertULRAToBGRA(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *pStrideBegin, *p;
	const uint8_t *g = pGBegin;
	const uint8_t *b = pBBegin;
	const uint8_t *r = pRBegin;
	const uint8_t *a = pABegin;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*(p+1) = *g;
			*(p+0) = *b + *g - 0x80;
			*(p+2) = *r + *g - 0x80;
			*(p+3) = *a;
			g++; b++; r++; a++;
		}
	}
}

void cpp_ConvertULRAToARGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *pStrideBegin, *p;
	const uint8_t *g = pGBegin;
	const uint8_t *b = pBBegin;
	const uint8_t *r = pRBegin;
	const uint8_t *a = pABegin;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*(p+2) = *g;
			*(p+3) = *b + *g - 0x80;
			*(p+1) = *r + *g - 0x80;
			*(p+0) = *a;
			g++; b++; r++; a++;
		}
	}
}

void cpp_ConvertULY2ToYUYV(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin)
{
	uint8_t *p;
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (p = pDstBegin; p < pDstEnd; p += 4)
	{
		*p     = *y++;
		*(p+1) = *u++;
		*(p+2) = *y++;
		*(p+3) = *v++;
	}
}

void cpp_ConvertULY2ToUYVY(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin)
{
	uint8_t *p;
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (p = pDstBegin; p < pDstEnd; p += 4)
	{
		*p     = *u++;
		*(p+1) = *y++;
		*(p+2) = *v++;
		*(p+3) = *y++;
	}
}
