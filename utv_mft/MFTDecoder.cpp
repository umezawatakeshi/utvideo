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

HRESULT CMFTDecoder::InternalProcessOutput(IMFSample *pOutputSample, IMFSample *pInputSample)
{
	_RPT0(_CRT_WARN, "CMFTDecoder::InternalProcessOutput()\n");

	size_t cbOutput;
	UINT32 bKeyFrame;
	IMFMediaBuffer *pInputBuffer;
	IMFMediaBuffer *pOutputBuffer;
	BYTE *pInputByteBuffer;
	BYTE *pOutputByteBuffer;

	if (FAILED(m_pInputSample->GetUINT32(MFSampleExtension_CleanPoint, &bKeyFrame)))
		bKeyFrame = FALSE;
	pInputSample->GetBufferByIndex(0, &pInputBuffer);
	pOutputSample->GetBufferByIndex(0, &pOutputBuffer);
	pInputBuffer->Lock(&pInputByteBuffer, NULL, NULL);
	pOutputBuffer->Lock(&pOutputByteBuffer, NULL, NULL);
	cbOutput = m_pCodec->DecodeFrame(pOutputByteBuffer, pInputByteBuffer, (bKeyFrame != FALSE) /* convert to bool */);
	pInputBuffer->Unlock();
	pOutputBuffer->Unlock();
	pOutputBuffer->SetCurrentLength((DWORD)cbOutput);
	pOutputSample->SetUINT32(MFSampleExtension_CleanPoint, TRUE);
	pInputBuffer->Release();
	pOutputBuffer->Release();

	return S_OK;
}
