/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// DMODecoder.h : CDMODecoder の宣言

#pragma once
#include "resource.h"       // メイン シンボル

#include "utvideo.h"
#include "utv_dmo.h"
#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"

#include "DMOCodec.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif



// CDMODecoder
// <FCC>-D991-460D-840B-C1C6497457EF

class ATL_NO_VTABLE CDMODecoder :
	public CDMOCodec<CDMODecoder>
{
public:
	CDMODecoder(DWORD fcc, REFCLSID clsid) : CDMOCodec(fcc, clsid) {}
	virtual ~CDMODecoder() {}

	BEGIN_COM_MAP(CDMODecoder)
		COM_INTERFACE_ENTRY(IMediaObject)
	END_COM_MAP()

public:
	// CDMOCodec
	static const GUID &DMOCATEGORY /* = DMOCATEGORY_VIDEO_DECODER */;
	static const UINT IDR = IDR_DMODECODER;
	static const bool bEncoding = false;

	static const utvf_t *GetInputFormatInfo(CCodec *pCodec) { return pCodec->GetCompressedFormat(); }
	static const utvf_t *GetOutputFormatInfo(CCodec *pCodec) { return pCodec->GetDecoderOutputFormat(); }
	static const void GetName(CCodec *pCodec, WCHAR *szCodecName, size_t cchCodecName) { wsprintfW(szCodecName, L"%S Decoder DMO", pCodec->GetTinyName()); }

	size_t GetExtraDataSize(void)
	{
		return 0;
	}

	int GetExtraData(void *pExtraData, size_t cbExtraData, utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return 0;
	}

	size_t GetSize(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return m_pCodec->DecodeGetOutputSize(outfmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

	int Query(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height, const void *pOutExtraData, size_t cbOutExtraData, const void *pInExtraData, size_t cbInExtraData)
	{
		return m_pCodec->DecodeQuery(outfmt, width, height, CBGROSSWIDTH_WINDOWS, pInExtraData, cbInExtraData);
	}

public:
	// IMediaObjectImpl
	HRESULT InternalAllocateStreamingResources();
	HRESULT InternalFreeStreamingResources();
	HRESULT InternalProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, DMO_OUTPUT_DATA_BUFFER *pOutputBuffers, DWORD *pdwStatus);
};


template<DWORD fcc, const CLSID *pclsid>
class ATL_NO_VTABLE CSpecializedDMODecoder :
	public CDMODecoder,
	public CComCoClass<CSpecializedDMODecoder<fcc, pclsid>, pclsid>
{
public:
	CSpecializedDMODecoder() : CDMODecoder(fcc, *pclsid) {}

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
	{
		return CDMODecoder::UpdateRegistry(fcc, *pclsid, bRegister);
	}
};

#define DMODECODER_ENTRY_AUTO(fcc, fccname) \
	typedef CSpecializedDMODecoder<fcc, &CLSID_##fccname##DMODecoder> C##fccname##DMODecoder; \
	OBJECT_ENTRY_AUTO(CLSID_##fccname##DMODecoder, C##fccname##DMODecoder)

DMODECODER_ENTRY_AUTO(FCC('ULRA'), ULRA);
DMODECODER_ENTRY_AUTO(FCC('ULRG'), ULRG);
DMODECODER_ENTRY_AUTO(FCC('ULY0'), ULY0);
DMODECODER_ENTRY_AUTO(FCC('ULY2'), ULY2);
DMODECODER_ENTRY_AUTO(FCC('ULH0'), ULH0);
DMODECODER_ENTRY_AUTO(FCC('ULH2'), ULH2);
