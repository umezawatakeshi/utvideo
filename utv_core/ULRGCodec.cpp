/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "ULRGCodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"

const utvf_t CULRGCodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRGCodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_NFCC_BGRX_BU,
	UTVF_NFCC_BGR_TD,
	UTVF_NFCC_BGRX_TD,
	UTVF_NFCC_RGB_TD,
	UTVF_NFCC_ARGB_TD,
	UTVF_INVALID,
};

const utvf_t CULRGCodec::m_utvfCompressed[] = {
	UTVF_ULRG,
	UTVF_INVALID,
};

CULRGCodec::CULRGCodec(const char *pszInterfaceName) : CUL00Codec("ULRG", pszInterfaceName)
{
}

void CULRGCodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_dwPlaneSize[0]          = width * height;
	m_dwPlaneSize[1]          = width * height;
	m_dwPlaneSize[2]          = width * height;

	m_dwPlaneWidth[0]         = width;
	m_dwPlaneWidth[1]         = width;
	m_dwPlaneWidth[2]         = width;

	m_dwPlaneStripeSize[0]    = width;
	m_dwPlaneStripeSize[1]    = width;
	m_dwPlaneStripeSize[2]    = width;

	m_dwPlanePredictStride[0] = width;
	m_dwPlanePredictStride[1] = width;
	m_dwPlanePredictStride[2] = width;
}

