/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "Convert.h"

void cpp_ConvertULY2ToBottomupRGB24(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, DWORD dwStride, DWORD dwDataStride)
{
	const BYTE *y = pYBegin;
	const BYTE *u = pUBegin;
	const BYTE *v = pVBegin;

	_ASSERT(dwStride == ROUNDUP(dwDataStride, 4));

	for (BYTE *pStrideBegin = pDstEnd - dwStride; pStrideBegin >= pDstBegin; pStrideBegin -= dwStride)
	{
		BYTE *pStrideEnd = pStrideBegin + dwDataStride;
		for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 6)
		{
			*(p+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(p+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(p+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			y++;
			if (p+3 < pStrideEnd)
			{
				*(p+4) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(p+3) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(p+5) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			}
			y++; u++; v++;
		}
	}
}

void cpp_ConvertULY2ToBottomupRGB32(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, DWORD dwStride, DWORD dwDataStride)
{
	const BYTE *y = pYBegin;
	const BYTE *u = pUBegin;
	const BYTE *v = pVBegin;

	_ASSERT(dwStride == dwDataStride);

	for (BYTE *pStrideBegin = pDstEnd - dwStride; pStrideBegin >= pDstBegin; pStrideBegin -= dwStride)
	{
		BYTE *pStrideEnd = pStrideBegin + dwStride;
		for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 8)
		{
			*(p+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
			*(p+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
			*(p+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
			*(p+3) = 0;
			y++;
			if (p+4 < pStrideEnd)
			{
				*(p+5) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
				*(p+4) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
				*(p+6) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
				*(p+7) = 0;
			}
			y++; u++; v++;
		}
	}
}

void cpp_ConvertBottomupRGB24ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD dwDataStride)
{
	BYTE *y = pYBegin;
	BYTE *u = pUBegin;
	BYTE *v = pVBegin;

	_ASSERT(dwStride == ROUNDUP(dwDataStride, 4));

	for (const BYTE *pStrideBegin = pSrcEnd - dwStride; pStrideBegin >= pSrcBegin; pStrideBegin -= dwStride)
	{
		const BYTE *pStrideEnd = pStrideBegin + dwDataStride;
		for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 6)
		{
			const BYTE *q;
			if (p+3 < pStrideEnd)
				q = p+3;
			else
				q = p;
			*(y+0) = min(max(int((*(p+0))*0.098 + (*(p+1))*0.504 + (*(p+2))*0.257 + 16.5), 16), 235);
			*(y+1) = min(max(int((*(q+0))*0.098 + (*(q+1))*0.504 + (*(q+2))*0.257 + 16.5), 16), 235);
			*u     = min(max(int(((*(p+0)+*(q+0))*0.439 + (*(p+1)+*(q+1))*-0.291 + (*(p+2)+*(q+2))*-0.148)/2 + 128.5), 16), 240);
			*v     = min(max(int(((*(p+0)+*(q+0))*-0.071 + (*(p+1)+*(q+1))*-0.368 + (*(p+2)+*(q+2))*0.439)/2 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
	}
}

void cpp_ConvertBottomupRGB32ToULY2(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD dwDataStride)
{
	BYTE *y = pYBegin;
	BYTE *u = pUBegin;
	BYTE *v = pVBegin;

	_ASSERT(dwStride == ROUNDUP(dwDataStride, 4));

	for (const BYTE *pStrideBegin = pSrcEnd - dwStride; pStrideBegin >= pSrcBegin; pStrideBegin -= dwStride)
	{
		const BYTE *pStrideEnd = pStrideBegin + dwDataStride;
		for (const BYTE *p = pStrideBegin; p < pStrideEnd; p += 8)
		{
			const BYTE *q;
			if (p+4 < pStrideEnd)
				q = p+4;
			else
				q = p;
			*(y+0) = min(max(int((*(p+0))*0.098 + (*(p+1))*0.504 + (*(p+2))*0.257 + 16.5), 16), 235);
			*(y+1) = min(max(int((*(q+0))*0.098 + (*(q+1))*0.504 + (*(q+2))*0.257 + 16.5), 16), 235);
			*u     = min(max(int(((*(p+0)+*(q+0))*0.439 + (*(p+1)+*(q+1))*-0.291 + (*(p+2)+*(q+2))*-0.148)/2 + 128.5), 16), 240);
			*v     = min(max(int(((*(p+0)+*(q+0))*-0.071 + (*(p+1)+*(q+1))*-0.368 + (*(p+2)+*(q+2))*0.439)/2 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
	}
}
