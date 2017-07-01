/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Convert.h"
#include "ColorOrder.h"
#include "Coefficient.h"
#include "ByteOrder.h"

template<class C, class T>
void cpp_ConvertULY2ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP * 2)
		{
			uint8_t *q = p + T::BYPP;
			*(p+T::G) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
			*(p+T::B) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
			*(p+T::R) = min(max(int((*y-16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
			if (T::BYPP == 4)
				*(p+T::A) = 0xff;
			y++;
			*(q+T::G) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
			*(q+T::B) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
			*(q+T::R) = min(max(int((*y-16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
			if (T::BYPP == 4)
				*(q+T::A) = 0xff;
			y++; u++; v++;
		}
	}
}

template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);

//

template<class C, class T>
void cpp_ConvertRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP * 2)
		{
			const uint8_t *q = p + T::BYPP;
			*(y+0) = min(max(int((*(p+T::B))*C::B2Y + (*(p+T::G))*C::G2Y + (*(p+T::R))*C::R2Y + 16.5), 16), 235);
			*(y+1) = min(max(int((*(q+T::B))*C::B2Y + (*(q+T::G))*C::G2Y + (*(q+T::R))*C::R2Y + 16.5), 16), 235);
			*u     = min(max(int(((*(p+T::B)+*(q+T::B))*C::B2U + (*(p+T::G)+*(q+T::G))*C::G2U + (*(p+T::R)+*(q+T::R))*C::R2U)/2 + 128.5), 16), 240);
			*v     = min(max(int(((*(p+T::B)+*(q+T::B))*C::B2V + (*(p+T::G)+*(q+T::G))*C::G2V + (*(p+T::R)+*(q+T::R))*C::R2V)/2 + 128.5), 16), 240);
			y+=2; u++; v++;
		}
	}
}

template void cpp_ConvertRGBToULY2<CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY2<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY2<CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY2<CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

//

template<class C, class T>
void cpp_ConvertULY4ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *y = pSrcYBegin;
	const uint8_t *u = pSrcUBegin;
	const uint8_t *v = pSrcVBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*(p + T::G) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2G + (*v - 128)*C::V2G), 0), 255);
			*(p + T::B) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2B                    ), 0), 255);
			*(p + T::R) = min(max(int((*y - 16)*C::Y2RGB                     + (*v - 128)*C::V2R), 0), 255);
			if (T::BYPP == 4)
				*(p + T::A) = 0xff;
			y++; u++; v++;
		}
	}
}

template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);

//

template<class C, class T>
void cpp_ConvertRGBToULY4(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *y = pDstYBegin;
	uint8_t *u = pDstUBegin;
	uint8_t *v = pDstVBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*y = min(max(int((*(p + T::B))*C::B2Y + (*(p + T::G))*C::G2Y + (*(p + T::R))*C::R2Y + 16.5), 16), 235);
			*u = min(max(int((*(p + T::B))*C::B2U + (*(p + T::G))*C::G2U + (*(p + T::R))*C::R2U + 128.5), 16), 240);
			*v = min(max(int((*(p + T::B))*C::B2V + (*(p + T::G))*C::G2V + (*(p + T::R))*C::R2V + 128.5), 16), 240);
			y++; u++; v++;
		}
	}
}

template void cpp_ConvertRGBToULY4<CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY4<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY4<CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY4<CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

//

template<class C, class T>
void cpp_ConvertULY0ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	const uint8_t *y = pSrcYBegin;
	const uint8_t *u = pSrcUBegin;
	const uint8_t *v = pSrcVBegin;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	for (uint8_t *pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += scbRawStripeSize)
	{
		for (uint8_t *pStrideBegin = pStripeBegin; pStrideBegin != pStripeBegin + scbPredictStride; pStrideBegin += scbStride)
		{
			uint8_t *pStrideEnd = pStrideBegin + cbWidth;
			for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP * 2)
			{
				uint8_t *q = p + scbPredictStride;
				*(p + T::G) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2G + (*v - 128)*C::V2G), 0), 255);
				*(p + T::B) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2B), 0), 255);
				*(p + T::R) = min(max(int((*y - 16)*C::Y2RGB + (*v - 128)*C::V2R), 0), 255);
				*(q + T::G) = min(max(int((*(y + dwYPlaneGrossWidth) - 16)*C::Y2RGB + (*u - 128)*C::U2G + (*v - 128)*C::V2G), 0), 255);
				*(q + T::B) = min(max(int((*(y + dwYPlaneGrossWidth) - 16)*C::Y2RGB + (*u - 128)*C::U2B), 0), 255);
				*(q + T::R) = min(max(int((*(y + dwYPlaneGrossWidth) - 16)*C::Y2RGB + (*v - 128)*C::V2R), 0), 255);
				if (T::BYPP == 4)
				{
					*(p + T::A) = 255;
					*(q + T::A) = 255;
				}
				y++;
				*(p + T::BYPP + T::G) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2G + (*v - 128)*C::V2G), 0), 255);
				*(p + T::BYPP + T::B) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2B), 0), 255);
				*(p + T::BYPP + T::R) = min(max(int((*y - 16)*C::Y2RGB + (*v - 128)*C::V2R), 0), 255);
				*(q + T::BYPP + T::G) = min(max(int((*(y + dwYPlaneGrossWidth) - 16)*C::Y2RGB + (*u - 128)*C::U2G + (*v - 128)*C::V2G), 0), 255);
				*(q + T::BYPP + T::B) = min(max(int((*(y + dwYPlaneGrossWidth) - 16)*C::Y2RGB + (*u - 128)*C::U2B), 0), 255);
				*(q + T::BYPP + T::R) = min(max(int((*(y + dwYPlaneGrossWidth) - 16)*C::Y2RGB + (*v - 128)*C::V2R), 0), 255);
				if (T::BYPP == 4)
				{
					*(p + T::BYPP + T::A) = 255;
					*(q + T::BYPP + T::A) = 255;
				}
				y++; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