void CULRGCodec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertBGRToULRG(g, b, r, pSrcEnd - m_dwRawGrossWidth, pSrcBegin - m_dwRawGrossWidth, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertBGRXToULRG(g, b, r, pSrcEnd - m_dwRawGrossWidth, pSrcBegin - m_dwRawGrossWidth, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertBGRToULRG(g, b, r, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertBGRXToULRG(g, b, r, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertRGBToULRG(g, b, r, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertXRGBToULRG(g, b, r, pSrcBegin, pSrcEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	}
}

void CULRGCodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_utvfRaw)
	{
	case UTVF_NFCC_BGR_BU:
		ConvertULRGToBGR(pDstEnd - m_dwRawGrossWidth, pDstBegin - m_dwRawGrossWidth, g, b, r, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_BU:
		ConvertULRGToBGRX(pDstEnd - m_dwRawGrossWidth, pDstBegin - m_dwRawGrossWidth, g, b, r, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_BGR_TD:
		ConvertULRGToBGR(pDstBegin, pDstEnd, g, b, r, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_BGRX_TD:
		ConvertULRGToBGRX(pDstBegin, pDstEnd, g, b, r, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_RGB_TD:
		ConvertULRGToRGB(pDstBegin, pDstEnd, g, b, r, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	case UTVF_NFCC_ARGB_TD:
		ConvertULRGToXRGB(pDstBegin, pDstEnd, g, b, r, m_dwRawNetWidth, m_dwRawGrossWidth);
		break;
	}
}

int CULRGCodec::DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	int ret;

	ret = CUL00Codec::DecodeBegin(outfmt, width, height, cbGrossWidth, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	m_pRawDecoded = new CFrameBuffer();
	m_pRawDecoded->AddPlane(m_dwRawSize, 0);

	return 0;
}

int CULRGCodec::DecodeEnd(void)
{
	delete m_pRawDecoded;

	return CUL00Codec::DecodeEnd();
}

bool CULRGCodec::DecodeDirect(uint32_t nBandIndex)
{
	uint8_t *pDstBegin = ((uint8_t *)m_pOutput) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	uint8_t *pDstEnd   = ((uint8_t *)m_pOutput) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	uint8_t *pDecBegin = ((uint8_t *)m_pRawDecoded->GetBuffer()) + m_dwRawStripeBegin[nBandIndex] * m_dwRawStripeSize;
	uint8_t *pDecEnd   = ((uint8_t *)m_pRawDecoded->GetBuffer()) + m_dwRawStripeEnd[nBandIndex]   * m_dwRawStripeSize;

	switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGR_BU:
			HuffmanDecodeAndAccumStep3          (pDstEnd-m_dwRawGrossWidth+1, pDstBegin-m_dwRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRBlue(pDstEnd-m_dwRawGrossWidth+0, pDstBegin-m_dwRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRRed (pDstEnd-m_dwRawGrossWidth+2, pDstBegin-m_dwRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_BU:
			HuffmanDecodeAndAccumStep4                       (pDstEnd-m_dwRawGrossWidth+1, pDstBegin-m_dwRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXBlue            (pDstEnd-m_dwRawGrossWidth+0, pDstBegin-m_dwRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(pDstEnd-m_dwRawGrossWidth+2, pDstBegin-m_dwRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			return true;
		case UTVF_NFCC_BGR_TD:
			HuffmanDecodeAndAccumStep3          (pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRBlue(pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForBGRRed (pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, m_dwRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			HuffmanDecodeAndAccumStep4                       (pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXBlue            (pDstBegin+0, pDstEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha(pDstBegin+2, pDstEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, m_dwRawGrossWidth);
			return true;
		case UTVF_NFCC_RGB_TD:
			HuffmanDecodeAndAccumStep3          (pDstBegin+1, pDstEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForRGBBlue(pDstBegin+2, pDstEnd+2, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep3ForRGBRed (pDstBegin+0, pDstEnd+0, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, m_dwRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			HuffmanDecodeAndAccumStep4                       (pDstBegin+2, pDstEnd+2, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForXRGBBlue            (pDstBegin+3, pDstEnd+3, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha(pDstBegin+1, pDstEnd+1, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, m_dwRawGrossWidth);
			return true;
		}
		break;
	case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		switch (m_utvfRaw)
		{
		case UTVF_NFCC_BGRX_BU:
			HuffmanDecodeStep4(pDecEnd-m_dwRawGrossWidth+1, pDecBegin-m_dwRawGrossWidth+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			HuffmanDecodeStep4(pDecEnd-m_dwRawGrossWidth+0, pDecBegin-m_dwRawGrossWidth+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			HuffmanDecodeStep4(pDecEnd-m_dwRawGrossWidth+2, pDecBegin-m_dwRawGrossWidth+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			RestoreWrongMedianBlock4(pDstEnd-m_dwRawGrossWidth, pDecEnd-m_dwRawGrossWidth, pDecBegin-m_dwRawGrossWidth, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			EncorrelateInplaceBGRX(pDstEnd-m_dwRawGrossWidth, pDstBegin-m_dwRawGrossWidth, m_dwRawNetWidth, -(ssize_t)m_dwRawGrossWidth);
			return true;
		case UTVF_NFCC_BGRX_TD:
			HuffmanDecodeStep4(pDecBegin+1, pDecEnd+1, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+0, pDecEnd+0, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+2, pDecEnd+2, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, m_dwRawGrossWidth);
			RestoreWrongMedianBlock4(pDstBegin, pDecBegin, pDecEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
			EncorrelateInplaceBGRX(pDstBegin, pDstEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
			return true;
		case UTVF_NFCC_ARGB_TD:
			HuffmanDecodeStep4(pDecBegin+2, pDecEnd+2, m_pDecodeCode[0][nBandIndex], &m_hdt[0], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+3, pDecEnd+3, m_pDecodeCode[1][nBandIndex], &m_hdt[1], m_dwRawNetWidth, m_dwRawGrossWidth);
			HuffmanDecodeStep4(pDecBegin+1, pDecEnd+1, m_pDecodeCode[2][nBandIndex], &m_hdt[2], m_dwRawNetWidth, m_dwRawGrossWidth);
			RestoreWrongMedianBlock4(pDstBegin, pDecBegin, pDecEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
			EncorrelateInplaceXRGB(pDstBegin, pDstEnd, m_dwRawNetWidth, m_dwRawGrossWidth);
			return true;
		}
	}

	return false;
}
