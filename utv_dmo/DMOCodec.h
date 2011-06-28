/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// DMOCodec.h : CDMOCodec の宣言

#pragma once
#include "resource.h"       // メイン シンボル

#include "utv_dmo.h"
//#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"

#define FCC4PRINTF(fcc) \
	(BYTE)(fcc), \
	(BYTE)(fcc >> 8), \
	(BYTE)(fcc >> 16), \
	(BYTE)(fcc >> 24)


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif



// CDMOCodec

template<class T>
class ATL_NO_VTABLE CDMOCodec :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IMediaObjectImpl<T, 1, 1>
{
protected:
	const DWORD m_fcc;
	const CLSID m_clsid;
	CCodec *m_pCodec;
	IMediaBuffer *m_pInputBuffer;
	BOOL m_bInputKeyFrame;
	BOOL m_bInputTimestampValid;
	BOOL m_bInputTimelengthValid;
	REFERENCE_TIME m_rtInputTimestamp;
	REFERENCE_TIME m_rtInputTimelength;

public:
	CDMOCodec(DWORD fcc, REFCLSID clsid) :
		m_fcc(fcc), m_clsid(clsid)
	{
		m_pCodec = CCodec::CreateInstance(fcc, "DMO");
		m_pInputBuffer = NULL;
	}

	virtual ~CDMOCodec()
	{
		FreeStreamingResources();

		if (m_pInputBuffer != NULL)
			m_pInputBuffer->Release();
		delete m_pCodec;
	}

	static HRESULT WINAPI UpdateRegistry(DWORD fcc, REFCLSID clsid, BOOL bRegister)
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
		hr = ATL::_pAtlModule->UpdateRegistryFromResource(T::IDR, bRegister, regmap);
		if (FAILED(hr))
			return hr;

		if (bRegister)
		{
			CCodec *pCodec = CCodec::CreateInstance(fcc, "DMO");
			DMO_PARTIAL_MEDIATYPE *pInTypes;
			DMO_PARTIAL_MEDIATYPE *pOutTypes;
			DWORD cInTypes, cOutTypes;

			FormatInfoToPartialMediaType(T::GetInputFormatInfo(pCodec), &cInTypes, &pInTypes);
			FormatInfoToPartialMediaType(T::GetOutputFormatInfo(pCodec), &cOutTypes, &pOutTypes);
			T::GetName(pCodec, szCodecName, _countof(szCodecName));
			hr = DMORegister(szCodecName, clsid, T::DMOCATEGORY, 0, cInTypes, pInTypes, cOutTypes, pOutTypes);
			delete pInTypes;
			delete pOutTypes;
			return hr;
		}
		else
		{
			return DMOUnregister(clsid, T::DMOCATEGORY);
		}
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// IMediaObjectImpl

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


	// IMediaObjectImpl

	HRESULT InternalGetInputStreamInfo(DWORD dwInputStreamIndex, DWORD *pdwFlags)
	{
		*pdwFlags = DMO_INPUT_STREAMF_WHOLE_SAMPLES |
					DMO_INPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER |
					(T::bEncoding ? DMO_INPUT_STREAMF_FIXED_SAMPLE_SIZE : 0);

		return S_OK;
	}

	HRESULT InternalGetOutputStreamInfo(DWORD dwOutputStreamIndex, DWORD *pdwFlags)
	{
		*pdwFlags = DMO_OUTPUT_STREAMF_WHOLE_SAMPLES |
					DMO_OUTPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER |
					(T::bEncoding ? 0 : DMO_OUTPUT_STREAMF_FIXED_SAMPLE_SIZE);

		return S_OK;
	}

	HRESULT InternalCheckInputType(DWORD dwInputStreamIndex, const DMO_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalCheckInputType()\n");

		const FORMATINFO *pfi;
		const VIDEOINFOHEADER *pvih;

		if (!IsEqualGUID(pmt->majortype, MEDIATYPE_Video))
			return DMO_E_INVALIDTYPE;
		if (T::bEncoding ? !pmt->bFixedSizeSamples : pmt->bFixedSizeSamples)
			return DMO_E_INVALIDTYPE;
		if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
			return DMO_E_INVALIDTYPE;

		pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

		for (pfi = T::GetInputFormatInfo(m_pCodec); !IS_FORMATINFO_END(pfi); pfi++)
		{
			if (IsEqualGUID(pfi->guidMediaSubType, pmt->subtype) &&
				((T *)this)->Query(&pvih->bmiHeader, NULL) == 0 &&
				(pmt->bTemporalCompression && pfi->bTemporalCompression ||
					!pmt->bTemporalCompression && !pfi->bTemporalCompression))
			{
					return S_OK;
			}
		}

		return DMO_E_INVALIDTYPE;
	}

	HRESULT InternalCheckOutputType(DWORD dwOutputStreamIndex, const DMO_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalCheckOutputType()\n");

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
		if (T::bEncoding ? pmt->bFixedSizeSamples : !pmt->bFixedSizeSamples)
			return DMO_E_INVALIDTYPE;
		if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
			return DMO_E_INVALIDTYPE;

		pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

		for (pfi = T::GetOutputFormatInfo(m_pCodec); !IS_FORMATINFO_END(pfi); pfi++)
		{
			if (IsEqualGUID(pfi->guidMediaSubType, pmt->subtype) &&
				pvih->bmiHeader.biCompression == pfi->fcc &&
				pvih->bmiHeader.biBitCount == pfi->nBitCount &&
				((T *)this)->Query(&pvihIn->bmiHeader, &pvih->bmiHeader) == 0 &&
				(pmt->bTemporalCompression && pfi->bTemporalCompression ||
					!pmt->bTemporalCompression && !pfi->bTemporalCompression))
			{
					return S_OK;
			}
		}

		return DMO_E_INVALIDTYPE;
	}

