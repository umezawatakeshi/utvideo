/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

// MFTEncoder.cpp : CMFTEncoder ‚ÌÀ‘•

#include "stdafx.h"
#include "MFTEncoder.h"
#include <Format.h>

// CMFTEncoder

const GUID &CMFTEncoder::MFTCATEGORY = MFT_CATEGORY_VIDEO_ENCODER;

HRESULT CMFTEncoder::InternalBeginStream(void)
{
	LOGPRINTF("%p CMFTEncoder::InternalBeginStream()", this);

	if (m_pCodec->EncodeBegin(m_infmt, m_nFrameWidth, m_nFrameHeight, m_cbInStride) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CMFTEncoder::InternalEndStream(void)
{
	LOGPRINTF("%p CMFTEncoder::InternalEndStream()", this);

	m_pCodec->EncodeEnd();

	return S_OK;
}

HRESULT CMFTEncoder::InternalProcessOutput(IMFSample *pOutputSample, IMFSample *pInputSample)
{
	size_t cbOutput;
	bool bKeyFrame;
	IMFMediaBuffer *pInputBuffer;
	IMFMediaBuffer *pOutputBuffer;
	BYTE *pInputByteBuffer;
	BYTE *pOutputByteBuffer;

	pInputSample->GetBufferByIndex(0, &pInputBuffer);
	pOutputSample->GetBufferByIndex(0, &pOutputBuffer);
	pInputBuffer->Lock(&pInputByteBuffer, NULL, NULL);
	pOutputBuffer->Lock(&pOutputByteBuffer, NULL, NULL);
	cbOutput = m_pCodec->EncodeFrame(pOutputByteBuffer, &bKeyFrame, pInputByteBuffer);
	pInputBuffer->Unlock();
	pOutputBuffer->Unlock();
	pOutputBuffer->SetCurrentLength((DWORD)cbOutput);
	pOutputSample->SetUINT32(MFSampleExtension_CleanPoint, bKeyFrame);
	pInputBuffer->Release();
	pOutputBuffer->Release();

	return S_OK;
}
