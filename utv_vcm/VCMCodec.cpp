/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "VCMCodec.h"
#include <Format.h>

CVCMCodec::CVCMCodec(DWORD fccHandler) : m_fcc(fccHandler)
{
	utvf_t utvf;

	LOGPRINTF("%p CVCMCodec::CVCMCodec(fccHandler=%08X)", this, fccHandler);

	if (VCMFormatToUtVideoFormat(&utvf, fccHandler, 0) != 0)
		utvf = UTVF_INVALID;

	m_pCodec = CCodec::CreateInstance(utvf, "VCM");

	LOGPRINTF("%p  m_pCodec=%p, TinyName=\"%s\"", this, m_pCodec, m_pCodec->GetTinyName());
}

CVCMCodec::~CVCMCodec(void)
{
	LOGPRINTF("%p CVCMCodec::~CVCMCodec()", this);

	CCodec::DeleteInstance(m_pCodec);
}

CVCMCodec *CVCMCodec::Open(ICOPEN *icopen)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		if (icopen != NULL)
			LOGPRINTF("CVCMCodec::Open(icopen=%p, icopen->fccType=%08X, icopen->fccHandler=%08X)", icopen, icopen->fccType, icopen->fccHandler);
		else
			LOGPRINTF("CVCMCodec::Open(icopen=NULL)");
	}

	union
	{
		DWORD fccHandler;
		char fccChar[4];
	};

	if (icopen != NULL)
	{
		if (icopen->fccType != ICTYPE_VIDEO)
			return NULL;
		fccHandler = icopen->fccHandler;
		// なぜか小文字で渡されることがあるので、最初に大文字化しておく。
		for (int i = 0; i < 4; i++)
			fccChar[i] = toupper(fccChar[i]);
		icopen->dwError = ICERR_OK;
	}
	else
		fccHandler = (DWORD)-1;

	return new CVCMCodec(fccHandler);
}

LRESULT CVCMCodec::QueryAbout(void)
{
	return ICERR_OK;
}

LRESULT CVCMCodec::About(HWND hwnd)
{
	return m_pCodec->About(hwnd);
}

LRESULT CVCMCodec::GetInfo(ICINFO *icinfo, SIZE_T cb)
{
	if (icinfo == NULL)
		return sizeof(ICINFO);

	if (cb < sizeof(ICINFO))
		return 0;

	icinfo->dwSize       = sizeof(ICINFO);
	icinfo->fccType      = ICTYPE_VIDEO;
	icinfo->fccHandler   = m_fcc;
	icinfo->dwFlags      = 0;
	icinfo->dwVersion    = UTVIDEO_VERSION_AND_IMPLEMENTATION;
	icinfo->dwVersionICM = ICVERSION;
	m_pCodec->GetShortFriendlyName(icinfo->szName, _countof(icinfo->szName));
	m_pCodec->GetLongFriendlyName(icinfo->szDescription, _countof(icinfo->szDescription));

	return sizeof(ICINFO);
}

LRESULT CVCMCodec::QueryConfigure(void)
{
	return ICERR_OK;
}

LRESULT CVCMCodec::Configure(HWND hwnd)
{
	return m_pCodec->Configure(hwnd) == 0 ? ICERR_OK : ICERR_UNSUPPORTED;
}

LRESULT CVCMCodec::GetStateSize(void)
{
	return m_pCodec->GetStateSize();
}

LRESULT CVCMCodec::GetState(void *pState, SIZE_T cb)
{
	return m_pCodec->GetState(pState, cb) == 0 ? ICERR_OK : ICERR_INTERNAL;
}

LRESULT CVCMCodec::SetState(const void *pState, SIZE_T cb)
{
	return m_pCodec->SetState(pState, cb) == 0 ? cb : 0;
}

LRESULT CVCMCodec::Compress(const ICCOMPRESS *icc, SIZE_T cb)
{
	bool bKeyFrame;
	size_t cbFrame;

	utvf_t infmt;

	if (VCMFormatToUtVideoFormat(&infmt, icc->lpbiInput->biCompression, icc->lpbiInput->biBitCount) != 0)
		return ICERR_BADFORMAT;

	cbFrame = m_pCodec->EncodeFrame(icc->lpOutput, &bKeyFrame, icc->lpInput, infmt, CBGROSSWIDTH_WINDOWS);
	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');
	icc->lpbiOutput->biSizeImage = (DWORD)cbFrame;
	*icc->lpdwFlags = bKeyFrame ? AVIIF_KEYFRAME : 0;
	return ICERR_OK;
}

