/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQY2Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQY2Codec::m_utvfEncoderInput[] = {
	UTVF_v210,
	UTVF_INVALID,
};

const utvf_t CUQY2Codec::m_utvfDecoderOutput[] = {
	UTVF_v210,
	UTVF_INVALID,
};

const utvf_t CUQY2Codec::m_utvfCompressed[] = {
	UTVF_UQY2,
	UTVF_INVALID,
};

CUQY2Codec::CUQY2Codec(const char *pszInterfaceName) : CUQ00Codec("UQY2", pszInterfaceName)
{
}

void CUQY2Codec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height * 2;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;

	m_cbPlaneWidth[0]         = width * 2;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;

	m_cbPlaneStripeSize[0]    = width * 2;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;

	m_cbPlanePredictStride[0] = width * 2;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
}

void CUQY2Codec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint16_t *y, *u, *v;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	y = (uint16_t *)(m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0]);
	u = (uint16_t *)(m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1]);
	v = (uint16_t *)(m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2]);

	switch (m_utvfRaw)
	{
	case UTVF_v210:
		for (const uint8_t *pStripeBegin = pSrcBegin; pStripeBegin != pSrcEnd; pStripeBegin += m_cbRawGrossWidth)
		{
			const uint8_t *p = pStripeBegin;
			for (unsigned int x = 0; x < m_nWidth; x += 6, p += 16)
			{
				const uint32_t *pp = (const uint32_t *)p;

				*u++ = (ltoh32(pp[0])      ) & 0x3ff;
				*y++ = (ltoh32(pp[0]) >> 10) & 0x3ff;
				*v++ = (ltoh32(pp[0]) >> 20) & 0x3ff;
				*y++ = (ltoh32(pp[1])      ) & 0x3ff;

				if (x + 2 < m_nWidth)
				{
					*u++ = (ltoh32(pp[1]) >> 10) & 0x3ff;
					*y++ = (ltoh32(pp[1]) >> 20) & 0x3ff;
					*v++ = (ltoh32(pp[2])      ) & 0x3ff;
					*y++ = (ltoh32(pp[2]) >> 10) & 0x3ff;
				}

				if (x + 4 < m_nWidth)
				{
					*u++ = (ltoh32(pp[2]) >> 20) & 0x3ff;
					*y++ = (ltoh32(pp[3])      ) & 0x3ff;
					*v++ = (ltoh32(pp[3]) >> 10) & 0x3ff;
					*y++ = (ltoh32(pp[3]) >> 20) & 0x3ff;
				}
			}
		}
		break;
	}
}

void CUQY2Codec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint16_t *y, *u, *v;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	y = (uint16_t *)(m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0]);
	u = (uint16_t *)(m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1]);
	v = (uint16_t *)(m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2]);

	switch (m_utvfRaw)
	{
	case UTVF_v210:
		for (uint8_t *pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += m_cbRawGrossWidth)
		{
			uint8_t *p = pStripeBegin;
			for (unsigned int x = 0; x < m_nWidth; x += 6, p += 16)
			{
				uint32_t *pp = (uint32_t *)p;

				uint16_t y0, y1, y2, y3, y4, y5, u0, u1, u2, v0, v1, v2;

				u0 = *u++;
				y0 = *y++;
				v0 = *v++;
				y1 = *y++;

				if (x + 2 < m_nWidth)
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

				if (x + 4 < m_nWidth)
				{
					u2 = *u++;
					y4 = *y++;
					v2 = *v++;
					y5 = *y++;
				}
				else
				{
					u2 = 0;
					y4 = 0;
					v2 = 0;
					y5 = 0;
				}

				pp[0] = htol32((v0 << 20) | (y0 << 10) | u0);
				pp[1] = htol32((y2 << 20) | (u1 << 10) | y1);
				pp[2] = htol32((u2 << 20) | (y3 << 10) | v1);
				pp[3] = htol32((y5 << 20) | (v2 << 10) | y4);
			}
			memset(p, 0, pStripeBegin + m_cbRawNetWidth - p);
		}
		break;
	}
}

bool CUQY2Codec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
