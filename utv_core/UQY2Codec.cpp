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
	m_syInitialPredict[0] = 0x200;
	m_syInitialPredict[1] = 0x200;
	m_syInitialPredict[2] = 0x200;
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
		for (const uint8_t *pStripeBegin = pSrcBegin; pStripeBegin != pSrcEnd; pStripeBegin += m_cbRawStripeSize)
		{
			const uint8_t *pStripeEnd = pStripeBegin + m_cbRawStripeSize;
			for (const uint8_t *p = pStripeBegin; p != pStripeEnd; p+= 128)
			{
				for (const uint8_t *q = p; q < p + 128; q += 16)
				{
					const uint32_t *qq = (const uint32_t *)q;
					u[0] = (ltoh32(qq[0])      ) & 0x3ff;
					y[0] = (ltoh32(qq[0]) >> 10) & 0x3ff;
					v[0] = (ltoh32(qq[0]) >> 20) & 0x3ff;
					y[1] = (ltoh32(qq[1])      ) & 0x3ff;
					u[1] = (ltoh32(qq[1]) >> 10) & 0x3ff;
					y[2] = (ltoh32(qq[1]) >> 20) & 0x3ff;
					v[1] = (ltoh32(qq[2])      ) & 0x3ff;
					y[3] = (ltoh32(qq[2]) >> 10) & 0x3ff;
					u[2] = (ltoh32(qq[2]) >> 20) & 0x3ff;
					y[4] = (ltoh32(qq[3])      ) & 0x3ff;
					v[2] = (ltoh32(qq[3]) >> 10) & 0x3ff;
					y[5] = (ltoh32(qq[3]) >> 20) & 0x3ff;
					y += 6;
					u += 3;
					v += 3;
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
		for (uint8_t *pStripeBegin = pDstBegin; pStripeBegin != pDstEnd; pStripeBegin += m_cbRawStripeSize)
		{
			uint8_t *pStripeEnd = pStripeBegin + m_cbRawStripeSize;
			for (uint8_t *p = pStripeBegin; p != pStripeEnd; p+= 128)
			{
				for (uint8_t *q = p; q < p + 128; q += 16)
				{
					uint32_t *qq = (uint32_t *)q;
					qq[0] = htol32((v[0] << 20) | (y[0] << 10) | u[0]);
					qq[1] = htol32((y[2] << 20) | (u[1] << 10) | y[1]);
					qq[2] = htol32((u[2] << 20) | (y[3] << 10) | v[1]);
					qq[3] = htol32((y[5] << 20) | (v[2] << 10) | y[4]);
					y += 6;
					u += 3;
					v += 3;
				}
			}
		}
		break;
	}
}

bool CUQY2Codec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
