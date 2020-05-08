/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULYUV422Codec.h"
#include "Predict.h"
#include "Convert.h"
#include "ConvertPredict.h"
#include "TunedFunc.h"

template<>
const utvf_t CULYUV422Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
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
const utvf_t CULYUV422Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
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
const utvf_t CULYUV422Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
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
const utvf_t CULYUV422Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
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
const utvf_t CULYUV422Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CULYUV422Codec<CBT601Coefficient>::CULYUV422Codec(const char *pszInterfaceName) : CUL00Codec("ULY2", pszInterfaceName)
{
}

template<>
CULYUV422Codec<CBT709Coefficient>::CULYUV422Codec(const char *pszInterfaceName) : CUL00Codec("ULH2", pszInterfaceName)
{
}

template<>
const char *CULYUV422Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.601";
}

template<>
const char *CULYUV422Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV422 BT.709";
}

template<class C>
void CULYUV422Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height / 2;
	m_cbPlaneSize[2]          = width * height / 2;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width / 2;
	m_cbPlaneWidth[2]         = width / 2;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width / 2;
	m_cbPlaneStripeSize[2]    = width / 2;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width / 2;
	m_cbPlanePredictStride[2] = width / 2;
}

template<class C>
void CULYUV422Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertYUYVToULY2(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertUYVYToULY2(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULY2(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	}
}

template<class C>
void CULYUV422Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertULY2ToYUYV(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertULY2ToUYVY(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertULY2ToBGR(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertULY2ToBGRX(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY2ToRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY2ToXRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlaneWidth[0], m_cbPlaneWidth[1]);
		break;
	}
}

template<class C>
bool CULYUV422Codec<C>::PredictDirect(uint32_t nBandIndex)
{
	if (m_utvfRaw == UTVF_YV16)
	{
		auto [pRawPlaneBegin] = CalcPlanePosition<true>();

		PredictFromPlanar(nBandIndex, pRawPlaneBegin);

		return true;
	}

	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertYUYVToULY2_PredictCylindricalLeftAndCount(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertUYVYToULY2_PredictCylindricalLeftAndCount(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertYUYVToULY2_PredictPlanarGradientAndCount(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertUYVYToULY2_PredictPlanarGradientAndCount(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertYUYVToULY2_PredictCylindricalWrongMedianAndCount(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertUYVYToULY2_PredictCylindricalWrongMedianAndCount(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
			return true;
		}
		break;
	}

	return false;
}

template<class C>
bool CULYUV422Codec<C>::DecodeDirect(uint32_t nBandIndex)
{
	if (m_utvfRaw == UTVF_YV16)
	{
		auto [pRawPlaneBegin] = CalcPlanePosition<false>();

		DecodeAndRestoreToPlanar(nBandIndex, pRawPlaneBegin);

		return true;
	}

	return false;
}

template<class C>
bool CULYUV422Codec<C>::RestoreDirect(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertULY2ToYUYV_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertULY2ToUYVY_RestoreCylindricalLeft(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertULY2ToYUYV_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertULY2ToUYVY_RestorePlanarGradient(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
			ConvertULY2ToYUYV_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			ConvertULY2ToUYVY_RestoreCylindricalWrongMedian(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0]);
			return true;
		}
		break;
	}

	return false;
}

template<class C>
bool CULYUV422Codec<C>::IsDirectRestorable()
{
	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_yuvs:
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
		case UTVF_HDYC:
			return true;
		}
		break;
	}

	return false;
}
