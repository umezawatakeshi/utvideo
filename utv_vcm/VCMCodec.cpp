/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "VCMCodec.h"
#include <Format.h>

CVCMCodec::CVCMCodec(DWORD fccHandler) : m_fcc(fccHandler)
{
	utvf_t utvf;

	DEBUG_ENTER_LEAVE("CVCMCodec::CVCMCodec(DWORD) this=%p fccHandler=%08X", this, fccHandler);

	if (WindowsFormatToUtVideoFormat(&utvf, fccHandler, 0) != 0)
		utvf = UTVF_INVALID;

	m_pCodec = CCodec::CreateInstance(utvf, "VCM");

	_RPT2(_CRT_WARN, "in=%08X found=%s\n", fccHandler, m_pCodec->GetTinyName());
}

CVCMCodec::~CVCMCodec(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::~CVCMCodec(void) this=%p", this);

	CCodec::DeleteInstance(m_pCodec);
}

CVCMCodec *CVCMCodec::Open(ICOPEN *icopen)
{
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
	DEBUG_ENTER_LEAVE("CVCMCodec::Configure() this=%p hwnd=%08X", this, hwnd);
	return m_pCodec->Configure(hwnd) == 0 ? ICERR_OK : ICERR_UNSUPPORTED;
}

LRESULT CVCMCodec::GetStateSize(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::GetStateSize() this=%p", this);
	return m_pCodec->GetStateSize();
}

LRESULT CVCMCodec::GetState(void *pState, SIZE_T cb)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::GetState() this=%p pState=%p, cb=%z", this, pState, cb);
	return m_pCodec->GetState(pState, cb) == 0 ? ICERR_OK : ICERR_INTERNAL;
}

LRESULT CVCMCodec::SetState(const void *pState, SIZE_T cb)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::SetState() this=%p pState=%p, cb=%z", this, pState, cb);
	return m_pCodec->SetState(pState, cb) == 0 ? ICERR_OK : ICERR_INTERNAL;
}

LRESULT CVCMCodec::Compress(const ICCOMPRESS *icc, SIZE_T cb)
{
	bool bKeyFrame;
	size_t cbFrame;

	cbFrame = m_pCodec->EncodeFrame(icc->lpOutput, &bKeyFrame, icc->lpInput);
	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');
	icc->lpbiOutput->biSizeImage = (DWORD)cbFrame;
	*icc->lpdwFlags = bKeyFrame ? AVIIF_KEYFRAME : 0;
	return ICERR_OK;
}

LRESULT CVCMCodec::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	utvf_t infmt;
	LRESULT ret;

	DEBUG_ENTER_LEAVE("CVCMCodec::CompressBegin() this=%p", this);

	ret = CompressQuery(pbihIn, pbihOut);
	if (ret != ICERR_OK)
		return ret;

	if (WindowsFormatToUtVideoFormat(&infmt, pbihIn->biCompression, pbihIn->biBitCount) != 0)
		return ICERR_BADFORMAT;

	return m_pCodec->EncodeBegin(infmt, pbihIn->biWidth, pbihIn->biHeight, CBGROSSWIDTH_WINDOWS) == 0 ? ICERR_OK : ICERR_INTERNAL;
}

LRESULT CVCMCodec::CompressEnd(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::CompressEnd() this=%p", this);
	return m_pCodec->EncodeEnd();
}

LRESULT CVCMCodec::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	utvf_t infmt;

	DEBUG_ENTER_LEAVE("CVCMCodec::CompressGetFormat() this=%p", this);

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER) + m_pCodec->EncodeGetExtraDataSize();

	memcpy(pbihOut, pbihIn, sizeof(BITMAPINFOHEADER));
	pbihOut->biSize = (DWORD)(sizeof(BITMAPINFOHEADER) + m_pCodec->EncodeGetExtraDataSize());

	if (WindowsFormatToUtVideoFormat(&infmt, pbihIn->biCompression, pbihIn->biBitCount) != 0)
		return ICERR_BADFORMAT;

	UtVideoFormatToWindowsFormat(&pbihOut->biCompression, &pbihOut->biBitCount, NULL, *m_pCodec->GetCompressedFormat());
	return m_pCodec->EncodeGetExtraData(((BYTE *)pbihOut) + sizeof(BITMAPINFOHEADER), m_pCodec->EncodeGetExtraDataSize(), infmt, pbihIn->biWidth, pbihIn->biHeight, CBGROSSWIDTH_WINDOWS) == 0 ? ICERR_OK : ICERR_BADFORMAT;
}

