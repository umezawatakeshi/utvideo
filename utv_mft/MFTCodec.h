/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// MFTCodec.h : CMFTCodec の宣言

#pragma once
#include "resource.h"       // メイン シンボル

#include "utvideo.h"
#include "utv_mft.h"
//#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"
#include <Format.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif

void FormatInfoToRegisterTypeInfo(const utvf_t *putvf, UINT32 *pcTypes, MFT_REGISTER_TYPE_INFO **ppTypes);

class LockAttr
{
private:
	IMFAttributes *m_p;
public:
	LockAttr(IMFAttributes *p) : m_p(p)
	{
		m_p->LockStore();
	}
	~LockAttr()
	{
		m_p->UnlockStore();
	}
};

// CMFTCodec

template<class T>
class ATL_NO_VTABLE CMFTCodec :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IMFTransform
{
protected:
	class LockIt
	{
	private:
		T *const m_p;
	public:
		LockIt(T *p) : m_p(p)
		{
			m_p->Lock();
		}
		~LockIt()
		{
			m_p->Unlock();
		}
	};

protected:
	const DWORD m_fcc;
	const CLSID m_clsid;
	CCodec *m_pCodec;
	IMFSample *m_pInputSample;
	IMFMediaType *m_pInputMediaType;
	IMFMediaType *m_pOutputMediaType;
	bool m_bStreamBegin;

public:
	CMFTCodec(DWORD fcc, REFCLSID clsid) :
		m_fcc(fcc), m_clsid(clsid)
	{
		utvf_t utvf;

		VCMFormatToUtVideoFormat(&utvf, fcc, 0);
		m_pCodec = CCodec::CreateInstance(utvf, "MFT");
		m_pInputSample = NULL;
		m_pInputMediaType = NULL;
		m_pOutputMediaType = NULL;
		m_bStreamBegin = false;
	}

	virtual ~CMFTCodec()
	{
		EndStream();

		if (m_pInputSample != NULL)
			m_pInputSample->Release();
		if (m_pInputMediaType != NULL)
			m_pInputMediaType->Release();
		if (m_pOutputMediaType != NULL)
			m_pOutputMediaType->Release();
		CCodec::DeleteInstance(m_pCodec);
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
			MFT_REGISTER_TYPE_INFO *pInTypes;
			MFT_REGISTER_TYPE_INFO *pOutTypes;
			UINT32 cInTypes, cOutTypes;
			utvf_t utvf;

			VCMFormatToUtVideoFormat(&utvf, fcc, 0);
			pCodec = CCodec::CreateInstance(utvf, "MFT");
			FormatInfoToRegisterTypeInfo(T::GetInputFormatInfo(pCodec), &cInTypes, &pInTypes);
			FormatInfoToRegisterTypeInfo(T::GetOutputFormatInfo(pCodec), &cOutTypes, &pOutTypes);
			T::GetName(pCodec, szCodecName, _countof(szCodecName));
			hr = MFTRegister(clsid, T::MFTCATEGORY, szCodecName, 0, cInTypes, pInTypes, cOutTypes, pOutTypes, NULL);
			delete pInTypes;
			delete pOutTypes;
			CCodec::DeleteInstance(pCodec);
			return hr;
		}
		else
		{
			return MFTUnregister(clsid);
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
	// IMFTransform

	HRESULT STDMETHODCALLTYPE GetStreamLimits(DWORD *pdwInputMinimum, DWORD *pdwInputMaximum, DWORD *pdwOutputMinimum, DWORD *pdwOutputMaximum)
	{
		*pdwInputMinimum = 1;
		*pdwInputMaximum = 1;
		*pdwOutputMinimum = 1;
		*pdwOutputMaximum = 1;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetStreamCount(DWORD *pcInputStreams, DWORD *pcOutputStreams)
	{
		*pcInputStreams = 1;
		*pcOutputStreams = 1;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetStreamIDs(DWORD dwInputIDArraySize, DWORD *pdwInputIDs, DWORD dwOutputIDArraySize, DWORD *pdwOutputIDs)
	{
		return E_NOTIMPL; // default ID assignment
	}

	HRESULT STDMETHODCALLTYPE GetInputStreamInfo(DWORD dwInputStreamID, MFT_INPUT_STREAM_INFO *pStreamInfo)
	{
		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		pStreamInfo->hnsMaxLatency = 0;
		pStreamInfo->dwFlags = MFT_INPUT_STREAM_WHOLE_SAMPLES | MFT_INPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER;
		pStreamInfo->cbSize = 0;
		pStreamInfo->cbMaxLookahead = 0;
		pStreamInfo->cbAlignment = 4;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputStreamInfo(DWORD dwOutputStreamID, MFT_OUTPUT_STREAM_INFO *pStreamInfo)
	{
		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		pStreamInfo->dwFlags = MFT_OUTPUT_STREAM_WHOLE_SAMPLES | MFT_OUTPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER | MFT_OUTPUT_STREAM_PROVIDES_SAMPLES;
		pStreamInfo->cbSize = 0;
		pStreamInfo->cbAlignment = 4;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetAttributes(IMFAttributes **pAttributes)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetAttributes()\n");
		return E_NOTIMPL; // XXX?
	}

	HRESULT STDMETHODCALLTYPE GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes **pAttributes)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetInputStreamAttributes()\n");
		return E_NOTIMPL; // XXX?
	}

	HRESULT STDMETHODCALLTYPE GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes **pAttributes)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetOutputStreamAttributes()\n");
		return E_NOTIMPL; // XXX?
	}