template void cpp_ConvertULY0ToRGB<CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertULY0ToRGB<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertULY0ToRGB<CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertULY0ToRGB<CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertULY0ToRGB<CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertULY0ToRGB<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertULY0ToRGB<CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertULY0ToRGB<CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);

//

template<class C, class T>
void cpp_ConvertRGBToULY0(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
{
	uint8_t *y = pDstYBegin;
	uint8_t *u = pDstUBegin;
	uint8_t *v = pDstVBegin;

	ssize_t scbPredictStride = scbStride * (bInterlace ? 2 : 1);
	ssize_t scbRawStripeSize = scbStride * 2 * (bInterlace ? 2 : 1);

	for (const uint8_t *pStripeBegin = pSrcBegin; pStripeBegin != pSrcEnd; pStripeBegin += scbRawStripeSize)
	{
		for (const uint8_t *pStrideBegin = pStripeBegin; pStrideBegin != pStripeBegin + scbPredictStride; pStrideBegin += scbStride)
		{
			const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
			for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP * 2)
			{
				const uint8_t *q = p + scbPredictStride;
				*(y + 0) = min(max(int((*(p + T::B))*C::B2Y + (*(p + T::G))*C::G2Y + (*(p + T::R))*C::R2Y + 16.5), 16), 235);
				*(y + 1) = min(max(int((*(p + T::BYPP + T::B))*C::B2Y + (*(p + T::BYPP + T::G))*C::G2Y + (*(p + T::BYPP + T::R))*C::R2Y + 16.5), 16), 235);
				*(y + dwYPlaneGrossWidth + 0) = min(max(int((*(q + T::B))*C::B2Y + (*(q + T::G))*C::G2Y + (*(q + T::R))*C::R2Y + 16.5), 16), 235);
				*(y + dwYPlaneGrossWidth + 1) = min(max(int((*(q + T::BYPP + T::B))*C::B2Y + (*(q + T::BYPP + T::G))*C::G2Y + (*(q + T::BYPP + T::R))*C::R2Y + 16.5), 16), 235);
				*u = min(max(int(((*(p + T::B) + *(p + T::BYPP + T::B) + *(q + T::B) + *(q + T::BYPP + T::B))*C::B2U + (*(p + T::G) + *(p + T::BYPP + T::G) + *(q + T::G) + *(q + T::BYPP + T::G))*C::G2U + (*(p + T::R) + *(p + T::BYPP + T::R) + *(q + T::R) + *(q + T::BYPP + T::R))*C::R2U) / 4 + 128.5), 16), 240);
				*v = min(max(int(((*(p + T::B) + *(p + T::BYPP + T::B) + *(q + T::B) + *(q + T::BYPP + T::B))*C::B2V + (*(p + T::G) + *(p + T::BYPP + T::G) + *(q + T::G) + *(q + T::BYPP + T::G))*C::G2V + (*(p + T::R) + *(p + T::BYPP + T::R) + *(q + T::R) + *(q + T::BYPP + T::R))*C::R2V) / 4 + 128.5), 16), 240);
				y += 2; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

template void cpp_ConvertRGBToULY0<CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertRGBToULY0<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertRGBToULY0<CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertRGBToULY0<CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertRGBToULY0<CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertRGBToULY0<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertRGBToULY0<CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template void cpp_ConvertRGBToULY0<CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);

//

template<class T>
void cpp_ConvertRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pStrideBegin, *p;
	uint8_t *g = pGBegin;
	uint8_t *b = pBBegin;
	uint8_t *r = pRBegin;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*g++ = *(p+T::G);
			*b++ = *(p+T::B) - *(p+T::G) + 0x80;
			*r++ = *(p+T::R) - *(p+T::G) + 0x80;
		}
	}
}

template void cpp_ConvertRGBToULRG<CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULRG<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToULRG<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

//

template<class T>
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
			*g++ = *(p+T::G);
			*b++ = *(p+T::B) - *(p+T::G) + 0x80;
			*r++ = *(p+T::R) - *(p+T::G) + 0x80;
			*a++ = *(p+T::A);
		}
	}
}