LRESULT CVCMCodec::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	utvf_t infmt;

	DEBUG_ENTER_LEAVE("CVCMCodec::CompressGetSize() this=%p", this);

	if (WindowsFormatToUtVideoFormat(&infmt, pbihIn->biCompression, pbihIn->biBitCount) != 0)
		return ICERR_BADFORMAT;

	return (LRESULT)m_pCodec->EncodeGetOutputSize(infmt, pbihIn->biWidth, pbihIn->biHeight, CBGROSSWIDTH_WINDOWS);
}

LRESULT CVCMCodec::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	utvf_t infmt;

	DEBUG_ENTER_LEAVE("CVCMCodec::CompressQuery() this=%p", this);

	if (WindowsFormatToUtVideoFormat(&infmt, pbihIn->biCompression, pbihIn->biBitCount) != 0)
		return ICERR_BADFORMAT;

	if (pbihIn->biHeight < 0)
		return ICERR_BADFORMAT;

	if (pbihOut != NULL && (pbihOut->biWidth != pbihIn->biWidth || pbihOut->biHeight != pbihIn->biHeight))
		return ICERR_BADFORMAT;

	return m_pCodec->EncodeQuery(infmt, pbihIn->biWidth, pbihIn->biHeight, CBGROSSWIDTH_WINDOWS) == 0 ? ICERR_OK : ICERR_BADFORMAT;
}

LRESULT CVCMCodec::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	size_t cbFrame;

	cbFrame = m_pCodec->DecodeFrame(icd->lpOutput, icd->lpInput, !(icd->dwFlags & ICDECOMPRESS_NOTKEYFRAME));
	icd->lpbiOutput->biSizeImage = (DWORD)cbFrame;
	return ICERR_OK;
}

LRESULT CVCMCodec::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	utvf_t utvf;
	LRESULT ret;

	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressBegin() this=%p", this);

	ret = DecompressQuery(pbihIn, pbihOut);
	if (ret != ICERR_OK)
		return ret;

	if (WindowsFormatToUtVideoFormat(&utvf, pbihOut->biCompression, pbihOut->biBitCount) != 0)
		return ICERR_BADFORMAT;

	return m_pCodec->DecodeBegin(utvf, pbihIn->biWidth, pbihIn->biHeight, CBGROSSWIDTH_WINDOWS, ((BYTE *)pbihIn) + sizeof(BITMAPINFOHEADER), pbihIn->biSize - sizeof(BITMAPINFOHEADER)) == 0 ? ICERR_OK : ICERR_BADFORMAT;
}

LRESULT CVCMCodec::DecompressEnd(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressEnd() this=%p", this);
	return m_pCodec->DecodeEnd();
}

LRESULT CVCMCodec::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	const utvf_t *utvf;

	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressGetFormat() this=%p", this);

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER);

	memcpy(pbihOut, pbihIn, sizeof(BITMAPINFOHEADER));
	pbihOut->biSize = sizeof(BITMAPINFOHEADER);

	for (utvf = m_pCodec->GetDecoderOutputFormat(); *utvf; utvf++)
	{
		if (UtVideoFormatToWindowsFormat(&pbihOut->biCompression, &pbihOut->biBitCount, NULL, *utvf) == 0)
			return ICERR_OK;
	}

	return ICERR_INTERNAL;
}

LRESULT CVCMCodec::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	utvf_t utvf;
	const utvf_t *putvf;

	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressQuery() this=%p", this);

	WindowsFormatToUtVideoFormat(&utvf, pbihIn->biCompression, pbihIn->biBitCount);
	if (utvf != *m_pCodec->GetCompressedFormat())
		return ICERR_BADFORMAT;

	if (pbihIn->biHeight < 0)
		return ICERR_BADFORMAT;

	if (pbihOut == NULL)
		return ICERR_OK;

	if (pbihOut->biWidth != pbihIn->biWidth || pbihOut->biHeight != pbihIn->biHeight)
		return ICERR_BADFORMAT;

	if (WindowsFormatToUtVideoFormat(&utvf, pbihOut->biCompression, pbihOut->biBitCount) != 0)
		return ICERR_BADFORMAT;

	for (putvf = m_pCodec->GetDecoderOutputFormat(); *putvf; putvf++)
	{
		if (utvf != *putvf)
			continue;
		if (m_pCodec->DecodeQuery(*putvf, pbihIn->biWidth, pbihIn->biHeight, CBGROSSWIDTH_WINDOWS, ((BYTE *)pbihIn) + sizeof(BITMAPINFOHEADER), pbihIn->biSize - sizeof(BITMAPINFOHEADER)) == 0)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}
