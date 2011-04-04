/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

// DMOEncoder.cpp : CDMOEncoder ‚ÌŽÀ‘•

#include "stdafx.h"
#include "DMOEncoder.h"

#define FCC4PRINTF(fcc) \
	(BYTE)(fcc), \
	(BYTE)(fcc >> 8), \
	(BYTE)(fcc >> 16), \
	(BYTE)(fcc >> 24)

// CDMOEncoder

CDMOEncoder::CDMOEncoder(DWORD fcc, REFCLSID clsid) :
	m_fcc(fcc), m_clsid(clsid)
{
	m_pCodec = CCodec::CreateInstance(fcc, "Encoder DMO");
	m_pInputBuffer = NULL;
}

CDMOEncoder::~CDMOEncoder()
{
	FreeStreamingResources();

	if (m_pInputBuffer != NULL)
		m_pInputBuffer->Release();
	delete m_pCodec;
}

static void FormatInfoToPartialMediaType(const FORMATINFO *pfi, DWORD *pcTypes, DMO_PARTIAL_MEDIATYPE **ppTypes)
{
	const FORMATINFO *p;
	DWORD n;
	DWORD i;

	p = pfi;
	n = 0;
	while (!IS_FORMATINFO_END(p++))
		n++;

	*pcTypes = n;
	*ppTypes = new DMO_PARTIAL_MEDIATYPE[n];

	for (i = 0; i < n; i++)
	{
		(*ppTypes)[i].type = MEDIATYPE_Video;
		(*ppTypes)[i].subtype = pfi[i].guidMediaSubType;
	}
}

HRESULT WINAPI CDMOEncoder::UpdateRegistry(DWORD fcc, REFCLSID clsid, BOOL bRegister)
{
	HRESULT hr;
	OLECHAR szFcc[5];
	OLECHAR szClsID[64];
	_ATL_REGMAP_ENTRY regmap[3] = {
		{ L"FCC",   szFcc   },
		{ L"CLSID", szClsID },
		{ NULL,     NULL    }
	};
	WCHAR szCodecName[128];

	wsprintfW(szFcc, L"%C%C%C%C", FCC4PRINTF(fcc));
	StringFromGUID2(clsid, szClsID, _countof(szClsID));
	hr = ATL::_pAtlModule->UpdateRegistryFromResource(IDR_DMOENCODER, bRegister, regmap);
	if (FAILED(hr))
		return hr;

	if (bRegister)
	{
		CCodec *pCodec = CCodec::CreateInstance(fcc, "DMO");
		DMO_PARTIAL_MEDIATYPE *pInTypes;
		DMO_PARTIAL_MEDIATYPE *pOutTypes;
		DWORD cInTypes, cOutTypes;

		FormatInfoToPartialMediaType(pCodec->GetEncoderInputFormat(), &cInTypes, &pInTypes);
		FormatInfoToPartialMediaType(pCodec->GetCompressedFormat(), &cOutTypes, &pOutTypes);
		pCodec->GetLongFriendlyName(szCodecName, _countof(szCodecName));
		hr = DMORegister(szCodecName, clsid, DMOCATEGORY_VIDEO_ENCODER, 0, cInTypes, pInTypes, cOutTypes, pOutTypes);
		delete pInTypes;
		delete pOutTypes;
		return hr;
	}
	else
	{
		return DMOUnregister(clsid, DMOCATEGORY_VIDEO_ENCODER);
	}
}


// IMediaObjectImpl

HRESULT CDMOEncoder::InternalGetInputStreamInfo(DWORD dwInputStreamIndex, DWORD *pdwFlags)
{
	*pdwFlags = DMO_INPUT_STREAMF_WHOLE_SAMPLES |
	            DMO_INPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER |
	            DMO_INPUT_STREAMF_FIXED_SAMPLE_SIZE |
	            DMO_INPUT_STREAMF_HOLDS_BUFFERS;

	return S_OK;
}

