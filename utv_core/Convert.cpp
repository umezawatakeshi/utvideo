/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Convert.h"
#include "ColorOrder.h"
#include "Coefficient.h"
#include "ByteOrder.h"

template<class C, class T>
void cpp_ConvertULY2ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		const uint8_t *y = pYBegin;
		const uint8_t *u = pUBegin;
		const uint8_t *v = pVBegin;

		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP * 2)
		{
			uint8_t *q = p + T::BYPP;
			*(p+T::G) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
			*(p+T::B) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
			*(p+T::R) = min(max(int((*y-16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
			if (T::HAS_ALPHA)
				*(p+T::A) = 0xff;
			y++;
			*(q+T::G) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2G + (*v-128)*C::V2G), 0), 255);
			*(q+T::B) = min(max(int((*y-16)*C::Y2RGB + (*u-128)*C::U2B                  ), 0), 255);
			*(q+T::R) = min(max(int((*y-16)*C::Y2RGB                   + (*v-128)*C::V2R), 0), 255);
			if (T::HAS_ALPHA)
				*(q+T::A) = 0xff;
			y++; u++; v++;
		}
	}
}

template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToRGB<CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToRGB<CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);

//

template<class C, class T>
void cpp_ConvertRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		uint8_t *y = pYBegin;
		uint8_t *u = pUBegin;
		uint8_t *v = pVBegin;

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

		std::fill(y, pYBegin + cbYWidth, y[-1]);
		std::fill(u, pUBegin + cbCWidth, u[-1]);
		std::fill(v, pVBegin + cbCWidth, v[-1]);
	}
}

template void cpp_ConvertRGBToULY2<CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertRGBToULY2<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertRGBToULY2<CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertRGBToULY2<CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertRGBToULY2<CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);

//

template<class C, class T>
void cpp_ConvertULY4ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride, pSrcYBegin += cbPlaneWidth, pSrcUBegin += cbPlaneWidth, pSrcVBegin += cbPlaneWidth)
	{
		const uint8_t *y = pSrcYBegin;
		const uint8_t *u = pSrcUBegin;
		const uint8_t *v = pSrcVBegin;

		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*(p + T::G) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2G + (*v - 128)*C::V2G), 0), 255);
			*(p + T::B) = min(max(int((*y - 16)*C::Y2RGB + (*u - 128)*C::U2B                    ), 0), 255);
			*(p + T::R) = min(max(int((*y - 16)*C::Y2RGB                     + (*v - 128)*C::V2R), 0), 255);
			if (T::HAS_ALPHA)
				*(p + T::A) = 0xff;
			y++; u++; v++;
		}
	}
}

template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULY4ToRGB<CBT601Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CRGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULY4ToRGB<CBT709Coefficient, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);

//

template<class C, class T>
void cpp_ConvertRGBToULY4(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride, pDstYBegin += cbPlaneWidth, pDstUBegin += cbPlaneWidth, pDstVBegin += cbPlaneWidth)
	{
		uint8_t *y = pDstYBegin;
		uint8_t *u = pDstUBegin;
		uint8_t *v = pDstVBegin;

		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*y = min(max(int((*(p + T::B))*C::B2Y + (*(p + T::G))*C::G2Y + (*(p + T::R))*C::R2Y + 16.5), 16), 235);
			*u = min(max(int((*(p + T::B))*C::B2U + (*(p + T::G))*C::G2U + (*(p + T::R))*C::R2U + 128.5), 16), 240);
			*v = min(max(int((*(p + T::B))*C::B2V + (*(p + T::G))*C::G2V + (*(p + T::R))*C::R2V + 128.5), 16), 240);
			y++; u++; v++;
		}

		std::fill(y, pDstYBegin + cbPlaneWidth, y[-1]);
		std::fill(u, pDstUBegin + cbPlaneWidth, u[-1]);
		std::fill(v, pDstVBegin + cbPlaneWidth, v[-1]);
	}
}