	HRESULT STDMETHODCALLTYPE DeleteInputStream(DWORD dwStreamID)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AddInputStreams(DWORD cStreams, DWORD *adwStreamIDs)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetInputAvailableType(DWORD dwInputStreamID, DWORD dwTypeIndex, IMFMediaType **ppType)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetInputAvailableType()\n");

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		const utvf_t *putvf = T::GetInputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return MF_E_NO_MORE_TYPES;
			if (UtVideoFormatToMediaFoundationFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;

		(*ppType)->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		(*ppType)->SetGUID(MF_MT_SUBTYPE, subtype);
		(*ppType)->SetUINT32(MF_MT_COMPRESSED, !T::bEncoding);
		(*ppType)->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, T::bEncoding || !m_pCodec->IsTemporalCompressionSupported());

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType **ppType)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetOutputAvailableType()\n");

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		const utvf_t *putvf = T::GetOutputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return MF_E_NO_MORE_TYPES;
			if (UtVideoFormatToMediaFoundationFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;

		(*ppType)->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		(*ppType)->SetGUID(MF_MT_SUBTYPE, subtype);
		(*ppType)->SetUINT32(MF_MT_COMPRESSED, T::bEncoding);
		(*ppType)->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, !T::bEncoding || !m_pCodec->IsTemporalCompressionSupported());

		if (m_pInputMediaType != NULL)
		{
			void *buf;
			size_t cbExtraData;
			GUID inputSubtype;
			UINT32 u32FrameWidth, u32FrameHeight;
			UINT32 u32FrameRateNumerator, u32FrameRateDenominator;
			utvf_t infmt;
			utvf_t outfmt = *putvf;

			m_pInputMediaType->GetGUID(MF_MT_SUBTYPE, &inputSubtype);
			MediaFoundationFormatToUtVideoFormat(&infmt, inputSubtype);
			MFGetAttributeSize(m_pInputMediaType, MF_MT_FRAME_SIZE, &u32FrameWidth, &u32FrameHeight);
			MFSetAttributeSize((*ppType), MF_MT_FRAME_SIZE, u32FrameWidth, u32FrameHeight);
			MFGetAttributeRatio(m_pInputMediaType, MF_MT_FRAME_RATE, &u32FrameRateNumerator, &u32FrameRateDenominator);
			MFSetAttributeRatio((*ppType), MF_MT_FRAME_RATE, u32FrameRateNumerator, u32FrameRateDenominator);
			// 最低限フレームサイズとフレームレートは設定しておく必要があるようだ

			cbExtraData = ((T *)this)->GetExtraDataSize();
			buf = malloc(max(cbExtraData, 1)); // cbExtraData が 0 の時に malloc() に 0 を渡して NULL が返ってくるのを防ぐ
			((T *)this)->GetExtraData(buf, cbExtraData, outfmt, infmt, u32FrameWidth, u32FrameHeight);
			(*ppType)->SetBlob(MF_MT_USER_DATA, (UINT8 *)buf, (UINT32)cbExtraData);
			free(buf);
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetInputType(DWORD dwInputStreamID, IMFMediaType *pType, DWORD dwFlags)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::SetInputType()\n");

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;
		IMFMediaType *pNewType;

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (pType == NULL)
		{
			m_pInputMediaType->Release();
			m_pInputMediaType = NULL;
			return S_OK;
		}

		LockAttr lockattr(pType);

		// TODO: check type

		if (dwFlags & MFT_SET_TYPE_TEST_ONLY)
			return S_OK;

		hr = MFCreateMediaType(&pNewType);
		if (FAILED(hr))
			return hr;
		hr = pType->CopyAllItems(pNewType);
		if (FAILED(hr))
		{
			pNewType->Release();
			return hr;
		}
		if (m_pInputMediaType != NULL)
			m_pInputMediaType->Release();
		m_pInputMediaType = pNewType;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetOutputType(DWORD dwOutputStreamID, IMFMediaType *pType, DWORD dwFlags)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::SetOutputType()\n");

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;
		IMFMediaType *pNewType;

		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (pType == NULL)
		{
			m_pOutputMediaType->Release();
			m_pOutputMediaType = NULL;
			return S_OK;
		}

		LockAttr lockattr(pType);

		// TODO: check type

		if (dwFlags & MFT_SET_TYPE_TEST_ONLY)
			return S_OK;

		hr = MFCreateMediaType(&pNewType);
		if (FAILED(hr))
			return hr;
		hr = pType->CopyAllItems(pNewType);
		if (FAILED(hr))
		{
			pNewType->Release();
			return hr;
		}
		if (m_pOutputMediaType != NULL)
			m_pOutputMediaType->Release();
		m_pOutputMediaType = pNewType;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetInputCurrentType(DWORD dwInputStreamID, IMFMediaType **ppType)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetInputCurrentType()\n");

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (m_pInputMediaType == NULL)
			return MF_E_TRANSFORM_TYPE_NOT_SET;

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;
		hr = m_pInputMediaType->CopyAllItems(*ppType);
		if (FAILED(hr))
		{
			(*ppType)->Release();
			return hr;
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType **ppType)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetOutputCurrentType()\n");

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;

		if (dwOutputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (m_pOutputMediaType == NULL)
			return MF_E_TRANSFORM_TYPE_NOT_SET;

		hr = MFCreateMediaType(ppType);
		if (FAILED(hr))
			return hr;
		hr = m_pOutputMediaType->CopyAllItems(*ppType);
		if (FAILED(hr))
		{
			(*ppType)->Release();
			return hr;
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetInputStatus(DWORD dwInputStreamID, DWORD *pdwFlags)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetInputStatus()\n");

		LockIt lck(static_cast<T *>(this));

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		*pdwFlags = m_pInputSample == NULL ? MFT_INPUT_STATUS_ACCEPT_DATA : 0;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetOutputStatus(DWORD *pdwFlags)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::GetOutputStatus()\n");

		LockIt lck(static_cast<T *>(this));

		*pdwFlags = m_pInputSample == NULL ? 0 : MFT_OUTPUT_STATUS_SAMPLE_READY;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetOutputBounds(LONGLONG hnsLowerBound, LONGLONG hnsUpperBound)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::SetOutputBounds()\n");
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ProcessEvent(DWORD dwInputStreamID, IMFMediaEvent *pEvent)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::ProcessEvent()\n");
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
	{
		_RPT1(_CRT_WARN, "CMFTCodec::ProcessMessage() eMessage=%08x\n", eMessage);
//		return E_NOTIMPL; // TODO
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ProcessInput(DWORD dwInputStreamID, IMFSample *pSample, DWORD dwFlags)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::ProcessInput()\n");

		LockIt lck(static_cast<T *>(this));

		BeginStream();

		if (dwInputStreamID != 0)
			return MF_E_INVALIDSTREAMNUMBER;

		if (m_pInputSample != NULL)
			return MF_E_NOTACCEPTING;

		m_pInputSample = pSample;
		m_pInputSample->AddRef();

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER *pOutputSamples, DWORD *pdwStatus)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::ProcessOutput()\n");

		LockIt lck(static_cast<T *>(this));

		HRESULT hr;
		IMFMediaBuffer *pInputBuffer;
		IMFMediaBuffer *pOutputBuffer;
		IMFSample *pOutputSample;
		size_t cbOutput;
		utvf_t infmt, outfmt;
		UINT32 u32FrameWidth, u32FrameHeight;
		BYTE *pInputByteBuffer;
		BYTE *pOutputByteBuffer;
		GUID guidSubtype;
		LONGLONG ll;
		UINT32 bKeyFrame;

		if (m_pInputSample == NULL)
			return MF_E_TRANSFORM_NEED_MORE_INPUT;

		m_pOutputMediaType->GetGUID(MF_MT_SUBTYPE, &guidSubtype);
		if (MediaFoundationFormatToUtVideoFormat(&outfmt, guidSubtype))
			return MF_E_INVALIDMEDIATYPE;

		m_pInputMediaType->GetGUID(MF_MT_SUBTYPE, &guidSubtype);
		if (MediaFoundationFormatToUtVideoFormat(&infmt, guidSubtype))
			return MF_E_INVALIDMEDIATYPE;

		hr = m_pInputSample->ConvertToContiguousBuffer(&pInputBuffer);
		if (FAILED(hr))
			return hr;
		hr = MFCreateSample(&pOutputSample);
		if (FAILED(hr))
		{
			pInputBuffer->Release();
			return hr;
		}

		MFGetAttributeSize(m_pInputMediaType, MF_MT_FRAME_SIZE, &u32FrameWidth, &u32FrameHeight);
		MFCreateAlignedMemoryBuffer((DWORD)((T *)this)->GetSize(outfmt, infmt, u32FrameWidth, u32FrameHeight), 4, &pOutputBuffer);
		pOutputSample->AddBuffer(pOutputBuffer);
		if (FAILED(m_pInputSample->GetUINT32(MFSampleExtension_CleanPoint, &bKeyFrame)))
			bKeyFrame = FALSE;

		pInputBuffer->Lock(&pInputByteBuffer, NULL, NULL);
		pOutputBuffer->Lock(&pOutputByteBuffer, NULL, NULL);
		cbOutput = m_pCodec->DecodeFrame(pOutputByteBuffer, pInputByteBuffer, (bKeyFrame != FALSE) /* convert to bool */);
		pInputBuffer->Unlock();
		pOutputBuffer->Unlock();
		pOutputBuffer->SetCurrentLength((DWORD)cbOutput);

		if (SUCCEEDED(m_pInputSample->GetSampleTime(&ll)))
			pOutputSample->SetSampleTime(ll);
		if (SUCCEEDED(m_pInputSample->GetSampleDuration(&ll)))
			pOutputSample->SetSampleDuration(ll);
		pOutputSample->SetUINT32(MFSampleExtension_CleanPoint, TRUE);

		pInputBuffer->Release();
		pOutputBuffer->Release();

		pOutputSamples->pSample = pOutputSample;

		m_pInputSample->Release();
		m_pInputSample = NULL;
		return S_OK;
	}


	HRESULT BeginStream(void)
	{
		HRESULT hr;

		if (m_bStreamBegin)
			return S_OK;

		hr = ((T *)this)->InternalBeginStream();
		if (SUCCEEDED(hr))
			m_bStreamBegin = true;

		return hr;
	}

	HRESULT EndStream(void)
	{
		HRESULT hr;

		if (!m_bStreamBegin)
			return S_OK;

		hr = ((T *)this)->InternalEndStream();
		if (SUCCEEDED(hr))
			m_bStreamBegin = false;

		return S_OK;
	}

	// IMediaObjectImpl
	/*
	HRESULT InternalGetInputStreamInfo(DWORD dwInputStreamIndex, DWORD *pdwFlags)
	{
		*pdwFlags = MFT_INPUT_STREAMF_WHOLE_SAMPLES |
					MFT_INPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER |
					(T::bEncoding ? MFT_INPUT_STREAMF_FIXED_SAMPLE_SIZE : 0);

		return S_OK;
	}

	HRESULT InternalGetOutputStreamInfo(DWORD dwOutputStreamIndex, DWORD *pdwFlags)
	{
		*pdwFlags = MFT_OUTPUT_STREAMF_WHOLE_SAMPLES |
					MFT_OUTPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER |
					(T::bEncoding ? 0 : MFT_OUTPUT_STREAMF_FIXED_SAMPLE_SIZE);

		return S_OK;
	}

	HRESULT InternalCheckInputType(DWORD dwInputStreamIndex, const MFT_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalCheckInputType()\n");

		const VIDEOINFOHEADER *pvih;
		utvf_t infmt;

		if (!IsEqualGUID(pmt->majortype, MEDIATYPE_Video))
			return MFT_E_INVALIDTYPE;
		if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
			return MFT_E_INVALIDTYPE;

		pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

		if (DirectShowFormatToUtVideoFormat(&infmt, pvih->bmiHeader.biCompression, pvih->bmiHeader.biBitCount, pmt->subtype) != 0)
			return MFT_E_INVALIDTYPE;

		if (pvih->bmiHeader.biHeight < 0)
			return MFT_E_INVALIDTYPE;

		if (((T *)this)->Query(UTVF_INVALID, infmt,
				pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight,
				NULL, 0,
				((const BYTE *)&pvih->bmiHeader) + sizeof(BITMAPINFOHEADER), pvih->bmiHeader.biSize - sizeof(BITMAPINFOHEADER)) != 0)
		{
			return MFT_E_INVALIDTYPE;
		}

		return S_OK;
	}

	HRESULT InternalCheckOutputType(DWORD dwOutputStreamIndex, const MFT_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalCheckOutputType()\n");

		const MFT_MEDIA_TYPE *pmtIn;
		const VIDEOINFOHEADER *pvih;
		const VIDEOINFOHEADER *pvihIn;
		utvf_t infmt;
		utvf_t outfmt;

		if (!InputTypeSet(0))
			return MFT_E_INVALIDTYPE;
		pmtIn = InputType(0);
		pvihIn = (const VIDEOINFOHEADER *)pmtIn->pbFormat;

		if (!IsEqualGUID(pmt->majortype, MEDIATYPE_Video))
			return MFT_E_INVALIDTYPE;
		if (!IsEqualGUID(pmt->formattype, FORMAT_VideoInfo))
			return MFT_E_INVALIDTYPE;

		pvih = (const VIDEOINFOHEADER *)pmt->pbFormat;

		if (DirectShowFormatToUtVideoFormat(&infmt, pvihIn->bmiHeader.biCompression, pvihIn->bmiHeader.biBitCount, pmtIn->subtype) != 0)
			return MFT_E_INVALIDTYPE;
		if (DirectShowFormatToUtVideoFormat(&outfmt, pvih->bmiHeader.biCompression, pvih->bmiHeader.biBitCount, pmt->subtype) != 0)
			return MFT_E_INVALIDTYPE;

		if (pvih->bmiHeader.biWidth != pvihIn->bmiHeader.biWidth || pvih->bmiHeader.biHeight != pvihIn->bmiHeader.biHeight)
			return MFT_E_INVALIDTYPE;

		if (((T *)this)->Query(outfmt, infmt,
				pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight,
				((const BYTE *)&pvih->bmiHeader) + sizeof(BITMAPINFOHEADER), pvih->bmiHeader.biSize - sizeof(BITMAPINFOHEADER),
				((const BYTE *)&pvihIn->bmiHeader) + sizeof(BITMAPINFOHEADER), pvihIn->bmiHeader.biSize - sizeof(BITMAPINFOHEADER)) != 0)
		{
			return MFT_E_INVALIDTYPE;
		}

		return S_OK;
	}

	HRESULT InternalGetInputType(DWORD dwInputStreamIndex, DWORD dwTypeIndex, MFT_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalGetInputType()\n");

		const utvf_t *putvf = T::GetInputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return MFT_E_NO_MORE_ITEMS;
			if (UtVideoFormatToDirectShowFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		if (pmt == NULL)
			return S_OK;

		memset(pmt, 0, sizeof(MFT_MEDIA_TYPE));
		pmt->majortype            = MEDIATYPE_Video;
		pmt->subtype              = subtype;
		pmt->bFixedSizeSamples    = T::bEncoding;
		pmt->bTemporalCompression = m_pCodec->IsTemporalCompressionSupported() && !T::bEncoding;

		return S_OK;
	}

	HRESULT InternalGetOutputType(DWORD dwOutputStreamIndex, DWORD dwTypeIndex, MFT_MEDIA_TYPE *pmt)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalGetOutputType()\n");

		const utvf_t *putvf = T::GetOutputFormatInfo(m_pCodec);
		GUID subtype;

		for (;; putvf++)
		{
			if (!*putvf)
				return MFT_E_NO_MORE_ITEMS;
			if (UtVideoFormatToDirectShowFormat(&subtype, *putvf) != 0)
				continue;
			if (dwTypeIndex == 0)
				break;
			dwTypeIndex--;
		}

		if (pmt == NULL)
			return S_OK;

		memset(pmt, 0, sizeof(MFT_MEDIA_TYPE));
		pmt->majortype            = MEDIATYPE_Video;
		pmt->subtype              = subtype;
		pmt->bFixedSizeSamples    = !T::bEncoding;
		pmt->bTemporalCompression = m_pCodec->IsTemporalCompressionSupported() && T::bEncoding;

		if (InputTypeSet(0))
		{
			const MFT_MEDIA_TYPE *pmtIn = InputType(0);
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
		_RPT0(_CRT_WARN, "CMFTCodec::InternalGetInputSizeInfo()\n");

		*pcbSize = 0;
		*pcbMaxLookahead = 0;
		*pcbAlignment = 4;

		return S_OK;
	}

	HRESULT InternalGetOutputSizeInfo(DWORD dwOutputStreamIndex, DWORD *pcbSize, DWORD *pcbAlignment)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalGetOutputSizeInfo()\n");

		const MFT_MEDIA_TYPE *pmtIn  = InputType(0);
		const MFT_MEDIA_TYPE *pmtOut = OutputType(0);
		const VIDEOINFOHEADER *pvihIn  = (const VIDEOINFOHEADER *)pmtIn->pbFormat;
		const VIDEOINFOHEADER *pvihOut = (const VIDEOINFOHEADER *)pmtOut->pbFormat;
		utvf_t infmt;
		utvf_t outfmt;

		if (DirectShowFormatToUtVideoFormat(&infmt, pvihIn->bmiHeader.biCompression, pvihIn->bmiHeader.biBitCount, pmtIn->subtype) != 0)
			return MFT_E_INVALIDTYPE;
		if (DirectShowFormatToUtVideoFormat(&outfmt, pvihOut->bmiHeader.biCompression, pvihOut->bmiHeader.biBitCount, pmtOut->subtype) != 0)
			return MFT_E_INVALIDTYPE;

		*pcbSize = (DWORD)((T *)this)->GetSize(outfmt, infmt, pvihIn->bmiHeader.biWidth, pvihIn->bmiHeader.biHeight);
		*pcbAlignment = 4;

		return S_OK;
	}

	HRESULT InternalGetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME *prtMaxLatency)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalGetInputMaxLatency()\n");

		return E_NOTIMPL;
	}

	HRESULT InternalSetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME rtMaxLatency)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalSetInputMaxLatency()\n");

		return E_NOTIMPL;
	}

	HRESULT InternalFlush()
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalFlush()\n");

		if (m_pInputBuffer != NULL)
		{
			m_pInputBuffer->Release();
			m_pInputBuffer = NULL;
		}

		return S_OK;
	}

	HRESULT InternalDiscontinuity(DWORD dwInputStreamIndex)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalDiscontinuity()\n");

		return S_OK;
	}

	//HRESULT InternalAllocateStreamingResources();

	//HRESULT InternalFreeStreamingResources();

	HRESULT InternalProcessInput(DWORD dwInputStreamIndex, IMediaBuffer *pBuffer, DWORD dwFlags, REFERENCE_TIME rtTimestamp, REFERENCE_TIME rtTimelength)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalProcessInput()\n");

		m_pInputBuffer = pBuffer;
		m_pInputBuffer->AddRef();
		m_bInputKeyFrame = dwFlags & MFT_INPUT_DATA_BUFFERF_SYNCPOINT;
		m_bInputTimestampValid = dwFlags & MFT_INPUT_DATA_BUFFERF_TIME;
		m_bInputTimelengthValid = dwFlags & MFT_INPUT_DATA_BUFFERF_TIMELENGTH;
		m_rtInputTimestamp = rtTimestamp;
		m_rtInputTimelength = rtTimelength;

		return S_OK;
	}

	//HRESULT InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus);

	HRESULT InternalAcceptingInput(DWORD dwInputStreamIndex)
	{
		_RPT0(_CRT_WARN, "CMFTCodec::InternalAcceptingInput()\n");

		if (m_pInputBuffer == NULL)
			return S_OK;
		else
			return S_FALSE;
	}
	*/
};