template void cpp_ConvertARGBToULRA<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertARGBToULRA<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

//

template<class T>
void cpp_ConvertYUV422ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	const uint8_t *pStrideBegin, *p;
	uint8_t *y = pYBegin;
	uint8_t *u = pUBegin;
	uint8_t *v = pVBegin;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*y++ = *(p+T::Y0);
			*y++ = *(p+T::Y1);
			*u++ = *(p+T::U);
			*v++ = *(p+T::V);
		}
	}
}

template void cpp_ConvertYUV422ToULY2<class CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertYUV422ToULY2<class CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

//

template<class T>
void cpp_ConvertULRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *pStrideBegin, *p;
	const uint8_t *g = pGBegin;
	const uint8_t *b = pBBegin;
	const uint8_t *r = pRBegin;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*(p+T::G) = *g;
			*(p+T::B) = *b + *g - 0x80;
			*(p+T::R) = *r + *g - 0x80;
			if (T::BYPP == 4)
				*(p+T::A) = 0xff;
			g++; b++; r++;
		}
	}
}

template void cpp_ConvertULRGToRGB<CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRGToRGB<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);

//

template<class T>
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
			*(p+T::G) = *g;
			*(p+T::B) = *b + *g - 0x80;
			*(p+T::R) = *r + *g - 0x80;
			*(p+T::A) = *a;
			g++; b++; r++; a++;
		}
	}
}

template void cpp_ConvertULRAToARGB<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULRAToARGB<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);

//

template<class T>
void cpp_ConvertULY2ToYUV422(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t *pStrideBegin, *p;
	const uint8_t *y = pYBegin;
	const uint8_t *u = pUBegin;
	const uint8_t *v = pVBegin;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*(p+T::Y0) = *y++;
			*(p+T::Y1) = *y++;
			*(p+T::U)  = *u++;
			*(p+T::V)  = *v++;
		}
	}
}

template void cpp_ConvertULY2ToYUV422<CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertULY2ToYUV422<CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);

//

template<class T>
void cpp_ConvertRGBToUQRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			const uint16_t *pp = (const uint16_t *)p;
			uint16_t gg = Convert16To10Fullrange(btoh16(pp[T::G]));
			uint16_t bb = Convert16To10Fullrange(btoh16(pp[T::B]));
			uint16_t rr = Convert16To10Fullrange(btoh16(pp[T::R]));

			*g++ = gg;
			*b++ = (bb - gg + 0x200) & 0x3ff;
			*r++ = (rr - gg + 0x200) & 0x3ff;
		}
	}
}

template void cpp_ConvertRGBToUQRG<CB48rColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertRGBToUQRG<CB64aColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);

//

template<class T> void cpp_ConvertUQRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	const uint16_t *g = (const uint16_t *)pGBegin;
	const uint16_t *b = (const uint16_t *)pBBegin;
	const uint16_t *r = (const uint16_t *)pRBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			uint16_t *pp = (uint16_t *)p;

			pp[T::G] = htob16(Convert10To16Fullrange(*g));
			pp[T::B] = htob16(Convert10To16Fullrange(*b + *g - 0x200));
			pp[T::R] = htob16(Convert10To16Fullrange(*r + *g - 0x200));
			g++; b++; r++;
		}
	}

}

template void cpp_ConvertUQRGToRGB<CB48rColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void cpp_ConvertUQRGToRGB<CB64aColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);

//

void cpp_ConvertB64aToUQRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;
	uint16_t *a = (uint16_t *)pABegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += 8)
		{
			const uint16_t *pp = (const uint16_t *)p;
			uint16_t gg = Convert16To10Fullrange(btoh16(pp[2]));
			uint16_t bb = Convert16To10Fullrange(btoh16(pp[3]));
			uint16_t rr = Convert16To10Fullrange(btoh16(pp[1]));
			uint16_t aa = Convert16To10Fullrange(btoh16(pp[0]));

			*g++ = gg;
			*b++ = (bb - gg + 0x200) & 0x3ff;
			*r++ = (rr - gg + 0x200) & 0x3ff;
			*a++ = aa;
		}
	}
}

void cpp_ConvertUQRAToB64a(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	const uint16_t *g = (const uint16_t *)pGBegin;
	const uint16_t *b = (const uint16_t *)pBBegin;
	const uint16_t *r = (const uint16_t *)pRBegin;
	const uint16_t *a = (const uint16_t *)pABegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += 8)
		{
			uint16_t *pp = (uint16_t *)p;

			pp[2] = htob16(Convert10To16Fullrange(*g));
			pp[3] = htob16(Convert10To16Fullrange(*b + *g - 0x200));
			pp[1] = htob16(Convert10To16Fullrange(*r + *g - 0x200));
			pp[0] = htob16(Convert10To16Fullrange(*a));
			g++; b++; r++; a++;
		}
	}
}
