/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UMYUV422Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

template<>
const utvf_t CUMYUV422Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV422Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV422Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
	UTVF_HDYC,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<>
const utvf_t CUMYUV422Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_HDYC,
	UTVF_YUY2, UTVF_YUYV, UTVF_YUNV, UTVF_yuvs,
	UTVF_UYVY, UTVF_UYNV, UTVF_2vuy,
	UTVF_YV16,
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

template<class C>
const utvf_t CUMYUV422Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CUMYUV422Codec<CBT601Coefficient>::CUMYUV422Codec(const char *pszInterfaceName) : CUM00Codec("UMY2", pszInterfaceName)
{
}

template<>
CUMYUV422Codec<CBT709Coefficient>::CUMYUV422Codec(const char *pszInterfaceName) : CUM00Codec("UMH2", pszInterfaceName)
{
}

template<>
const char *CUMYUV422Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.601";
}

template<>
const char *CUMYUV422Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.709";
}

template<class C>
void CUMYUV422Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	auto cbYWidth = ROUNDUP(width, 64);
	auto cbCWidth = ROUNDUP(width / 2, 64);

	m_cbPlaneSize[0]          = cbYWidth * height;
	m_cbPlaneSize[1]          = cbCWidth * height;
	m_cbPlaneSize[2]          = cbCWidth * height;

	m_cbPlaneWidth[0]         = cbYWidth;
	m_cbPlaneWidth[1]         = cbCWidth;
	m_cbPlaneWidth[2]         = cbCWidth;

	m_cbPlaneStripeSize[0]    = cbYWidth;
	m_cbPlaneStripeSize[1]    = cbCWidth;
	m_cbPlaneStripeSize[2]    = cbCWidth;

	m_cbPlanePredictStride[0] = cbYWidth;
	m_cbPlanePredictStride[1] = cbCWidth;
	m_cbPlanePredictStride[2] = cbCWidth;
}

template<class C>
void CUMYUV422Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YV16:
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

	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertYUYVToULY2(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertUYVYToULY2(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	}
}

template<class C>
void CUMYUV422Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YV16:
		for (int i = 0; i < 3; ++i)
		{
			auto q = pPlaneBegin[i];
			auto cbLineWidth = m_fmRaw.cbLineWidth[i];
			for (auto p = pRawBegin[i]; p != pRawEnd[i]; p += m_fmRaw.scbLineStride[i], q += m_cbPlaneWidth[i])
				memcpy(p, q, cbLineWidth);
		}
		return;

	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertULY2ToYUYV(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertULY2ToUYVY(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertULY2ToBGR(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertULY2ToBGRX(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY2ToRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY2ToXRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	}
}

template<class C>
bool CUMYUV422Codec<C>::EncodeDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, _] = CalcBandPosition<true>(nBandIndex);

	size_t cbYPlane = (m_dwStripeEnd[nBandIndex] - m_dwStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[0];
	size_t cbCPlane = (m_dwStripeEnd[nBandIndex] - m_dwStripeBegin[nBandIndex]) * m_cbPlaneStripeSize[1];

	if (m_nKeyFrameInterval <= 1)
	{
		m_cbControlStream[0][nBandIndex] = cbYPlane / 64 * 3;
		m_cbControlStream[1][nBandIndex] = cbCPlane / 64 * 3;
		m_cbControlStream[2][nBandIndex] = cbCPlane / 64 * 3;

		switch (m_utvfRaw)
		{
		case UTVF_YV16:
			if ((m_nWidth % 128) == 0)
			{
				auto [pRawPlaneBegin] = CalcPlanePosition<true>();

				EncodeFromPlanar(nBandIndex, pRawPlaneBegin);

				return true;
			}
			break;

		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertYUYVToULY2_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertUYVYToULY2_Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[0][nBandIndex], &m_cbPackedStream[0][nBandIndex],
				m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], &m_cbPackedStream[1][nBandIndex],
				m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], &m_cbPackedStream[2][nBandIndex],
				m_pControlStream[2][nBandIndex],
				pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0]);
			return true;
		}
	}

	return false;
}

template<class C>
bool CUMYUV422Codec<C>::DecodeDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, _] = CalcBandPosition<false>(nBandIndex);

	if (!(m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION))
	{
		switch (m_utvfRaw)
		{
		case UTVF_YV16:
			if ((m_nWidth % 128) == 0)
			{
				auto [pRawPlaneBegin] = CalcPlanePosition<false>();

				DecodeToPlanar(nBandIndex, pRawPlaneBegin);

				return true;
			}
			break;

		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertULY2ToYUYV_Unpack8SymAndRestorePredictPlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertULY2ToUYVY_Unpack8SymAndRestorePredictPlanarGradient8(
				pRawBegin[0], pRawEnd[0],
				m_pPackedStream[0][nBandIndex], m_pControlStream[0][nBandIndex],
				m_pPackedStream[1][nBandIndex], m_pControlStream[1][nBandIndex],
				m_pPackedStream[2][nBandIndex], m_pControlStream[2][nBandIndex],
				m_fmRaw.cbLineWidth[0], m_fmRaw.scbStripeStride[0]);
			return true;
		}
	}

	return false;
}