template void cpp_ConvertRGBToULY4<CBT601Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULY4<CBT601Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULY4<CBT601Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULY4<CBT601Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CBGRColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CBGRAColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CRGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULY4<CBT709Coefficient, CARGBColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);

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
				if (T::HAS_ALPHA)
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
				if (T::HAS_ALPHA)
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
void cpp_ConvertRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	const uint8_t *pStrideBegin, *p;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth)
	{
		uint8_t *g = pGBegin;
		uint8_t *b = pBBegin;
		uint8_t *r = pRBegin;

		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*g++ = *(p+T::G);
			*b++ = *(p+T::B) - *(p+T::G) + 0x80;
			*r++ = *(p+T::R) - *(p+T::G) + 0x80;
		}

		std::fill(g, pGBegin + cbPlaneWidth, g[-1]);
		std::fill(b, pBBegin + cbPlaneWidth, b[-1]);
		std::fill(r, pRBegin + cbPlaneWidth, r[-1]);
	}
}

template void cpp_ConvertRGBToULRG<CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULRG<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertRGBToULRG<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);

//

template<class T>
void cpp_ConvertARGBToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	const uint8_t *pStrideBegin, *p;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth, pABegin += cbPlaneWidth)
	{
		uint8_t *g = pGBegin;
		uint8_t *b = pBBegin;
		uint8_t *r = pRBegin;
		uint8_t *a = pABegin;

		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*g++ = *(p+T::G);
			*b++ = *(p+T::B) - *(p+T::G) + 0x80;
			*r++ = *(p+T::R) - *(p+T::G) + 0x80;
			*a++ = *(p+T::A);
		}

		std::fill(g, pGBegin + cbPlaneWidth, g[-1]);
		std::fill(b, pBBegin + cbPlaneWidth, b[-1]);
		std::fill(r, pRBegin + cbPlaneWidth, r[-1]);
		std::fill(a, pABegin + cbPlaneWidth, a[-1]);
	}
}

template void cpp_ConvertARGBToULRA<CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertARGBToULRA<CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);

//

template<class T>
void cpp_ConvertYUV422ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	const uint8_t *pStrideBegin, *p;

	for (pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		uint8_t *y = pYBegin;
		uint8_t *u = pUBegin;
		uint8_t *v = pVBegin;

		const uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			*y++ = *(p+T::Y0);
			*y++ = *(p+T::Y1);
			*u++ = *(p+T::U);
			*v++ = *(p+T::V);
		}

		std::fill(y, pYBegin + cbYWidth, y[-1]);
		std::fill(u, pUBegin + cbCWidth, u[-1]);
		std::fill(v, pVBegin + cbCWidth, v[-1]);
	}
}

template void cpp_ConvertYUV422ToULY2<class CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertYUV422ToULY2<class CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);

//

template<class T>
void cpp_ConvertULRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	uint8_t *pStrideBegin, *p;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth)
	{
		const uint8_t *g = pGBegin;
		const uint8_t *b = pBBegin;
		const uint8_t *r = pRBegin;

		uint8_t *pStrideEnd = pStrideBegin + cbWidth;
		for (p = pStrideBegin; p < pStrideEnd; p += T::BYPP)
		{
			*(p+T::G) = *g;
			*(p+T::B) = *b + *g - 0x80;
			*(p+T::R) = *r + *g - 0x80;
			if (T::HAS_ALPHA)
				*(p+T::A) = 0xff;
			g++; b++; r++;
		}
	}
}

template void cpp_ConvertULRGToRGB<CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULRGToRGB<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULRGToRGB<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);

//

template<class T>
void cpp_ConvertULRAToARGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth)
{
	uint8_t *pStrideBegin, *p;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride, pGBegin += cbPlaneWidth, pBBegin += cbPlaneWidth, pRBegin += cbPlaneWidth, pABegin += cbPlaneWidth)
	{
		const uint8_t *g = pGBegin;
		const uint8_t *b = pBBegin;
		const uint8_t *r = pRBegin;
		const uint8_t *a = pABegin;

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

template void cpp_ConvertULRAToARGB<CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template void cpp_ConvertULRAToARGB<CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);

//

