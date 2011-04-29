/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

// DMOEncoder.cpp : CDMOEncoder ‚ÌŽÀ‘•

#include "stdafx.h"
#include "DMOEncoder.h"

// CDMOEncoder

HRESULT CDMOEncoder::InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalProcessOutput()\n");

	const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
	const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;
	ICCOMPRESS icc;
	DWORD ckid;
	DWORD dwAviFlags;

	if (m_pInputBuffer == NULL)
		return S_FALSE;

	memset(&icc, 0, sizeof(icc));
	icc.dwFlags = m_bInputKeyFrame ? ICCOMPRESS_KEYFRAME : 0;
	icc.lpbiOutput = (BITMAPINFOHEADER *)malloc(pvihOut->bmiHeader.biSize);
	memcpy(icc.lpbiOutput, &pvihOut->bmiHeader, pvihOut->bmiHeader.biSize);
	pOutputBuffers->pBuffer->GetBufferAndLength((BYTE **)&icc.lpOutput, NULL);
	icc.lpbiInput = (BITMAPINFOHEADER *)malloc(pvihIn->bmiHeader.biSize);
	memcpy(icc.lpbiInput, &pvihIn->bmiHeader, pvihIn->bmiHeader.biSize);
	m_pInputBuffer->GetBufferAndLength((BYTE **)&icc.lpInput, NULL);
	icc.lpckid = &ckid;
	icc.lpdwFlags = &dwAviFlags;
	icc.lFrameNum = 0; // XXX
	icc.dwFrameSize = 0;
	icc.dwQuality = 0;

	m_pCodec->Compress(&icc, sizeof(icc));

	pOutputBuffers->dwStatus = 0;
	if (dwAviFlags & AVIIF_KEYFRAME)
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
	pOutputBuffers->pBuffer->SetLength(icc.lpbiOutput->biSizeImage);
	_RPT1(_CRT_WARN, "Size = %d\n", icc.lpbiOutput->biSizeImage);

	free(icc.lpbiOutput);
	free(icc.lpbiInput);

	m_pInputBuffer->Release();
	m_pInputBuffer = NULL;

	return S_OK;	
}


// IPersist

HRESULT STDMETHODCALLTYPE CDMOEncoder::GetClassID(CLSID *pClassID)
{
	*pClassID = m_clsid;

	return S_OK;
}


// IPersistStream

HRESULT STDMETHODCALLTYPE CDMOEncoder::IsDirty(void)
{
	return S_OK; /* XXX í‚É dirty */
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::Load(IStream *pStm)
{
	ULONG cbState;
	ULONG cbRead;
	void *pState;
	HRESULT hr;

	LockIt lck(this);

	cbState = m_pCodec->GetStateSize();
	pState = malloc(cbState);
	hr = pStm->Read(pState, cbState, &cbRead);
	if (FAILED(hr))
	{
		free(pState);
		return hr;
	}
	m_pCodec->SetState(pState, cbRead);
	free(pState);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::Save(IStream *pStm, BOOL fClearDirty)
{
	ULONG cbState;
	ULONG cbWritten;
	void *pState;
	HRESULT hr;

	LockIt lck(this);

	cbState = m_pCodec->GetStateSize();
	pState = malloc(cbState);
	m_pCodec->GetState(pState, cbState);
	hr = pStm->Write(pState, cbState, &cbWritten);
	free(pState);
	if (FAILED(hr))
		return hr;
	if (cbWritten != cbState)
		return STG_E_MEDIUMFULL;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	LockIt lck(this);

	pcbSize->QuadPart = m_pCodec->GetStateSize();

	return S_OK;
}


// IAMVfwCompressDialogs

HRESULT STDMETHODCALLTYPE CDMOEncoder::ShowDialog(int iDialog, HWND hwnd)
{
	LockIt lck(this);

	switch (iDialog)
	{
	case VfwCompressDialog_Config:
		m_pCodec->Configure(hwnd);
		return S_OK;
	case VfwCompressDialog_About:
		m_pCodec->About(hwnd);
		return S_OK;
	case VfwCompressDialog_QueryConfig:
	case VfwCompressDialog_QueryAbout:
		return S_OK;
	default:
		return E_INVALIDARG;
	}
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::GetState(LPVOID pState, int *pcbState)
{
	LockIt lck(this);

	*pcbState = m_pCodec->GetState(pState, *pcbState);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::SetState(LPVOID pState, int cbState)
{
	LockIt lck(this);

	m_pCodec->SetState(pState, cbState);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDMOEncoder::SendDriverMessage(int uMsg, long dw1, long dw2)
{
	return E_NOTIMPL;
}
