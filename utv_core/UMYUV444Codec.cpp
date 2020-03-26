/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UMYUV444Codec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "ColorOrder.h"

template<>
const utvf_t CUMYUV444Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV444Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV444Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV444Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YV24,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<class C>
const utvf_t CUMYUV444Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CUMYUV444Codec<CBT601Coefficient>::CUMYUV444Codec(const char *pszInterfaceName) : CUM00Codec("UMY4", pszInterfaceName)
{
}

template<>
CUMYUV444Codec<CBT709Coefficient>::CUMYUV444Codec(const char *pszInterfaceName) : CUM00Codec("UMH4", pszInterfaceName)
{
}

template<>
const char *CUMYUV444Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV444 BT.601";
}

template<>
const char *CUMYUV444Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV444 BT.709";
}

template<class C>
void CUMYUV444Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	width = ROUNDUP(width, 64);

	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
}

template<class C>
void CUMYUV444Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YV24:
		for (int i = 0; i < 3; ++i)
		{
			auto q = pPlaneBegin[i];
			auto cbLineWidth = m_fmRaw.cbLineWidth[i];
			for (auto p = pRawBegin[i]; p != pRawEnd[i]; p += m_fmRaw.scbStripeStride[i], q += m_cbPlaneWidth[i])
			{
				memcpy(q, p, cbLineWidth);
				std::fill(q + cbLineWidth, q + m_cbPlaneWidth[i], q[cbLineWidth - 1]);
			}
		}
		return;

	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULY4(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULY4(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY4(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULY4(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	}
}

template<class C>
void CUMYUV444Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YV24:
		for (int i = 0; i < 3; ++i)
		{
			auto q = pPlaneBegin[i];
			auto cbLineWidth = m_fmRaw.cbLineWidth[i];
			for (auto p = pRawBegin[i]; p != pRawEnd[i]; p += m_fmRaw.scbLineStride[i], q += m_cbPlaneWidth[i])
				memcpy(p, q, cbLineWidth);
		}
		return;

	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertULY4ToBGR(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertULY4ToBGRX(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY4ToRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY4ToXRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0]);
		break;
	}
}

template<class C>
bool CUMYUV444Codec<C>::EncodeDirect(uint32_t nBandIndex)
{
	if (m_nKeyFrameInterval <= 1)
	{
		if (m_utvfRaw == UTVF_YV24 && (m_nWidth % 64) == 0)
		{
			const uint8_t *pSrcBegin[3];

			pSrcBegin[0] = ((const uint8_t *)m_pInput);
			pSrcBegin[2] = pSrcBegin[0] + m_nWidth * m_nHeight;
			pSrcBegin[1] = pSrcBegin[2] + m_nWidth * m_nHeight;

			EncodeFromPlanar(nBandIndex, pSrcBegin);

			return true;
		}
	}

	return false;
}

template<class C>
bool CUMYUV444Codec<C>::DecodeDirect(uint32_t nBandIndex)
{
	if (!(m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION))
	{
		if (m_utvfRaw == UTVF_YV24 && (m_nWidth % 64) == 0)
		{
			uint8_t *pDstBegin[3];

			pDstBegin[0] = ((uint8_t *)m_pOutput);
			pDstBegin[2] = pDstBegin[0] + m_nWidth * m_nHeight;
			pDstBegin[1] = pDstBegin[2] + m_nWidth * m_nHeight;

			DecodeToPlanar(nBandIndex, pDstBegin);

			return true;
		}
	}

	return false;
}