template<class T>
void cpp_ConvertULY2ToYUV422(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth)
{
	uint8_t *pStrideBegin, *p;

	for (pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride, pYBegin += cbYWidth, pUBegin += cbCWidth, pVBegin += cbCWidth)
	{
		const uint8_t *y = pYBegin;
		const uint8_t *u = pUBegin;
		const uint8_t *v = pVBegin;

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

template void cpp_ConvertULY2ToYUV422<CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template void cpp_ConvertULY2ToYUV422<CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);

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

template<class T>
void cpp_ConvertUQRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
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
			if (T::HAS_ALPHA)
				pp[T::A] = 0xffff;
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

//

void cpp_ConvertV210ToUQY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride)
{
	uint16_t *y = (uint16_t *)pYBegin;
	uint16_t *u = (uint16_t *)pUBegin;
	uint16_t *v = (uint16_t *)pVBegin;

	for (const uint8_t *pStripeBegin = pSrcBegin; pStripeBegin != pSrcEnd; pStripeBegin += scbStride)
	{
		const uint8_t *p = pStripeBegin;
		for (unsigned int x = 0; x < nWidth; x += 6, p += 16)
		{
			const uint32_t *pp = (const uint32_t *)p;

			*u++ = (ltoh32(pp[0])) & 0x3ff;
			*y++ = (ltoh32(pp[0]) >> 10) & 0x3ff;
			*v++ = (ltoh32(pp[0]) >> 20) & 0x3ff;
			*y++ = (ltoh32(pp[1])) & 0x3ff;

			if (x + 2 < nWidth)
			{
				*u++ = (ltoh32(pp[1]) >> 10) & 0x3ff;
				*y++ = (ltoh32(pp[1]) >> 20) & 0x3ff;
				*v++ = (ltoh32(pp[2])) & 0x3ff;
				*y++ = (ltoh32(pp[2]) >> 10) & 0x3ff;
			}

			if (x + 4 < nWidth)
			{
				*u++ = (ltoh32(pp[2]) >> 20) & 0x3ff;
				*y++ = (ltoh32(pp[3])) & 0x3ff;
				*v++ = (ltoh32(pp[3]) >> 10) & 0x3ff;
				*y++ = (ltoh32(pp[3]) >> 20) & 0x3ff;
			}
		}
	}
}

void cpp_ConvertUQY2ToV210(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, unsigned int nWidth, ssize_t scbStride)
{
	const uint16_t *y = (const uint16_t *)pYBegin;
	const uint16_t *u = (const uint16_t *)pUBegin;
	const uint16_t *v = (const uint16_t *)pVBegin;

	for (uint8_t *pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += scbStride)
	{
		uint8_t *p = pStripeBegin;
		unsigned int x = 0;

		for (; x <= nWidth - 6; x += 6, p += 16)
		{
			uint32_t *pp = (uint32_t *)p;

			pp[0] = htol32((v[0] << 20) | (y[0] << 10) | u[0]);
			pp[1] = htol32((y[2] << 20) | (u[1] << 10) | y[1]);
			pp[2] = htol32((u[2] << 20) | (y[3] << 10) | v[1]);
			pp[3] = htol32((y[5] << 20) | (v[2] << 10) | y[4]);
			y += 6;
			u += 3;
			v += 3;
		}

		if (x < nWidth)
		{
			uint32_t *pp = (uint32_t *)p;

			uint16_t y0, y1, y2, y3, u0, u1, v0, v1;

			u0 = *u++;
			y0 = *y++;
			v0 = *v++;
			y1 = *y++;

			if (x + 2 < nWidth)
			{
				u1 = *u++;
				y2 = *y++;
				v1 = *v++;
				y3 = *y++;
			}
			else
			{
				u1 = 0;
				y2 = 0;
				v1 = 0;
				y3 = 0;
			}

			pp[0] = htol32((v0 << 20) | (y0 << 10) | u0);
			pp[1] = htol32((y2 << 20) | (u1 << 10) | y1);
			pp[2] = htol32(             (y3 << 10) | v1);
			pp[3] = 0;

			p += 16;
		}

		memset(p, 0, pStripeBegin + ((nWidth + 47) / 48 * 128) - p);
	}
}

//

