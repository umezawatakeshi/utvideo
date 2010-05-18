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

CDMOEncoder::CDMOEncoder(DWORD fcc)
{
}

CDMOEncoder::~CDMOEncoder()
{
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
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalCheckOutputType(DWORD dwOutputStreamIndex, const DMO_MEDIA_TYPE *pmt)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalGetInputType(DWORD dwInputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalGetOutputType(DWORD dwOutputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalGetInputSizeInfo(DWORD dwInputStreamIndex, DWORD *pcbSize, DWORD *pcbMaxLookahead, DWORD *pcbAlignment)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalGetOutputSizeInfo(DWORD dwOutputStreamIndex, DWORD *pcbSize, DWORD *pcbAlignment)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalGetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME *prtMaxLatency)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalSetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME rtMaxLatency)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalFlush()
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalDiscontinuity(DWORD dwInputStreamIndex)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalAllocateStreamingResources()
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalFreeStreamingResources()
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalProcessInput(DWORD dwInputStreamIndex, IMediaBuffer *pBuffer, DWORD dwFlags, REFERENCE_TIME rtTimestamp, REFERENCE_TIME rtTimelength)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus)
{
	return E_NOTIMPL;
}

HRESULT CDMOEncoder::InternalAcceptingInput(DWORD dwInputStreamIndex)
{
	return E_NOTIMPL;
}
