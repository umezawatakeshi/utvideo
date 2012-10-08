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

	UINT8 *pData;
	UINT32 cbData;
	UINT32 u32FrameWidth, u32FrameHeight;
	GUID guidSubtype;
	utvf_t outfmt;
	int ret;

	m_pOutputMediaType->GetGUID(MF_MT_SUBTYPE, &guidSubtype);
	if (MediaFoundationFormatToUtVideoFormat(&outfmt, guidSubtype))
		return MF_E_INVALIDMEDIATYPE;
	MFGetAttributeSize(m_pInputMediaType, MF_MT_FRAME_SIZE, &u32FrameWidth, &u32FrameHeight);
	m_pInputMediaType->GetAllocatedBlob(MF_MT_USER_DATA, &pData, &cbData);

	ret = m_pCodec->DecodeBegin(outfmt, u32FrameWidth, u32FrameHeight, CBGROSSWIDTH_WINDOWS, pData, cbData);
	CoTaskMemFree(pData);
	if (ret == 0)
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

/*
HRESULT CMFTDecoder::InternalAllocateStreamingResources()
{
	_RPT0(_CRT_WARN, "CMFTDecoder::InternalAllocateStreamingResources()\n");

	const MFT_MEDIA_TYPE *pmtIn  = InputType(0);
	const MFT_MEDIA_TYPE *pmtOut = OutputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;
	utvf_t outfmt;

	if (DirectShowFormatToUtVideoFormat(&outfmt, pvihOut->bmiHeader.biCompression, pvihOut->bmiHeader.biBitCount, pmtOut->subtype) != 0)
		return MFT_E_INVALIDTYPE;

	if (m_pCodec->DecodeBegin(outfmt, pvihOut->bmiHeader.biWidth, pvihOut->bmiHeader.biHeight, CBGROSSWIDTH_WINDOWS,
			((BYTE *)&pvihIn->bmiHeader) + sizeof(BITMAPINFOHEADER), pvihIn->bmiHeader.biSize - sizeof(BITMAPINFOHEADER)) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CMFTDecoder::InternalFreeStreamingResources()
{
	_RPT0(_CRT_WARN, "CMFTDecoder::InternalFreeStreamingResources()\n");

	m_pCodec->DecodeEnd();

	return S_OK;
}

HRESULT CMFTDecoder::InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus)
{
	_RPT0(_CRT_WARN, "CMFTDecoder::InternalProcessOutput()\n");

	BYTE *pInput;
	BYTE *pOutput;
	size_t cbOutput;

	if (m_pInputBuffer == NULL)
		return S_FALSE;

	pOutputBuffers->pBuffer->GetBufferAndLength(&pOutput, NULL);
	m_pInputBuffer->GetBufferAndLength(&pInput, NULL);

	cbOutput = m_pCodec->DecodeFrame(pOutput, pInput, m_bInputKeyFrame != 0);

	pOutputBuffers->dwStatus = 0;
	pOutputBuffers->dwStatus |= MFT_OUTPUT_DATA_BUFFERF_SYNCPOINT;
	if (m_bInputTimestampValid)
	{
		pOutputBuffers->dwStatus |= MFT_OUTPUT_DATA_BUFFERF_TIME;
		pOutputBuffers->rtTimestamp = m_rtInputTimestamp;
	}
	if (m_bInputTimelengthValid)
	{
		pOutputBuffers->dwStatus |= MFT_OUTPUT_DATA_BUFFERF_TIMELENGTH;
		pOutputBuffers->rtTimelength = m_rtInputTimelength;
	}
	pOutputBuffers->pBuffer->SetLength((DWORD)cbOutput);
	_RPT1(_CRT_WARN, "Size = %u\n", (unsigned int)cbOutput);

	m_pInputBuffer->Release();
	m_pInputBuffer = NULL;

	return S_OK;	
}
*/