#define LOGPRINT_BIH_THIS(_this, prefix, pbih) \
	LOGPRINTF("%p %s: biSize=%d, biWidth=%d, biHeight=%d, biPlanes=%d, biBitCount=%d, biCompression=%08X, biSizeImage=%d", \
		_this, prefix, pbih->biSize, pbih->biWidth, pbih->biHeight, pbih->biPlanes, pbih->biBitCount, pbih->biCompression, pbih->biSizeImage);

LRESULT CVCMCodec::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		LOGPRINTF("%p CVCMCodec::CompressBegin(pbihIn=%p, pbihOut=%p)", this, pbihIn, pbihOut);
		if (pbihIn != NULL)
			LOGPRINT_BIH_THIS(this, " pbihIn ", pbihIn);
		if (pbihOut != NULL)
			LOGPRINT_BIH_THIS(this, " pbihOut", pbihOut);
	}

	LRESULT ret;

	ret = CompressQuery(pbihIn, pbihOut);
	if (ret != ICERR_OK)
		return ret;

	return m_pCodec->EncodeBegin(pbihIn->biWidth, pbihIn->biHeight) == 0 ? ICERR_OK : ICERR_INTERNAL;
}

LRESULT CVCMCodec::CompressEnd(void)
{
	LOGPRINTF("%p CVCMCodec::CompressEnd()", this);

	return m_pCodec->EncodeEnd();
}

LRESULT CVCMCodec::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		LOGPRINTF("%p CVCMCodec::CompressGetFormat(pbihIn=%p, pbihOut=%p)", this, pbihIn, pbihOut);
		if (pbihIn != NULL)
			LOGPRINT_BIH_THIS(this, " pbihIn ", pbihIn);
	}

	utvf_t infmt;

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER) + m_pCodec->EncodeGetExtraDataSize();

	memcpy(pbihOut, pbihIn, sizeof(BITMAPINFOHEADER));
	pbihOut->biSize = (DWORD)(sizeof(BITMAPINFOHEADER) + m_pCodec->EncodeGetExtraDataSize());

	if (pbihIn->biWidth <= 0 || pbihIn->biHeight <= 0)
		return ICERR_BADFORMAT;

	if (VCMFormatToUtVideoFormat(&infmt, pbihIn->biCompression, pbihIn->biBitCount) != 0)
		return ICERR_BADFORMAT;

	if (m_pCodec->EncodeQuery(infmt, pbihIn->biWidth, pbihIn->biHeight) != 0)
		return ICERR_BADFORMAT;

	UtVideoFormatToVCMFormat(&pbihOut->biCompression, &pbihOut->biBitCount, *m_pCodec->GetCompressedFormat());
	pbihOut->biSizeImage = (DWORD)m_pCodec->EncodeGetOutputSize(infmt, pbihIn->biWidth, pbihIn->biHeight);
	return m_pCodec->EncodeGetExtraData(((BYTE *)pbihOut) + sizeof(BITMAPINFOHEADER), m_pCodec->EncodeGetExtraDataSize(), pbihIn->biWidth, pbihIn->biHeight) == 0 ? ICERR_OK : ICERR_BADFORMAT;
}

LRESULT CVCMCodec::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	utvf_t infmt;

	if (VCMFormatToUtVideoFormat(&infmt, pbihIn->biCompression, pbihIn->biBitCount) != 0)
		return ICERR_BADFORMAT;

	return (LRESULT)m_pCodec->EncodeGetOutputSize(infmt, pbihIn->biWidth, pbihIn->biHeight);
}

LRESULT CVCMCodec::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		LOGPRINTF("%p CVCMCodec::CompressQuery(pbihIn=%p, pbihOut=%p)", this, pbihIn, pbihOut);
		if (pbihIn != NULL)
			LOGPRINT_BIH_THIS(this, " pbihIn ", pbihIn);
		if (pbihOut != NULL)
			LOGPRINT_BIH_THIS(this, " pbihOut", pbihOut);
	}

	utvf_t infmt;

	if (VCMFormatToUtVideoFormat(&infmt, pbihIn->biCompression, pbihIn->biBitCount) != 0)
		return ICERR_BADFORMAT;

	if (pbihIn->biHeight < 0)
		return ICERR_BADFORMAT;

	if (pbihOut != NULL && (pbihOut->biWidth != pbihIn->biWidth || pbihOut->biHeight != pbihIn->biHeight))
		return ICERR_BADFORMAT;

	return m_pCodec->EncodeQuery(infmt, pbihIn->biWidth, pbihIn->biHeight) == 0 ? ICERR_OK : ICERR_BADFORMAT;
}

