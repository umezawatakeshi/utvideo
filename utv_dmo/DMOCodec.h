/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// DMOCodec.h : CDMOCodec の宣言

#pragma once
#include "resource.h"       // メイン シンボル

#include "utvideo.h"
#include "utv_dmo.h"
//#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"
#include <Format.h>
#include <LogWriter.h>
#include <LogUtil.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif

void FormatInfoToPartialMediaType(const utvf_t *putvf, DWORD *pcTypes, DMO_PARTIAL_MEDIATYPE **ppTypes);


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
		if (IsLogWriterInitializedOrDebugBuild())
		{
			OLECHAR szClsId[48];
			StringFromGUID2(clsid, szClsId, _countof(szClsId));
			LOGPRINTF("%p CDMOCodec::CDMOCodec(fcc=%08X, clsid=%S)", this, fcc, szClsId);
		}

		utvf_t utvf;

		VCMFormatToUtVideoFormat(&utvf, fcc, 0);
		m_pCodec = CCodec::CreateInstance(utvf, "DMO");
		m_pInputBuffer = NULL;

		LOGPRINTF("%p  m_pCodec=%p, TinyName=\"%s\"", this, m_pCodec, m_pCodec->GetTinyName());
	}

	virtual ~CDMOCodec()
	{
		LOGPRINTF("%p CDMOCodec::~CDMOCodec()", this);

		FreeStreamingResources();

		if (m_pInputBuffer != NULL)
			m_pInputBuffer->Release();
		CCodec::DeleteInstance(m_pCodec);
	}

	static HRESULT WINAPI UpdateRegistry(DWORD fcc, REFCLSID clsid, BOOL bRegister)
	{
		if (IsLogWriterInitializedOrDebugBuild())
		{
			OLECHAR szClsId[48];
			StringFromGUID2(clsid, szClsId, _countof(szClsId));
			LOGPRINTF("CDMOCodec::UpdateRegistry(fcc=%08X, clsid=%S, bRegister=%s)", fcc, szClsId, bRegister ? "true" : "false");
		}

		HRESULT hr;
		OLECHAR szFcc[5];
		OLECHAR szClsID[64];
		_ATL_REGMAP_ENTRY regmap[3] = {
			{ L"FCC",   szFcc   },
			{ L"CLSID", szClsID },
			{ NULL,     NULL    }
		};
		WCHAR szCodecName[128];

#define FCC4PRINTF(fcc) \
	(uint8_t)(fcc), \
	(uint8_t)(fcc >> 8), \
	(uint8_t)(fcc >> 16), \
	(uint8_t)(fcc >> 24)

		wsprintfW(szFcc, L"%C%C%C%C", FCC4PRINTF(fcc));
		StringFromGUID2(clsid, szClsID, _countof(szClsID));
		hr = ATL::_pAtlModule->UpdateRegistryFromResource(T::IDR, bRegister, regmap);
		if (FAILED(hr))
			return hr;

		if (bRegister)
		{
			CCodec *pCodec;
			DMO_PARTIAL_MEDIATYPE *pInTypes;
			DMO_PARTIAL_MEDIATYPE *pOutTypes;
			DWORD cInTypes, cOutTypes;
			utvf_t utvf;

			VCMFormatToUtVideoFormat(&utvf, fcc, 0);
			pCodec = CCodec::CreateInstance(utvf, "DMO");
			FormatInfoToPartialMediaType(T::GetInputFormatInfo(pCodec), &cInTypes, &pInTypes);
			FormatInfoToPartialMediaType(T::GetOutputFormatInfo(pCodec), &cOutTypes, &pOutTypes);
			T::GetName(pCodec, szCodecName, _countof(szCodecName));
			hr = DMORegister(szCodecName, clsid, T::DMOCATEGORY, 0, cInTypes, pInTypes, cOutTypes, pOutTypes);
			delete pInTypes;
			delete pOutTypes;
			CCodec::DeleteInstance(pCodec);
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

#define LOGPRINT_BIH_THIS(_this, prefix, pbih) \
	LOGPRINTF("%p %s: biSize=%d, biWidth=%d, biHeight=%d, biPlanes=%d, biBitCount=%d, biCompression=%08X, biSizeImage=%d", \
		(_this), (prefix), (pbih)->biSize, (pbih)->biWidth, (pbih)->biHeight, (pbih)->biPlanes, (pbih)->biBitCount, (pbih)->biCompression, (pbih)->biSizeImage);
#define LOGPRINT_DMT_THIS(_this, prefix, pmt) \
	do \
	{ \
		OLECHAR szMajorType[48]; \
		OLECHAR szSubType[48]; \
		OLECHAR szFormatType[48]; \
		StringFromGUID2((pmt)->majortype, szMajorType, _countof(szMajorType)); \
		StringFromGUID2((pmt)->subtype, szSubType, _countof(szSubType)); \
		StringFromGUID2((pmt)->formattype, szFormatType, _countof(szFormatType)); \
		LOGPRINTF("%p %s: majortype=%S, subtype=%S, formattype=%S, pbFormat=%p", \
			(_this), (prefix), szMajorType, szSubType, szFormatType, (pmt)->pbFormat); \
	} while (false)

	HRESULT InternalCheckInputType(DWORD dwInputStreamIndex, const DMO_MEDIA_TYPE *pmt)
	{
		if (IsLogWriterInitializedOrDebugBuild())
		{
			LOGPRINTF("%p CDMOCodec::InternalCheckInputType(dwInputStreamIndex=%d, pmt=%p)", this, dwInputStreamIndex, pmt);
			if (pmt != NULL)
			{
				LOGPRINT_DMT_THIS(this, " pmt", pmt);
				if (IsEqualGUID(pmt->majortype, MEDIATYPE_Video) && IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
				{
					const VIDEOINFOHEADER *pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;
					LOGPRINT_BIH_THIS(this, " pmt->pbFormat", &pvih->bmiHeader);
				}
			}
		}

		const VIDEOINFOHEADER *pvih;
		utvf_t infmt;

		if (!IsEqualGUID(pmt->majortype, MEDIATYPE_Video))
			return DMO_E_INVALIDTYPE;
		if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
			return DMO_E_INVALIDTYPE;

		pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

		if (DirectShowFormatToUtVideoFormat(&infmt, pvih->bmiHeader.biCompression, pvih->bmiHeader.biBitCount, pmt->subtype) != 0)
			return DMO_E_INVALIDTYPE;

		if (pvih->bmiHeader.biHeight < 0)
			return DMO_E_INVALIDTYPE;

		if (((T *)this)->Query(UTVF_INVALID, infmt,
				pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight,
				NULL, 0,
				((const BYTE *)&pvih->bmiHeader) + sizeof(BITMAPINFOHEADER), pvih->bmiHeader.biSize - sizeof(BITMAPINFOHEADER)) != 0)
		{
			return DMO_E_INVALIDTYPE;
		}

		return S_OK;
	}

	HRESULT InternalCheckOutputType(DWORD dwOutputStreamIndex, const DMO_MEDIA_TYPE *pmt)
	{
		if (IsLogWriterInitializedOrDebugBuild())
		{
			LOGPRINTF("%p CDMOCodec::InternalCheckOutputType(dwOutputStreamIndex=%d, pmt=%p)", this, dwOutputStreamIndex, pmt);
			if (pmt != NULL)
			{
				LOGPRINT_DMT_THIS(this, " pmt", pmt);
				if (IsEqualGUID(pmt->majortype, MEDIATYPE_Video) && IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
				{
					const VIDEOINFOHEADER *pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;
					LOGPRINT_BIH_THIS(this, " pmt->pbFormat", &pvih->bmiHeader);
				}
			}
		}

		const DMO_MEDIA_TYPE *pmtIn;
		const VIDEOINFOHEADER *pvih;
		const VIDEOINFOHEADER *pvihIn;
		utvf_t infmt;
		utvf_t outfmt;

		if (!InputTypeSet(0))
			return DMO_E_INVALIDTYPE;
		pmtIn = InputType(0);
		pvihIn = (const VIDEOINFOHEADER *)pmtIn->pbFormat;

		if (!IsEqualGUID(pmt->majortype, MEDIATYPE_Video))
			return DMO_E_INVALIDTYPE;
		if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
			return DMO_E_INVALIDTYPE;

		pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

		if (DirectShowFormatToUtVideoFormat(&infmt, pvihIn->bmiHeader.biCompression, pvihIn->bmiHeader.biBitCount, pmtIn->subtype) != 0)
			return DMO_E_INVALIDTYPE;
		if (DirectShowFormatToUtVideoFormat(&outfmt, pvih->bmiHeader.biCompression, pvih->bmiHeader.biBitCount, pmt->subtype) != 0)
			return DMO_E_INVALIDTYPE;

		if (pvih->bmiHeader.biWidth != pvihIn->bmiHeader.biWidth || pvih->bmiHeader.biHeight != pvihIn->bmiHeader.biHeight)
			return DMO_E_INVALIDTYPE;

		if (((T *)this)->Query(outfmt, infmt,
				pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight,
				((const BYTE *)&pvih->bmiHeader) + sizeof(BITMAPINFOHEADER), pvih->bmiHeader.biSize - sizeof(BITMAPINFOHEADER),
				((const BYTE *)&pvihIn->bmiHeader) + sizeof(BITMAPINFOHEADER), pvihIn->bmiHeader.biSize - sizeof(BITMAPINFOHEADER)) != 0)
		{
			return DMO_E_INVALIDTYPE;
		}

		return S_OK;
	}

	HRESULT InternalGetInputType(DWORD dwInputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
	{
		LOGPRINTF("%p CDMOCodec::InternalGetInputType(dwInputStreamIndex=%d, dwTypeIndex=%d, pmt=%p)", this, dwInputStreamIndex, dwTypeIndex, pmt);

		const utvf_t *putvf = T::GetInputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return DMO_E_NO_MORE_ITEMS;
			if (UtVideoFormatToDirectShowFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		if (pmt == NULL)
			return S_OK;

		memset(pmt, 0, sizeof(DMO_MEDIA_TYPE));
		pmt->majortype            = MEDIATYPE_Video;
		pmt->subtype              = subtype;
		pmt->bFixedSizeSamples    = T::bEncoding;
		pmt->bTemporalCompression = m_pCodec->IsTemporalCompressionSupported() && !T::bEncoding;

		return S_OK;
	}

	HRESULT InternalGetOutputType(DWORD dwOutputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt)
	{
		LOGPRINTF("%p CDMOCodec::InternalGetOutputType(dwOutputStreamIndex=%d, dwTypeIndex=%d, pmt=%p)", this, dwOutputStreamIndex, dwTypeIndex, pmt);

		const utvf_t *putvf = T::GetOutputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return DMO_E_NO_MORE_ITEMS;
			if (UtVideoFormatToDirectShowFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		if (pmt == NULL)
			return S_OK;

		memset(pmt, 0, sizeof(DMO_MEDIA_TYPE));
		pmt->majortype            = MEDIATYPE_Video;
		pmt->subtype              = subtype;
		pmt->bFixedSizeSamples    = !T::bEncoding;
		pmt->bTemporalCompression = m_pCodec->IsTemporalCompressionSupported() && T::bEncoding;

		if (InputTypeSet(0))
		{
			const DMO_MEDIA_TYPE *pmtIn = InputType(0);
			const VIDEOINFOHEADER *pvihIn = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
			VIDEOINFOHEADER *pvih;
			size_t cbExtraData;
			utvf_t infmt;
			utvf_t outfmt = *putvf;

			DirectShowFormatToUtVideoFormat(&infmt, pvihIn->bmiHeader.biCompression, pvihIn->bmiHeader.biBitCount, pmtIn->subtype);

			cbExtraData = ((T *)this)->GetExtraDataSize();
			MoInitMediaType(pmt, (DWORD)(sizeof(VIDEOINFOHEADER) + cbExtraData));
			pmt->formattype = FORMAT_VideoInfo;
			pvih = (VIDEOINFOHEADER *)pmt->pbFormat;
			memcpy(pvih, pvihIn, sizeof(VIDEOINFOHEADER));
			UtVideoFormatToVCMFormat(&pvih->bmiHeader.biCompression, &pvih->bmiHeader.biBitCount, *putvf);
			pvih->bmiHeader.biSizeImage = (DWORD)((T *)this)->GetSize(outfmt, infmt, pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight);
			pvih->bmiHeader.biSize = (DWORD)(sizeof(BITMAPINFOHEADER) + cbExtraData);
			((T *)this)->GetExtraData(((BYTE *)&pvih->bmiHeader) + sizeof(BITMAPINFOHEADER), cbExtraData, outfmt, infmt, pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight);
		}

		return S_OK;
	}

	HRESULT InternalGetInputSizeInfo(DWORD dwInputStreamIndex, DWORD *pcbSize, DWORD *pcbMaxLookahead, DWORD *pcbAlignment)
	{
		LOGPRINTF("%p CDMOCodec::InternalGetOutputSizeInfo(dwInputStreamIndex=%d, pcbSize=%p, pcbMaxLookahead=%p, pcbAlignment=%p)", this, dwInputStreamIndex, pcbSize, pcbMaxLookahead, pcbAlignment);

		*pcbSize = 0;
		*pcbMaxLookahead = 0;
		*pcbAlignment = 4;

		return S_OK;
	}

	HRESULT InternalGetOutputSizeInfo(DWORD dwOutputStreamIndex, DWORD *pcbSize, DWORD *pcbAlignment)
	{
		LOGPRINTF("%p CDMOCodec::InternalGetOutputSizeInfo(dwOutputStreamIndex=%d, pcbSize=%p, pcbAlignment=%p)", this, dwOutputStreamIndex, pcbSize, pcbAlignment);

		const DMO_MEDIA_TYPE *pmtIn  = InputType(0);
		const DMO_MEDIA_TYPE *pmtOut = OutputType(0);
		const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
		const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;
		utvf_t infmt;
		utvf_t outfmt;

		if (DirectShowFormatToUtVideoFormat(&infmt, pvihIn->bmiHeader.biCompression, pvihIn->bmiHeader.biBitCount, pmtIn->subtype) != 0)
			return DMO_E_INVALIDTYPE;
		if (DirectShowFormatToUtVideoFormat(&outfmt, pvihOut->bmiHeader.biCompression, pvihOut->bmiHeader.biBitCount, pmtOut->subtype) != 0)
			return DMO_E_INVALIDTYPE;

		*pcbSize = (DWORD)((T *)this)->GetSize(outfmt, infmt, pvihIn->bmiHeader.biWidth, pvihIn->bmiHeader.biHeight);
		*pcbAlignment = 4;

		return S_OK;
	}

	HRESULT InternalGetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME *prtMaxLatency)
	{
		return E_NOTIMPL;
	}

	HRESULT InternalSetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME rtMaxLatency)
	{
		return E_NOTIMPL;
	}

	HRESULT InternalFlush()
	{
		LOGPRINTF("%p CDMOCodec::InternalFlush()", this);

		if (m_pInputBuffer != NULL)
		{
			m_pInputBuffer->Release();
			m_pInputBuffer = NULL;
		}

		return S_OK;
	}

	HRESULT InternalDiscontinuity(DWORD dwInputStreamIndex)
	{
		LOGPRINTF("%p CDMOCodec::InternalDiscontinuity(dwInputStreamIndex=%d)", this, dwInputStreamIndex);

		return S_OK;
	}

	//HRESULT InternalAllocateStreamingResources();

	//HRESULT InternalFreeStreamingResources();

	HRESULT InternalProcessInput(DWORD dwInputStreamIndex, IMediaBuffer *pBuffer, DWORD dwFlags, REFERENCE_TIME rtTimestamp, REFERENCE_TIME rtTimelength)
	{
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
		if (m_pInputBuffer == NULL)
			return S_OK;
		else
			return S_FALSE;
	}
};
