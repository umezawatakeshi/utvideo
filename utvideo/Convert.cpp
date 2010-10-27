/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "Convert.h"

static void cpp_ConvertULY2ToBottomupRGB(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride, int bypp)
{
	const BYTE *y = pYBegin;
	const BYTE *u = pUBegin;
	const BYTE *v = pVBegin;

	_ASSERT(dwStride == ROUNDUP(dwDataStride, 4));

	for (BYTE *pStrideBegin = pDstEnd - dwStride; pStrideBegin >= pDstBegin; pStrideBegin -= dwStride)
	{
		BYTE *pStrideEnd = pStrideBegin + dwDataStride;
		for (BYTE *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			BYTE *q = p + bypp;
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
				*(p+3) = 0xff;
			y++; u++; v++;
		}
	}
}

void cpp_ConvertULY2ToBottomupRGB24(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride)
{
	cpp_ConvertULY2ToBottomupRGB(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, dwStride, dwDataStride, 3);
}

void cpp_ConvertULY2ToBottomupRGB32(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, size_t dwStride, size_t dwDataStride)
{
	cpp_ConvertULY2ToBottomupRGB(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, dwStride, dwDataStride, 4);
}

static void cpp_ConvertBottomupRGBToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride, int bypp)
{
	BYTE *y = pYBegin;
	BYTE *u = pUBegin;
	BYTE *v = pVBegin;

	_ASSERT(dwStride == ROUNDUP(dwDataStride, 4));

	for (const BYTE *pStrideBegin = pSrcEnd - dwStride; pStrideBegin >= pSrcBegin; pStrideBegin -= dwStride)
	{
		const BYTE *pStrideEnd = pStrideBegin + dwDataStride;
		for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += bypp * 2)
		{
			const BYTE *q = p + bypp;
			*(y+0) = min(max(int((*(p+0))*0.098 + (*(p+1))*0.504 + (*(p+2))*0.257 + 16.5), 16), 235);
			*(y+1) = min(max(int((*(q+0))*0.098 + (*(q+1))*0.504 + (*(q+2))*0.257 + 16.5), 16), 235);
			*u     = min(max(int(((*(p+0)+*(q+0))*0.439 + (*(p+1)+*(q+1))*-0.291 + (*(p+2)+*(q+2))*-0.148)/2 + 128.5), 16), 240);
			*v     = min(max(int(((*(p+0)+*(q+0))*-0.071 + (*(p+1)+*(q+1))*-0.368 + (*(p+2)+*(q+2))*0.439)/2 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
	}
}

void cpp_ConvertBottomupRGB24ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride)
{
	cpp_ConvertBottomupRGBToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, dwStride, dwDataStride, 3);
}

void cpp_ConvertBottomupRGB32ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, size_t dwStride, size_t dwDataStride)
{
	cpp_ConvertBottomupRGBToULY2(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, dwStride, dwDataStride, 4);
}
