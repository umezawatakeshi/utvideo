/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// MFTEncoder.h : CMFTEncoder の宣言

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



// CMFTEncoder
// <FCC>-E992-460D-840B-C1C6497457EF

class ATL_NO_VTABLE CMFTEncoder :
	public CMFTCodec<CMFTEncoder>
{
public:
	CMFTEncoder(DWORD fcc, REFCLSID clsid) : CMFTCodec(fcc, clsid) {}
	virtual ~CMFTEncoder() {}

	BEGIN_COM_MAP(CMFTEncoder)
		COM_INTERFACE_ENTRY(IMFTransform)
	END_COM_MAP()

public:
	// CMFTCodec
	static const GUID &MFTCATEGORY /* = MFT_CATEGORY_VIDEO_ENCODER */;
	static const UINT IDR = IDR_MFTENCODER;
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
		return m_pCodec->EncodeGetExtraData(pExtraData, cbExtraData, infmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

	size_t GetSize(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height)
	{
		return m_pCodec->EncodeGetOutputSize(infmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

	int Query(utvf_t outfmt, utvf_t infmt, unsigned int width, unsigned int height, const void *pOutExtraData, size_t cbOutExtraData, const void *pInExtraData, size_t cbInExtraData)
	{
		return m_pCodec->EncodeQuery(infmt, width, height, CBGROSSWIDTH_WINDOWS);
	}

public:
	HRESULT InternalBeginStream(void);
	HRESULT InternalEndStream(void);
	HRESULT InternalProcessOutput(IMFSample *pOutputSample, IMFSample *pInputSample);
};


template<DWORD fcc, const CLSID *pclsid>
class ATL_NO_VTABLE CSpecializedMFTEncoder :
	public CMFTEncoder,
	public CComCoClass<CSpecializedMFTEncoder<fcc, pclsid>, pclsid>
{
public:
	CSpecializedMFTEncoder() : CMFTEncoder(fcc, *pclsid) {}

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
	{
		return CMFTEncoder::UpdateRegistry(fcc, *pclsid, bRegister);
	}
};

#define MFTENCODER_ENTRY_AUTO(fcc, fccname) \
	typedef CSpecializedMFTEncoder<fcc, &CLSID_##fccname##MFTEncoder> C##fccname##MFTEncoder; \
	OBJECT_ENTRY_AUTO(CLSID_##fccname##MFTEncoder, C##fccname##MFTEncoder)

MFTENCODER_ENTRY_AUTO(FCC('ULRA'), ULRA);
MFTENCODER_ENTRY_AUTO(FCC('ULRG'), ULRG);
MFTENCODER_ENTRY_AUTO(FCC('ULY0'), ULY0);
MFTENCODER_ENTRY_AUTO(FCC('ULY2'), ULY2);
MFTENCODER_ENTRY_AUTO(FCC('ULH0'), ULH0);
MFTENCODER_ENTRY_AUTO(FCC('ULH2'), ULH2);
