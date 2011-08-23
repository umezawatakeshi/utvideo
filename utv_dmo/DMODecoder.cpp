/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

// DMODecoder.cpp : CDMODecoder ‚ÌÀ‘•

#include "stdafx.h"
#include "DMODecoder.h"
#include <Format.h>

// CDMODecoder

const GUID &CDMODecoder::DMOCATEGORY = DMOCATEGORY_VIDEO_DECODER;

HRESULT CDMODecoder::InternalAllocateStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMODecoder::InternalAllocateStreamingResources()\n");

	const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
	const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;
	utvf_t outfmt;

	if (WindowsFormatToUtVideoFormat(&outfmt, pvihOut->bmiHeader.biCompression, pvihOut->bmiHeader.biBitCount, pmtOut->subtype) != 0)
		return DMO_E_INVALIDTYPE;

	if (m_pCodec->DecodeBegin(outfmt, pvihOut->bmiHeader.biWidth, pvihOut->bmiHeader.biHeight, CBGROSSWIDTH_WINDOWS,
			((BYTE *)&pvihIn->bmiHeader) + sizeof(BITMAPINFOHEADER), pvihIn->bmiHeader.biSize - sizeof(BITMAPINFOHEADER)) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CDMODecoder::InternalFreeStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMODecoder::InternalFreeStreamingResources()\n");

	m_pCodec->DecodeEnd();

	return S_OK;
}

HRESULT CDMODecoder::InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus)
{
	_RPT0(_CRT_WARN, "CDMODecoder::InternalProcessOutput()\n");

	BYTE *pInput;
	BYTE *pOutput;
	size_t cbOutput;

	if (m_pInputBuffer == NULL)
		return S_FALSE;

	pOutputBuffers->pBuffer->GetBufferAndLength(&pOutput, NULL);
	m_pInputBuffer->GetBufferAndLength(&pInput, NULL);

	cbOutput = m_pCodec->DecodeFrame(pOutput, pInput, m_bInputKeyFrame != 0);

	pOutputBuffers->dwStatus = 0;
	pOutputBuffers->dwStatus |= DMO_OUTPUT_DATA_BUFFERF_SYNCPOINT;
	if (m_bInputTimestampValid)
	{
		pOutputBuffers->dwStatus |= DMO_OUTPUT_DATA_BUFFERF_TIME;
		pOutputBuffers->rtTimestamp = m_rtInputTimestamp;
	}
	if (m_bInputTimelengthValid)
	{
		pOutputBuffers->dwStatus |= DMO_OUTPUT_DATA_BUFFERF_TIMELENGTH;
		pOutputBuffers->rtTimelength = m_rtInputTimelength;
	}
	pOutputBuffers->pBuffer->SetLength((DWORD)cbOutput);
	_RPT1(_CRT_WARN, "Size = %u\n", (unsigned int)cbOutput);

	m_pInputBuffer->Release();
	m_pInputBuffer = NULL;

	return S_OK;	
}
