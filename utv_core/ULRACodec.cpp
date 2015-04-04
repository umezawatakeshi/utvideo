/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULRACodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

const utvf_t CULRACodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGRA_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGRA_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRACodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGRA_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGRA_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRACodec::m_utvfCompressed[] = {
	UTVF_ULRA,
	UTVF_INVALID,
};

CULRACodec::CULRACodec(const char *pszInterfaceName) : CUL00Codec("ULRA", pszInterfaceName)
{
}

void CULRACodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height;
	m_cbPlaneSize[1]          = width * height;
	m_cbPlaneSize[2]          = width * height;
	m_cbPlaneSize[3]          = width * height;

	m_cbPlaneWidth[0]         = width;
	m_cbPlaneWidth[1]         = width;
	m_cbPlaneWidth[2]         = width;
	m_cbPlaneWidth[3]         = width;

	m_cbPlaneStripeSize[0]    = width;
	m_cbPlaneStripeSize[1]    = width;
	m_cbPlaneStripeSize[2]    = width;
	m_cbPlaneStripeSize[3]    = width;

	m_cbPlanePredictStride[0] = width;
	m_cbPlanePredictStride[1] = width;
	m_cbPlanePredictStride[2] = width;
	m_cbPlanePredictStride[3] = width;
}

void CULRACodec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r, *a;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRAToULRA(g, b, r, a, pSrcEnd - m_cbRawGrossWidth, pSrcBegin - m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRA_TD:
		ConvertBGRAToULRA(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertARGBToULRA(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

void CULRACodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r, *a;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_BU:
		ConvertULRAToBGRA(pDstEnd - m_cbRawGrossWidth, pDstBegin - m_cbRawGrossWidth, g, b, r, a, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_BGRA_TD:
		ConvertULRAToBGRA(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRAToARGB(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

int CULRACodec::DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	int ret;

	ret = CUL00Codec::DecodeBegin(outfmt, width, height, cbGrossWidth, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	m_pRawDecoded = new CFrameBuffer();
	m_pRawDecoded->AddPlane(m_cbRawSize, 0);

	return 0;
}

int CULRACodec::DecodeEnd(void)
{
	delete m_pRawDecoded;

	return CUL00Codec::DecodeEnd();
}

bool CULRACodec::DecodeDirect(uint32_t nBandIndex)
{
	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	uint8_t *pDecBegin = ((uint8_t *)m_pRawDecoded->GetBuffer()) + m_dwRawStripeBegin[nBandIndex] * m_cbRawStripeSize;
	uint8_t *pDecEnd   = ((uint8_t *)m_pRawDecoded->GetBuffer()) + m_dwRawStripeEnd[nBandIndex]   * m_cbRawStripeSize;

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			HuffmanDecodeAndAccumStep4           (pDstEnd-m_cbRawGrossWidth+1, pDstBegin-m_cbRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXBlue(pDstEnd-m_cbRawGrossWidth+0, pDstBegin-m_cbRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXRed (pDstEnd-m_cbRawGrossWidth+2, pDstBegin-m_cbRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4           (pDstEnd-m_cbRawGrossWidth+3, pDstBegin-m_cbRawGrossWidth+3, m_pDecodeCode[3][nBandIndex], &m_hdt[3], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRA_TD:
			HuffmanDecodeAndAccumStep4           (pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXBlue(pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXRed (pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4           (pDstBegin+3, pDstEnd+3, m_pDecodeCode[3][nBandIndex], &m_hdt[3], m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			HuffmanDecodeAndAccumStep4           (pDstBegin+2, pDstEnd+2, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForXRGBBlue(pDstBegin+3, pDstEnd+3, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForXRGBRed (pDstBegin+1, pDstEnd+1, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeAndAccumStep4           (pDstBegin+0, pDstEnd+0, m_pDecodeCode[3][nBandIndex], &m_hdt[3], m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		if (m_bInterlace)
			return false;
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_BU:
			HuffmanDecodeStep4(pDecEnd-m_cbRawGrossWidth+1, pDecBegin-m_cbRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecEnd-m_cbRawGrossWidth+0, pDecBegin-m_cbRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecEnd-m_cbRawGrossWidth+2, pDecBegin-m_cbRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecEnd-m_cbRawGrossWidth+3, pDecBegin-m_cbRawGrossWidth+3, m_pDecodeCode[3][nBandIndex], &m_hdt[3], m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			RestoreWrongMedianBlock4(pDstEnd-m_cbRawGrossWidth, pDecEnd-m_cbRawGrossWidth, pDecBegin-m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			EncorrelateInplaceBGRA(pDstEnd-m_cbRawGrossWidth, pDstBegin-m_cbRawGrossWidth, m_cbRawNetWidth, -(ssize_t)m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			HuffmanDecodeStep4(pDecBegin+1, pDecEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+0, pDecEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+2, pDecEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+3, pDecEnd+3, m_pDecodeCode[3][nBandIndex], &m_hdt[3], m_cbRawNetWidth, m_cbRawGrossWidth);
			RestoreWrongMedianBlock4(pDstBegin, pDecBegin, pDecEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			EncorrelateInplaceBGRA(pDstBegin, pDstEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			HuffmanDecodeStep4(pDecBegin+2, pDecEnd+2, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+3, pDecEnd+3, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+1, pDecEnd+1, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_cbRawNetWidth, m_cbRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+0, pDecEnd+0, m_pDecodeCode[3][nBandIndex], &m_hdt[3], m_cbRawNetWidth, m_cbRawGrossWidth);
			RestoreWrongMedianBlock4(pDstBegin, pDecBegin, pDecEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			EncorrelateInplaceARGB(pDstBegin, pDstEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
			return true;
		}
	}

	return false;
}
