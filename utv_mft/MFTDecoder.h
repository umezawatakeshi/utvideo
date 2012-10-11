/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// MFTDecoder.h : CMFTDecoder の宣言

#pragma once
#include "resource.h"       // メイン シンボル

#include "utvideo.h"
#include "utv_mft.h"
#include <vfw.h>
#include "Codec.h"
#include "ClsID.h"

#include "MFTCodec.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif



// CMFTDecoder
// <FCC>-D992-460D-840B-C1C6497457EF

class ATL_NO_VTABLE CMFTDecoder :
	public CMFTCodec<CMFTDecoder>
{
public:
	CMFTDecoder(DWORD fcc, REFCLSID clsid) : CMFTCodec(fcc, clsid) {}
	virtual ~CMFTDecoder() {}

	BEGIN_COM_MAP(CMFTDecoder)
		COM_INTERFACE_ENTRY(IMFTransform)
	END_COM_MAP()

public:
	// CMFTCodec
	static const GUID &MFTCATEGORY /* = MFT_CATEGORY_VIDEO_DECODER */;
	static const UINT IDR = IDR_MFTDECODER;
	static const bool bEncoding = false;

	static const utvf_t *GetInputFormatInfo(CCodec *pCodec) { return pCodec->GetCompressedFormat(); }
	static const utvf_t *GetOutputFormatInfo(CCodec *pCodec) { return pCodec->GetDecoderOutputFormat(); }
	static const void GetName(CCodec *pCodec, WCHAR *szCodecName, size_t cchCodecName) { wsprintfW(szCodecName, L"%S MFT Decoder", pCodec->GetTinyName()); }

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
	HRESULT InternalBeginStream(void);
	HRESULT InternalEndStream(void);
};


template<DWORD fcc, const CLSID *pclsid>
class ATL_NO_VTABLE CSpecializedMFTDecoder :
	public CMFTDecoder,
	public CComCoClass<CSpecializedMFTDecoder<fcc, pclsid>, pclsid>
{
public:
	CSpecializedMFTDecoder() : CMFTDecoder(fcc, *pclsid) {}

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
	{
		return CMFTDecoder::UpdateRegistry(fcc, *pclsid, bRegister);
	}
};

#define MFTDECODER_ENTRY_AUTO(fcc, fccname) \
	typedef CSpecializedMFTDecoder<fcc, &CLSID_##fccname##MFTDecoder> C##fccname##MFTDecoder; \
	OBJECT_ENTRY_AUTO(CLSID_##fccname##MFTDecoder, C##fccname##MFTDecoder)

MFTDECODER_ENTRY_AUTO(FCC('ULRA'), ULRA);
MFTDECODER_ENTRY_AUTO(FCC('ULRG'), ULRG);
MFTDECODER_ENTRY_AUTO(FCC('ULY0'), ULY0);
MFTDECODER_ENTRY_AUTO(FCC('ULY2'), ULY2);
