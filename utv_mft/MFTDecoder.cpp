/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

// MFTDecoder.cpp : CMFTDecoder ‚ÌÀ‘•

#include "stdafx.h"
#include "MFTDecoder.h"
#include <Format.h>

// CMFTDecoder

const GUID &CMFTDecoder::MFTCATEGORY = MFT_CATEGORY_VIDEO_DECODER;

HRESULT CMFTDecoder::InternalBeginStream(void)
{
	_RPT0(_CRT_WARN, "CMFTDecoder::InternalBeginStream()\n");

	if (m_pCodec->DecodeBegin(m_outfmt, m_nFrameWidth, m_nFrameHeight, CBGROSSWIDTH_WINDOWS, m_pInputUserData, m_cbInputUserData) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CMFTDecoder::InternalEndStream(void)
{
	_RPT0(_CRT_WARN, "CMFTDecoder::InternalEndStream()\n");

	m_pCodec->DecodeEnd();

	return S_OK;
}
