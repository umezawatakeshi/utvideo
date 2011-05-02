/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

// DMODecoder.cpp : CDMODecoder ‚ÌŽÀ‘•

#include "stdafx.h"
#include "DMODecoder.h"

// CDMODecoder

const GUID &CDMODecoder::DMOCATEGORY = DMOCATEGORY_VIDEO_DECODER;

HRESULT CDMODecoder::InternalAllocateStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMODecoder::InternalAllocateStreamingResources()\n");

	const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
	const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;

	if (m_pCodec->DecompressBegin(&pvihIn->bmiHeader, &pvihOut->bmiHeader) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CDMODecoder::InternalFreeStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMODecoder::InternalFreeStreamingResources()\n");

	m_pCodec->DecompressEnd();

	return S_OK;
}

HRESULT CDMODecoder::InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus)
{
	_RPT0(_CRT_WARN, "CDMODecoder::InternalProcessOutput()\n");

	const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
	const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;
	ICDECOMPRESS icd;
	DWORD ckid;
	DWORD dwAviFlags;

	if (m_pInputBuffer == NULL)
		return S_FALSE;

	memset(&icd, 0, sizeof(icd));
	icd.dwFlags = m_bInputKeyFrame ? 0 : ICDECOMPRESS_NOTKEYFRAME;
	icd.lpbiOutput = (BITMAPINFOHEADER *)malloc(pvihOut->bmiHeader.biSize);
	memcpy(icd.lpbiOutput, &pvihOut->bmiHeader, pvihOut->bmiHeader.biSize);
	pOutputBuffers->pBuffer->GetBufferAndLength((BYTE **)&icd.lpOutput, NULL);
	icd.lpbiInput = (BITMAPINFOHEADER *)malloc(pvihIn->bmiHeader.biSize);
	memcpy(icd.lpbiInput, &pvihIn->bmiHeader, pvihIn->bmiHeader.biSize);
	m_pInputBuffer->GetBufferAndLength((BYTE **)&icd.lpInput, &icd.lpbiInput->biSizeImage);
	icd.ckid = FCC('dcdc');

	m_pCodec->Decompress(&icd, sizeof(icd));

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
	pOutputBuffers->pBuffer->SetLength(icd.lpbiOutput->biSizeImage);
	_RPT1(_CRT_WARN, "Size = %d\n", icd.lpbiOutput->biSizeImage);

	free(icd.lpbiOutput);
	free(icd.lpbiInput);

	m_pInputBuffer->Release();
	m_pInputBuffer = NULL;

	return S_OK;	
}