HRESULT CDMOEncoder::InternalGetOutputStreamInfo(DWORD dwOutputStreamIndex, DWORD *pdwFlags)
{
	*pdwFlags = DMO_OUTPUT_STREAMF_WHOLE_SAMPLES |
	            DMO_OUTPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER;

	return S_OK;
}

HRESULT CDMOEncoder::InternalCheckInputType(DWORD dwInputStreamIndex, const DMO_MEDIA_TYPE *pmt)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalCheckInputType()\n");

	const FORMATINFO *pfi;
	const VIDEOINFOHEADER *pvih;

	if (!IsEqualGUID(pmt->majortype, MEDIATYPE_Video))
		return DMO_E_INVALIDTYPE;
	if (!pmt->bFixedSizeSamples)
		return DMO_E_INVALIDTYPE;
	if (pmt->bTemporalCompression)
		return DMO_E_INVALIDTYPE;
	if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
		return DMO_E_INVALIDTYPE;

	pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

	for (pfi = m_pCodec->GetEncoderInputFormat(); !IS_FORMATINFO_END(pfi); pfi++)
	{
		if (IsEqualGUID(pfi->guidMediaSubType, pmt->subtype))
		{
			if (m_pCodec->CompressQuery(&pvih->bmiHeader, NULL) == 0)
				return S_OK;
		}
	}

	return DMO_E_INVALIDTYPE;
}

HRESULT CDMOEncoder::InternalCheckOutputType(DWORD dwOutputStreamIndex, const DMO_MEDIA_TYPE *pmt)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalCheckOutputType()\n");

	const FORMATINFO *pfi;
	const DMO_MEDIA_TYPE *pmtIn;
	const VIDEOINFOHEADER *pvih;
	const VIDEOINFOHEADER *pvihIn;

	if (!InputTypeSet(0))
		return DMO_E_INVALIDTYPE;
	pmtIn = InputType(0);
	pvihIn = (const VIDEOINFOHEADER *)pmtIn->pbFormat;

	if (!IsEqualGUID(pmt->majortype, MEDIATYPE_Video))
		return DMO_E_INVALIDTYPE;
	if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
		return DMO_E_INVALIDTYPE;

	pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

	pfi = m_pCodec->GetCompressedFormat();
	if (!IsEqualGUID(pfi->guidMediaSubType, pmt->subtype))
		return DMO_E_INVALIDTYPE;
	if (m_pCodec->CompressQuery(&pvihIn->bmiHeader, &pvih->bmiHeader) != 0)
		return DMO_E_INVALIDTYPE;

	return S_OK;
}

HRESULT CDMOEncoder::InternalGetInputType(DWORD dwInputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalGetInputType()\n");

	const FORMATINFO *pfi = m_pCodec->GetEncoderInputFormat();

	while (dwTypeIndex > 0 && !IS_FORMATINFO_END(pfi))
	{
		pfi++;
		dwTypeIndex--;
	}

	if (IS_FORMATINFO_END(pfi))
		return DMO_E_NO_MORE_ITEMS;

	if (pmt != NULL)
	{
		memset(pmt, 0, sizeof(DMO_MEDIA_TYPE));
		pmt->majortype            = MEDIATYPE_Video;
		pmt->subtype              = pfi->guidMediaSubType;
		pmt->bFixedSizeSamples    = TRUE;
		pmt->bTemporalCompression = FALSE;
	}

	return S_OK;
}

