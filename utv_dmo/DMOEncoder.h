/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// DMOEncoder.h : CDMOEncoder の宣言

#pragma once
#include "resource.h"       // メイン シンボル

#include "utv_dmo.h"
#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif



// CDMOEncoder
// 02EB5C68-6991-460D-840B-C1C6497457EF

class ATL_NO_VTABLE CDMOEncoder :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IMediaObjectImpl<CDMOEncoder, 1, 1>
{
private:
	CCodec *m_pCodec;
	IMediaBuffer *m_pInputBuffer;
	BOOL m_bInputKeyFrame;
	BOOL m_bInputTimestampValid;
	BOOL m_bInputTimelengthValid;
	REFERENCE_TIME m_rtInputTimestamp;
	REFERENCE_TIME m_rtInputTimelength;

public:
	CDMOEncoder(DWORD fcc);
	virtual ~CDMOEncoder();

	static HRESULT WINAPI UpdateRegistry(DWORD fcc, REFCLSID clsid, BOOL bRegister);

	BEGIN_COM_MAP(CDMOEncoder)
		COM_INTERFACE_ENTRY(IMediaObject)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	HRESULT InternalGetInputStreamInfo(DWORD dwInputStreamIndex, DWORD *pdwFlags);
	HRESULT InternalGetOutputStreamInfo(DWORD dwOutputStreamIndex, DWORD *pdwFlags);
	HRESULT InternalCheckInputType(DWORD dwInputStreamIndex, const DMO_MEDIA_TYPE *pmt);
	HRESULT InternalCheckOutputType(DWORD dwOutputStreamIndex, const DMO_MEDIA_TYPE *pmt);
	HRESULT InternalGetInputType(DWORD dwInputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt);
	HRESULT InternalGetOutputType(DWORD dwOutputStreamIndex, DWORD dwTypeIndex, DMO_MEDIA_TYPE *pmt);
	HRESULT InternalGetInputSizeInfo(DWORD dwInputStreamIndex, DWORD *pcbSize, DWORD *pcbMaxLookahead, DWORD *pcbAlignment);
	HRESULT InternalGetOutputSizeInfo(DWORD dwOutputStreamIndex, DWORD *pcbSize, DWORD *pcbAlignment);
	HRESULT InternalGetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME *prtMaxLatency);
	HRESULT InternalSetInputMaxLatency(DWORD dwInputStreamIndex, REFERENCE_TIME rtMaxLatency);
	HRESULT InternalFlush();
	HRESULT InternalDiscontinuity(DWORD dwInputStreamIndex);
	HRESULT InternalAllocateStreamingResources();
	HRESULT InternalFreeStreamingResources();
	HRESULT InternalProcessInput(DWORD dwInputStreamIndex, IMediaBuffer *pBuffer, DWORD dwFlags, REFERENCE_TIME rtTimestamp, REFERENCE_TIME rtTimelength);
	HRESULT InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus);
	HRESULT InternalAcceptingInput(DWORD dwInputStreamIndex);
};


template<DWORD fcc, const CLSID *pclsid>
class ATL_NO_VTABLE CSpecializedDMOEncoder :
	public CDMOEncoder,
	public CComCoClass<CSpecializedDMOEncoder<fcc, pclsid>, pclsid>
{
public:
	CSpecializedDMOEncoder() : CDMOEncoder(fcc) {}

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
	{
		return CDMOEncoder::UpdateRegistry(fcc, *pclsid, bRegister);
	}
};

#define DMOENCODER_ENTRY_AUTO(fcc, fccname) \
	typedef CSpecializedDMOEncoder<fcc, &CLSID_##fccname##DMOEncoder> C##fccname##DMOEncoder; \
	OBJECT_ENTRY_AUTO(CLSID_##fccname##DMOEncoder, C##fccname##DMOEncoder)

//DMOENCODER_ENTRY_AUTO(FCC('ULRA'), ULRA);
DMOENCODER_ENTRY_AUTO(FCC('ULRG'), ULRG);
DMOENCODER_ENTRY_AUTO(FCC('ULY0'), ULY0);
DMOENCODER_ENTRY_AUTO(FCC('ULY2'), ULY2);