LRESULT CVCMCodec::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	size_t cbFrame;

	utvf_t outfmt;

	if (VCMFormatToUtVideoFormat(&outfmt, icd->lpbiOutput->biCompression, icd->lpbiOutput->biBitCount) != 0)
		return ICERR_BADFORMAT;

	cbFrame = m_pCodec->DecodeFrame(icd->lpOutput, icd->lpInput, outfmt, CBGROSSWIDTH_WINDOWS);
	icd->lpbiOutput->biSizeImage = (DWORD)cbFrame;
	return ICERR_OK;
}

LRESULT CVCMCodec::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		LOGPRINTF("%p CVCMCodec::DecompressBegin(pbihIn=%p, pbihOut=%p)", this, pbihIn, pbihOut);
		if (pbihIn != NULL)
			LOGPRINT_BIH_THIS(this, " pbihIn ", pbihIn);
		if (pbihOut != NULL)
			LOGPRINT_BIH_THIS(this, " pbihOut", pbihOut);
	}

	LRESULT ret;

	ret = DecompressQuery(pbihIn, pbihOut);
	if (ret != ICERR_OK)
		return ret;

	return m_pCodec->DecodeBegin(pbihIn->biWidth, pbihIn->biHeight, ((BYTE *)pbihIn) + sizeof(BITMAPINFOHEADER), pbihIn->biSize - sizeof(BITMAPINFOHEADER)) == 0 ? ICERR_OK : ICERR_BADFORMAT;
}

LRESULT CVCMCodec::DecompressEnd(void)
{
	LOGPRINTF("%p CVCMCodec::DecompressEnd()", this);

	return m_pCodec->DecodeEnd();
}

LRESULT CVCMCodec::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		LOGPRINTF("%p CVCMCodec::DecompressGetFormat(pbihIn=%p, pbihOut=%p)", this, pbihIn, pbihOut);
		if (pbihIn != NULL)
			LOGPRINT_BIH_THIS(this, " pbihIn ", pbihIn);
	}

	const utvf_t *utvf;

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER);

	memcpy(pbihOut, pbihIn, sizeof(BITMAPINFOHEADER));
	pbihOut->biSize = sizeof(BITMAPINFOHEADER);

	for (utvf = m_pCodec->GetDecoderOutputFormat(); *utvf; utvf++)
	{
		if (UtVideoFormatToVCMFormat(&pbihOut->biCompression, &pbihOut->biBitCount, *utvf) == 0)
		{
			pbihOut->biSizeImage = (DWORD)m_pCodec->DecodeGetOutputSize(*utvf, pbihOut->biWidth, pbihOut->biHeight, CBGROSSWIDTH_WINDOWS);
			return ICERR_OK;
		}
	}

	return ICERR_INTERNAL;
}

LRESULT CVCMCodec::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		LOGPRINTF("%p CVCMCodec::DecompressQuery(pbihIn=%p, pbihOut=%p)", this, pbihIn, pbihOut);
		if (pbihIn != NULL)
			LOGPRINT_BIH_THIS(this, " pbihIn ", pbihIn);
		if (pbihOut != NULL)
			LOGPRINT_BIH_THIS(this, " pbihOut", pbihOut);
	}

	utvf_t utvf;
	const utvf_t *putvf;

	VCMFormatToUtVideoFormat(&utvf, pbihIn->biCompression, pbihIn->biBitCount);
	if (utvf != *m_pCodec->GetCompressedFormat())
		return ICERR_BADFORMAT;

	if (pbihIn->biHeight < 0)
		return ICERR_BADFORMAT;

	if (pbihOut == NULL)
		return ICERR_OK;

	if (pbihOut->biWidth != pbihIn->biWidth || pbihOut->biHeight != pbihIn->biHeight)
		return ICERR_BADFORMAT;

	if (VCMFormatToUtVideoFormat(&utvf, pbihOut->biCompression, pbihOut->biBitCount) != 0)
		return ICERR_BADFORMAT;

	for (putvf = m_pCodec->GetDecoderOutputFormat(); *putvf; putvf++)
	{
		if (utvf != *putvf)
			continue;
		if (m_pCodec->DecodeQuery(*putvf, pbihIn->biWidth, pbihIn->biHeight, ((BYTE *)pbihIn) + sizeof(BITMAPINFOHEADER), pbihIn->biSize - sizeof(BITMAPINFOHEADER)) == 0)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}
