/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// DMOEncoder.h : CDMOEncoder の宣言

#pragma once
#include "resource.h"       // メイン シンボル

#include "utvideo.h"
#include "utv_dmo.h"
#include <vfw.h>
#include "Codec.h"
#include "ProcessBlacklist.h"
#include "ClsID.h"

#include "DMOCodec.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif



// CDMOEncoder
// <FCC>-E991-460D-840B-C1C6497457EF

class ATL_NO_VTABLE CDMOEncoder :
	public CDMOCodec<CDMOEncoder>,
	public IPersistStream,
	public IAMVfwCompressDialogs
{
public:
	CDMOEncoder(DWORD fcc, REFCLSID clsid) : CDMOCodec(fcc, clsid) {}
	virtual ~CDMOEncoder() {}

	BEGIN_COM_MAP(CDMOEncoder)
		COM_INTERFACE_ENTRY(IMediaObject)
		COM_INTERFACE_ENTRY(IPersist)
		COM_INTERFACE_ENTRY(IPersistStream)
		COM_INTERFACE_ENTRY(IAMVfwCompressDialogs)
	END_COM_MAP()

	HRESULT FinalConstruct()
	{
		if (CheckInterfaceDisabledAndLog("DMO", "Encoder"))
			return E_FAIL;
		return CDMOCodec<CDMOEncoder>::FinalConstruct();
	}

public:
	// CDMOCodec
	static const GUID &DMOCATEGORY /* = DMOCATEGORY_VIDEO_ENCODER */;
	static const UINT IDR = IDR_DMOENCODER;
	static const bool bEncoding = true;

	static const utvf_t *GetInputFormatInfo(CCodec *pCodec) { return pCodec->GetEncoderInputFormat(); }
	static const utvf_t *GetOutputFormatInfo(CCodec *pCodec) { return pCodec->GetCompressedFormat(); }
	static const void GetName(CCodec *pCodec, WCHAR *szCodecName, size_t cchCodecName) { pCodec->GetLongFriendlyName(szCodecName, cchCodecName); }

	size_t GetExtraDataSize(void)
	{
		return m_pCodec->EncodeGetExtraDataSize();
	}

	int GetExtraData(void *pExtraData, size_t cbExtraData, utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return m_pCodec->EncodeGetExtraData(pExtraData, cbExtraData, infmt, width, height);
	}

	size_t GetSize(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return m_pCodec->EncodeGetOutputSize(infmt, width, height);
	}

	int Query(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height, const void *pOutExtraData, size_t cbOutExtraData, const void *pInExtraData, size_t cbInExtraData)
	{
		return m_pCodec->EncodeQuery(infmt, width, height);
	}

public:
	// IMediaObjectImpl
	HRESULT InternalAllocateStreamingResources();
	HRESULT InternalFreeStreamingResources();
	HRESULT InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus);

	// IPersist
	virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID *pClassID);

	// IPersistStream
	virtual HRESULT STDMETHODCALLTYPE IsDirty(void);
	virtual HRESULT STDMETHODCALLTYPE Load(IStream *pStm);
	virtual HRESULT STDMETHODCALLTYPE Save(IStream *pStm, BOOL fClearDirty);
	virtual HRESULT STDMETHODCALLTYPE GetSizeMax(ULARGE_INTEGER *pcbSize);

	// IAMVfwCompressDialogs
	virtual HRESULT STDMETHODCALLTYPE ShowDialog(int iDialog, HWND hwnd);
	virtual HRESULT STDMETHODCALLTYPE GetState(LPVOID pState, int *pcbState);
	virtual HRESULT STDMETHODCALLTYPE SetState(LPVOID pState, int cbState);
	virtual HRESULT STDMETHODCALLTYPE SendDriverMessage(int uMsg, long dw1, long dw2);
};


template<DWORD fcc, const CLSID *pclsid>
class ATL_NO_VTABLE CSpecializedDMOEncoder :
	public CDMOEncoder,
	public CComCoClass<CSpecializedDMOEncoder<fcc, pclsid>, pclsid>
{
public:
	CSpecializedDMOEncoder() : CDMOEncoder(fcc, *pclsid) {}

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
	{
		return CDMOEncoder::UpdateRegistry(fcc, *pclsid, bRegister);
	}
};

#define DMOENCODER_ENTRY_AUTO(fcc, fccname) \
	typedef CSpecializedDMOEncoder<fcc, &CLSID_##fccname##DMOEncoder> C##fccname##DMOEncoder; \
	OBJECT_ENTRY_AUTO(CLSID_##fccname##DMOEncoder, C##fccname##DMOEncoder)

DMOENCODER_ENTRY_AUTO(FCC('ULRA'), ULRA);
DMOENCODER_ENTRY_AUTO(FCC('ULRG'), ULRG);
DMOENCODER_ENTRY_AUTO(FCC('ULY0'), ULY0);
DMOENCODER_ENTRY_AUTO(FCC('ULY2'), ULY2);
DMOENCODER_ENTRY_AUTO(FCC('ULY4'), ULY4);
DMOENCODER_ENTRY_AUTO(FCC('ULH0'), ULH0);
DMOENCODER_ENTRY_AUTO(FCC('ULH2'), ULH2);
DMOENCODER_ENTRY_AUTO(FCC('ULH4'), ULH4);

DMOENCODER_ENTRY_AUTO(FCC('UQY2'), UQY2);
DMOENCODER_ENTRY_AUTO(FCC('UQRG'), UQRG);
DMOENCODER_ENTRY_AUTO(FCC('UQRA'), UQRA);

DMOENCODER_ENTRY_AUTO(FCC('UMRA'), UMRA);
DMOENCODER_ENTRY_AUTO(FCC('UMRG'), UMRG);
DMOENCODER_ENTRY_AUTO(FCC('UMY2'), UMY2);
DMOENCODER_ENTRY_AUTO(FCC('UMY4'), UMY4);
DMOENCODER_ENTRY_AUTO(FCC('UMH2'), UMH2);
DMOENCODER_ENTRY_AUTO(FCC('UMH4'), UMH4);