	HRESULT InternalGetInputType(DWORD dwInputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalGetInputType()\n");

		const FORMATINFO *pfi = T::GetInputFormatInfo(m_pCodec);

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
			pmt->bFixedSizeSamples    = T::bEncoding;
			pmt->bTemporalCompression = pfi->bTemporalCompression;
		}

		return S_OK;
	}

	HRESULT InternalGetOutputType(DWORD dwOutputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalGetOutputType()\n");

		const FORMATINFO *pfi = T::GetOutputFormatInfo(m_pCodec);

		while (dwTypeIndex > 0 && !IS_FORMATINFO_END(pfi))
		{
			pfi++;
			dwTypeIndex--;
		}

		if (IS_FORMATINFO_END(pfi))
			return DMO_E_NO_MORE_ITEMS;

		memset(pmt, 0, sizeof(DMO_MEDIA_TYPE));
		pmt->majortype            = MEDIATYPE_Video;
		pmt->subtype              = pfi->guidMediaSubType;
		pmt->bFixedSizeSamples    = !T::bEncoding;

		if (InputTypeSet(0))
		{
			const DMO_MEDIA_TYPE *pmtIn = InputType(0);
			const VIDEOINFOHEADER *pvihIn = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
			VIDEOINFOHEADER *pvih;
			DWORD biSize;

			biSize = ((T *)this)->GetFormat(&pvihIn->bmiHeader, NULL, pfi);
			MoInitMediaType(pmt, sizeof(VIDEOINFOHEADER) - sizeof(BITMAPINFOHEADER) + biSize);
			pvih = (VIDEOINFOHEADER *)pmt->pbFormat;
			memcpy(pvih, pvihIn, sizeof(VIDEOINFOHEADER) - sizeof(BITMAPINFOHEADER));
			((T *)this)->GetFormat(&pvihIn->bmiHeader, &pvih->bmiHeader, pfi);
			pmt->formattype = FORMAT_VideoInfo;
			pmt->bTemporalCompression = pfi->bTemporalCompression;
		}

		return S_OK;
	}

	HRESULT InternalGetInputSizeInfo(DWORD dwInputStreamIndex, DWORD *pcbSize, DWORD *pcbMaxLookahead, DWORD *pcbAlignment)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalGetInputSizeInfo()\n");

		*pcbSize = 0;
		*pcbMaxLookahead = 0;
		*pcbAlignment = 4;

		return S_OK;
	}

	HRESULT InternalGetOutputSizeInfo(DWORD dwOutputStreamIndex, DWORD *pcbSize, DWORD *pcbAlignment)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalGetOutputSizeInfo()\n");

		const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
		const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
		const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
		const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;

		*pcbSize = ((T *)this)->GetSize(&pvihIn->bmiHeader, &pvihOut->bmiHeader);
		*pcbAlignment = 4;

		return S_OK;
	}

	HRESULT InternalGetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME *prtMaxLatency)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalGetInputMaxLatency()\n");

		return E_NOTIMPL;
	}

	HRESULT InternalSetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME rtMaxLatency)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalSetInputMaxLatency()\n");

		return E_NOTIMPL;
	}

	HRESULT InternalFlush()
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalFlush()\n");

		if (m_pInputBuffer != NULL)
		{
			m_pInputBuffer->Release();
			m_pInputBuffer = NULL;
		}

		return S_OK;
	}

	HRESULT InternalDiscontinuity(DWORD dwInputStreamIndex)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalDiscontinuity()\n");

		return S_OK;
	}

	//HRESULT InternalAllocateStreamingResources();

	//HRESULT InternalFreeStreamingResources();

	HRESULT InternalProcessInput(DWORD dwInputStreamIndex, IMediaBuffer *pBuffer, DWORD dwFlags, REFERENCE_TIME rtTimestamp, REFERENCE_TIME rtTimelength)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalProcessInput()\n");

		m_pInputBuffer = pBuffer;
		m_pInputBuffer->AddRef();
		m_bInputKeyFrame = dwFlags & DMO_INPUT_DATA_BUFFERF_SYNCPOINT;
		m_bInputTimestampValid = dwFlags & DMO_INPUT_DATA_BUFFERF_TIME;
		m_bInputTimelengthValid = dwFlags & DMO_INPUT_DATA_BUFFERF_TIMELENGTH;
		m_rtInputTimestamp = rtTimestamp;
		m_rtInputTimelength = rtTimelength;

		return S_OK;
	}

	//HRESULT InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus);

	HRESULT InternalAcceptingInput(DWORD dwInputStreamIndex)
	{
		_RPT0(_CRT_WARN, "CDMOCodec::InternalAcceptingInput()\n");

		if (m_pInputBuffer == NULL)
			return S_OK;
		else
			return S_FALSE;
	}
};
