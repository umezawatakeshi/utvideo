/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULYUV420Codec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "ColorOrder.h"
#include "Convert.h"
#include "ConvertPredict.h"

template<>
const utvf_t CULYUV420Codec<CBT601Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YV12,
	UTVF_NV12,
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
const utvf_t CULYUV420Codec<CBT601Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YV12,
	UTVF_NV12,
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
const utvf_t CULYUV420Codec<CBT709Coefficient>::m_utvfEncoderInput[] = {
	UTVF_YV12,
	UTVF_NV12,
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
const utvf_t CULYUV420Codec<CBT709Coefficient>::m_utvfDecoderOutput[] = {
	UTVF_YV12,
	UTVF_NV12,
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
const utvf_t CULYUV420Codec<C>::m_utvfCompressed[] = {
	m_utvfCodec,
	UTVF_INVALID,
};

template<>
CULYUV420Codec<CBT601Coefficient>::CULYUV420Codec(const char *pszInterfaceName) : CUL00Codec("ULY0", pszInterfaceName)
{
}

template<>
CULYUV420Codec<CBT709Coefficient>::CULYUV420Codec(const char *pszInterfaceName) : CUL00Codec("ULH0", pszInterfaceName)
{
}

template<>
const char *CULYUV420Codec<CBT601Coefficient>::GetColorFormatName(void)
{
	return "YUV420 BT.601";
}

template<>
const char *CULYUV420Codec<CBT709Coefficient>::GetColorFormatName(void)
{
	return "YUV420 BT.709";
}

template<class C>
void CULYUV420Codec<C>::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height / 4;
	m_cbPlaneSize[2]          = width * height / 4;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width / 2;
	m_cbPlaneWidth[2]         = width / 2;

	m_cbPlaneStripeSize[0]    = width * 2;
	m_cbPlaneStripeSize[1]    = width / 2;
	m_cbPlaneStripeSize[2]    = width / 2;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width / 2;
	m_cbPlanePredictStride[2] = width / 2;
}

template<class T>
void ConvertPackedYUV422ToULY0(uint8_t *pDstYBegin, uint8_t *pDstUBegin, uint8_t *pDstVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
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
			for (const uint8_t *p = pStrideBegin; p < pStrideEnd; p += 4)
			{
				const uint8_t *q = p + scbPredictStride;
				*(y+0)                    = *(p+T::Y0);
				*(y+1)                    = *(p+T::Y1);
				*(y+dwYPlaneGrossWidth+0) = *(q+T::Y0);
				*(y+dwYPlaneGrossWidth+1) = *(q+T::Y1);
				*u                        = (*(p+T::U) + *(q+T::U)) / 2;
				*v                        = (*(p+T::V) + *(q+T::V)) / 2;

				y+=2; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

template<class C>
void CULYUV420Codec<C>::ConvertToPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_NV12:
		{
			memcpy(y, pRawBegin[0], pRawEnd[0] - pRawBegin[0]);
			for (auto p = pRawBegin[1]; p < pRawEnd[1]; p += 2)
			{
				*u++ = p[0];
				*v++ = p[1];
			}
		}
		break;
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertPackedYUV422ToULY0<CYUYVColorOrder>(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertPackedYUV422ToULY0<CUYVYColorOrder>(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULY0(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULY0(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULY0(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULY0(C)(y, u, v, pRawBegin[0], pRawEnd[0], m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	}
}

template<class T>
void ConvertULY0ToPackedYUV422(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcYBegin, const uint8_t *pSrcUBegin, const uint8_t *pSrcVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace)
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
			for (uint8_t *p = pStrideBegin; p < pStrideEnd; p += 4)
			{
				uint8_t *q = p + scbPredictStride;
				*(p+T::Y0) = *(y+0);
				*(p+T::Y1) = *(y+1);
				*(q+T::Y0) = *(y+dwYPlaneGrossWidth+0);
				*(q+T::Y1) = *(y+dwYPlaneGrossWidth+1);
				*(p+T::U)  = *u;
				*(q+T::U)  = *u;
				*(p+T::V)  = *v;
				*(q+T::V)  = *v;

				y+=2; u++; v++;
			}
		}
		y += dwYPlaneGrossWidth;
	}
}

template<class C>
void CULYUV420Codec<C>::ConvertFromPlanar(uint32_t nBandIndex)
{
	auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);
	auto& [y, u, v, _] = pPlaneBegin;

	switch (m_utvfRaw)
	{
	case UTVF_NV12:
	{
		memcpy(pRawBegin[0], y, pRawEnd[0] - pRawBegin[0]);
		for (auto p = pRawBegin[1]; p < pRawEnd[1]; p += 2)
		{
			p[0] = *u++;
			p[1] = *v++;
		}
	}
	break;
	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
		ConvertULY0ToPackedYUV422<CYUYVColorOrder>(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		ConvertULY0ToPackedYUV422<CUYVYColorOrder>(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
		ConvertULY0ToBGR(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRX_TD:
		ConvertULY0ToBGRX(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULY0ToRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULY0ToXRGB(C)(pRawBegin[0], pRawEnd[0], y, u, v, m_fmRaw.cbLineWidth[0], m_fmRaw.scbLineStride[0], m_cbPlanePredictStride[0], m_bInterlace);
		break;
	}
}

template<class C>
bool CULYUV420Codec<C>::PredictDirect(uint32_t nBandIndex)
{
	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			auto [pRawPlaneBegin] = CalcPlanePosition<true>();

			PredictFromPlanar(nBandIndex, pRawPlaneBegin);
		}
		return true;
	case UTVF_NV12:
		{
			auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pPredicted.get(), nBandIndex);
			auto& [y, u, v, _] = pPlaneBegin;

			switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
			{
			case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
				PredictCylindricalLeftAndCount8(y, pRawBegin[0], pRawEnd[0], m_counts[nBandIndex].dwCount[0]);
				ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount(u, v, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
				return true;
			case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
				if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
					return false;
				PredictPlanarGradientAndCount8(y, pRawBegin[0], pRawEnd[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
				ConvertPackedUVToPlanar_PredictPlanarGradientAndCount(u, v, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
				return true;
			case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
				if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE)
					return false;
				PredictCylindricalWrongMedianAndCount8(y, pRawBegin[0], pRawEnd[0], m_fmRaw.scbLineStride[0], m_counts[nBandIndex].dwCount[0]);
				ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount(u, v, pRawBegin[1], pRawEnd[1], m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2]);
				return true;
			}
		}
		break;
	case UTVF_YV16:
		{
			auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<true>(m_pCurFrame.get(), nBandIndex);

			size_t cbChromaStride = m_nWidth / 2 * (m_bInterlace ? 2 : 1);

			auto du = pPlaneBegin[1];
			auto dv = pPlaneBegin[2];
			auto su = pRawBegin[1];
			auto sv = pRawBegin[2];
			for (auto i = m_dwStripeBegin[nBandIndex]; i < m_dwStripeEnd[nBandIndex]; ++i)
			{
				for (size_t j = 0; j < cbChromaStride; ++j)
				{
					du[j] = (su[j] + su[j + cbChromaStride]) / 2;
					dv[j] = (sv[j] + sv[j + cbChromaStride]) / 2;
				}
				du += cbChromaStride;
				dv += cbChromaStride;
				su += cbChromaStride * 2;
				sv += cbChromaStride * 2;
			}

			const uint8_t *pSrcBegin[3];

			pSrcBegin[0] = ((const uint8_t *)m_pInput);
			pSrcBegin[1] = m_pCurFrame->GetPlane(1);
			pSrcBegin[2] = m_pCurFrame->GetPlane(2);

			PredictFromPlanar(nBandIndex, pSrcBegin);
		}
		return true;
	}

	return false;
}

template<class C>
bool CULYUV420Codec<C>::DecodeDirect(uint32_t nBandIndex)
{
	switch (m_utvfRaw)
	{
	case UTVF_YV12:
		{
			auto [pRawPlaneBegin] = CalcPlanePosition<false>();

			DecodeAndRestoreToPlanar(nBandIndex, pRawPlaneBegin);
		}
		return true;
	case UTVF_YV16:
		{
			uint8_t *pDstBegin[3];

			pDstBegin[0] = ((uint8_t *)m_pOutput);
			pDstBegin[1] = m_pCurFrame->GetPlane(1);
			pDstBegin[2] = m_pCurFrame->GetPlane(2);

			DecodeAndRestoreToPlanar(nBandIndex, pDstBegin);

			auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pCurFrame.get(), nBandIndex);

			size_t cbChromaStride = m_nWidth / 2 * (m_bInterlace ? 2 : 1);

			auto du = pRawBegin[1];
			auto dv = pRawBegin[2];
			auto su = pPlaneBegin[1];
			auto sv = pPlaneBegin[2];
			for (auto i = m_dwStripeBegin[nBandIndex]; i < m_dwStripeEnd[nBandIndex]; ++i)
			{
				memcpy(du, su, cbChromaStride);
				memcpy(du + cbChromaStride, su, cbChromaStride);
				memcpy(dv, sv, cbChromaStride);
				memcpy(dv + cbChromaStride, sv, cbChromaStride);
				du += cbChromaStride * 2;
				dv += cbChromaStride * 2;
				su += cbChromaStride;
				sv += cbChromaStride;
			}
		}
		return true;
	}

	return false;
}

template<class C>
bool CULYUV420Codec<C>::RestoreDirect(uint32_t nBandIndex)
{
	switch (m_utvfRaw)
	{
	case UTVF_NV12:
		{
			auto& [pRawBegin, pRawEnd, pPlaneBegin] = CalcBandPosition<false>(m_pPredicted.get(), nBandIndex);
			auto& [y, u, v, _] = pPlaneBegin;

			switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
			{
			case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
			case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
				RestoreCylindricalLeft8(pRawBegin[0], y, y + (pRawEnd[0] - pRawBegin[0]));
				ConvertPlanarToPackedUV_RestoreCylindricalLeft(pRawBegin[1], pRawEnd[1], u, v, m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1]);
				return true;
			case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
				if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
					return false;
				RestorePlanarGradient8(pRawBegin[0], y, y + (pRawEnd[0] - pRawBegin[0]), m_fmRaw.scbLineStride[0]);
				ConvertPlanarToPackedUV_RestorePlanarGradient(pRawBegin[1], pRawEnd[1], u, v, m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1]);
				return true;
			case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
				if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
					return false;
				RestoreCylindricalWrongMedian8(pRawBegin[0], y, y + (pRawEnd[0] - pRawBegin[0]), m_fmRaw.scbLineStride[0]);
				ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian(pRawBegin[1], pRawEnd[1], u, v, m_fmRaw.cbLineWidth[1], m_fmRaw.scbLineStride[1]);
				return true;
			}
		}
		break;
	}

	return false;
}

template<class C>
bool CULYUV420Codec<C>::IsDirectRestorable()
{
	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NV12:
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NV12:
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_ed.flags0 & BIE_FLAGS0_ASSUME_INTERLACE)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NV12:
			return true;
		}
		break;
	}

	return false;
}