HRESULT CDMOEncoder::InternalGetOutputType(DWORD dwOutputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalGetOutputType()\n");

	const FORMATINFO *pfi = m_pCodec->GetCompressedFormat();

	if (dwTypeIndex != 0)
		return DMO_E_NO_MORE_ITEMS;

	memset(pmt, 0, sizeof(DMO_MEDIA_TYPE));
	pmt->majortype            = MEDIATYPE_Video;
	pmt->subtype              = pfi->guidMediaSubType;
	pmt->bFixedSizeSamples    = FALSE;
	pmt->bTemporalCompression = m_pCodec->IsTemporalCompressionSupported();

	if (InputTypeSet(0))
	{
		const DMO_MEDIA_TYPE *pmtIn = InputType(0);
		const VIDEOINFOHEADER *pvihIn = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
		VIDEOINFOHEADER *pvih;
		DWORD biSize;

		biSize = m_pCodec->CompressGetFormat(&pvihIn->bmiHeader, NULL);
		MoInitMediaType(pmt, sizeof(VIDEOINFOHEADER) - sizeof(BITMAPINFOHEADER) + biSize);
		pvih = (VIDEOINFOHEADER *)pmt->pbFormat;
		memcpy(pvih, pvihIn, sizeof(VIDEOINFOHEADER) - sizeof(BITMAPINFOHEADER));
		m_pCodec->CompressGetFormat(&pvihIn->bmiHeader, &pvih->bmiHeader);
		pmt->formattype = FORMAT_VideoInfo;
	}

	return S_OK;
}

HRESULT CDMOEncoder::InternalGetInputSizeInfo(DWORD dwInputStreamIndex, DWORD *pcbSize, DWORD *pcbMaxLookahead, DWORD *pcbAlignment)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalGetInputSizeInfo()\n");

	*pcbSize = 0;
	*pcbMaxLookahead = 0;
	*pcbAlignment = 4;

	return S_OK;
}

HRESULT CDMOEncoder::InternalGetOutputSizeInfo(DWORD dwOutputStreamIndex, DWORD *pcbSize, DWORD *pcbAlignment)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalGetOutputSizeInfo()\n");

	const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
	const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;

	*pcbSize = m_pCodec->CompressGetSize(&pvihIn->bmiHeader, &pvihOut->bmiHeader);
	*pcbAlignment = 4;

	return S_OK;
}

HRESULT CDMOEncoder::InternalGetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME *prtMaxLatency)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalGetInputMaxLatency()\n");

	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalSetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME rtMaxLatency)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalSetInputMaxLatency()\n");

	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalFlush()
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalFlush()\n");

	if (m_pInputBuffer != NULL)
	{
		m_pInputBuffer->Release();
		m_pInputBuffer = NULL;
	}

	return S_OK;
}

HRESULT CDMOEncoder::InternalDiscontinuity(DWORD dwInputStreamIndex)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalDiscontinuity()\n");

	return S_OK;
}

HRESULT CDMOEncoder::InternalAllocateStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalAllocateStreamingResources()\n");

	const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
	const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
	const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
	const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;

	if (m_pCodec->CompressBegin(&pvihIn->bmiHeader, &pvihOut->bmiHeader) == 0)
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CDMOEncoder::InternalFreeStreamingResources()
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalFreeStreamingResources()\n");

	m_pCodec->CompressEnd();

	return S_OK;
}

HRESULT CDMOEncoder::InternalProcessInput(DWORD dwInputStreamIndex, IMediaBuffer *pBuffer, DWORD dwFlags, REFERENCE_TIME rtTimestamp, REFERENCE_TIME rtTimelength)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalProcessInput()\n");

	m_pInputBuffer = pBuffer;
	m_pInputBuffer->AddRef();
	m_bInputKeyFrame = dwFlags & DMO_INPUT_DATA_BUFFERF_SYNCPOINT;
	m_bInputTimestampValid = dwFlags & DMO_INPUT_DATA_BUFFERF_TIME;
	m_bInputTimelengthValid = dwFlags & DMO_INPUT_DATA_BUFFERF_TIMELENGTH;
	m_rtInputTimestamp = rtTimestamp;
	m_rtInputTimelength = rtTimelength;

	return S_OK;
}

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

HRESULT CDMOEncoder::InternalAcceptingInput(DWORD dwInputStreamIndex)
{
	_RPT0(_CRT_WARN, "CDMOEncoder::InternalAcceptingInput()\n");

	if (m_pInputBuffer == NULL)
		return S_OK;
	else
		return S_FALSE;
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