void cpp_ConvertR210ToUQRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride)
{
	uint16_t *g = (uint16_t *)pGBegin;
	uint16_t *b = (uint16_t *)pBBegin;
	uint16_t *r = (uint16_t *)pRBegin;

	for (const uint8_t *pStrideBegin = pSrcBegin; pStrideBegin != pSrcEnd; pStrideBegin += scbStride)
	{
		const uint8_t *pStrideEnd = pStrideBegin + nWidth * 4;
		for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += 4)
		{
			uint32_t val = btoh32(*(const uint32_t *)p);
			uint16_t gg = val >> 10;
			uint16_t bb = val - gg + 0x200;
			uint16_t rr = (val >> 20) - gg + 0x200;

			*g++ = gg & 0x3ff;
			*b++ = bb & 0x3ff;
			*r++ = rr & 0x3ff;
		}
	}
}

//

void cpp_ConvertUQRGToR210(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride)
{
	const uint16_t *g = (const uint16_t *)pGBegin;
	const uint16_t *b = (const uint16_t *)pBBegin;
	const uint16_t *r = (const uint16_t *)pRBegin;

	for (uint8_t *pStrideBegin = pDstBegin; pStrideBegin != pDstEnd; pStrideBegin += scbStride)
	{
		uint8_t *pStrideEnd = pStrideBegin + nWidth * 4;
		uint8_t *p = pStrideBegin;
		for (; p < pStrideEnd; p += 4)
		{
			uint32_t gg = *g;
			uint32_t bb = (*b + *g - 0x200) & 0x3ff;
			uint32_t rr = (*r + *g - 0x200) & 0x3ff;

			*(uint32_t *)p = htob32((rr << 20) | (gg << 10) | bb);
			g++;
			b++;
			r++;
		}
		memset(p, 0, pStrideBegin + ((nWidth + 63) / 64 * 256) - p);
	}
}

//

template<VALUERANGE VR>
void cpp_ConvertLittleEndian16ToHostEndian10(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd)
{
	auto q = (uint16_t*)pDst;
	for (auto p = (const uint16_t*)pSrcBegin; p < (const uint16_t*)pSrcEnd; ++p, ++q)
		*q = Convert16To10<VR>(ltoh16(*p));
}

template<VALUERANGE VR>
void cpp_ConvertHostEndian10ToLittleEndian16(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrc)
{
	auto p = (const uint16_t*)pSrc;
	for (auto q = (uint16_t*)pDstBegin; q < (uint16_t*)pDstEnd; ++p, ++q)
		*q = htol16(Convert10To16<VR>(*p));
}

template void cpp_ConvertLittleEndian16ToHostEndian10<VALUERANGE::LIMITED>(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void cpp_ConvertLittleEndian16ToHostEndian10<VALUERANGE::NOROUND>(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void cpp_ConvertHostEndian10ToLittleEndian16<VALUERANGE::LIMITED>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pSrc);

//

template<VALUERANGE VR>
void cpp_ConvertPackedUVLittleEndian16ToPlanarHostEndian10(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd)
{
	auto u = (uint16_t*)pUBegin;
	auto v = (uint16_t*)pVBegin;

	for (auto p = (const uint16_t*)pSrcBegin; p < (const uint16_t*)pSrcEnd; p += 2, ++u, ++v)
	{
		*u = Convert16To10<VR>(ltoh16(p[0]));
		*v = Convert16To10<VR>(ltoh16(p[1]));
	}
}

template<VALUERANGE VR>
void cpp_ConvertPlanarHostEndian10ToPackedUVLittleEndian16(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin)
{
	auto u = (const uint16_t*)pUBegin;
	auto v = (const uint16_t*)pVBegin;

	for (auto q = (uint16_t*)pDstBegin; q < (uint16_t*)pDstEnd; q += 2, ++u, ++v)
	{
		q[0] = htol16(Convert10To16<VR>(*u));
		q[1] = htol16(Convert10To16<VR>(*v));
	}
}

template void cpp_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<VALUERANGE::LIMITED>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void cpp_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<VALUERANGE::NOROUND>(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template void cpp_ConvertPlanarHostEndian10ToPackedUVLittleEndian16<VALUERANGE::LIMITED>(uint8_t* pDstBegin, uint8_t* pDstEnd, const uint8_t* pUBegin, const uint8_t* pVBegin);