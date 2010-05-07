/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "VCMCodec.h"

CVCMCodec::CVCMCodec(DWORD fccHandler)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::CVCMCodec(DWORD) this=%p fccHandler=%08X (%c%c%c%c)", this, fccHandler, FCC4PRINTF(fccHandler));

	m_pCodec = CCodec::CreateInstance(fccHandler);

	_RPT2(_CRT_WARN, "infcc=%08X foundfcc=%08X\n", fccHandler, m_pCodec->GetFCC());
}

CVCMCodec::~CVCMCodec(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::~CVCMCodec(void) this=%p", this);

	delete m_pCodec;
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
		// Ç»Ç∫Ç©è¨ï∂éöÇ≈ìnÇ≥ÇÍÇÈÇ±Ç∆Ç™Ç†ÇÈÇÃÇ≈ÅAç≈èâÇ…ëÂï∂éöâªÇµÇƒÇ®Ç≠ÅB
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
	char buf[256];

	wsprintf(buf, "Ut Video Codec Suite, Version %s %s \nCopyright (C) 2008-2010  UMEZAWA Takeshi",
		UTVIDEO_VERSION_STR,
		UTVIDEO_IMPLEMENTATION_STR);
	MessageBox(hwnd, buf, "Ut Video Codec Suite", MB_OK);
	return ICERR_OK;
}

LRESULT CVCMCodec::GetInfo(ICINFO *icinfo, SIZE_T cb)
{
	if (icinfo == NULL)
		return sizeof(ICINFO);

	if (cb < sizeof(ICINFO))
		return 0;

	icinfo->dwSize       = sizeof(ICINFO);
	icinfo->fccType      = ICTYPE_VIDEO;
	icinfo->fccHandler   = m_pCodec->GetFCC();
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
	return m_pCodec->Configure(hwnd);
}

LRESULT CVCMCodec::GetStateSize(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::GetStateSize() this=%p", this);
	return m_pCodec->GetStateSize();
}

LRESULT CVCMCodec::GetState(void *pState, SIZE_T cb)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::GetState() this=%p pState=%p, cb=%z", this, pState, cb);
	return m_pCodec->GetState(pState, cb);
}

LRESULT CVCMCodec::SetState(const void *pState, SIZE_T cb)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::SetState() this=%p pState=%p, cb=%z", this, pState, cb);
	return m_pCodec->SetState(pState, cb);
}

LRESULT CVCMCodec::Compress(const ICCOMPRESS *icc, SIZE_T cb)
{
	return m_pCodec->Compress(icc, cb);
}

LRESULT CVCMCodec::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::CompressBegin() this=%p", this);
	return m_pCodec->CompressBegin(pbihIn, pbihOut);
}

LRESULT CVCMCodec::CompressEnd(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::CompressEnd() this=%p", this);
	return m_pCodec->CompressEnd();
}

LRESULT CVCMCodec::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::CompressGetFormat() this=%p", this);
	return m_pCodec->CompressGetFormat(pbihIn, pbihOut);
}

LRESULT CVCMCodec::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::CompressGetSize() this=%p", this);
	return m_pCodec->CompressGetSize(pbihIn, pbihOut);
}

LRESULT CVCMCodec::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::CompressQuery() this=%p", this);
	return m_pCodec->CompressQuery(pbihIn, pbihOut);
}

LRESULT CVCMCodec::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	return m_pCodec->Decompress(icd, cb);
}

LRESULT CVCMCodec::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressBegin() this=%p", this);
	return m_pCodec->DecompressBegin(pbihIn, pbihOut);
}

LRESULT CVCMCodec::DecompressEnd(void)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressEnd() this=%p", this);
	return m_pCodec->DecompressEnd();
}

LRESULT CVCMCodec::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressGetFormat() this=%p", this);
	return m_pCodec->DecompressGetFormat(pbihIn, pbihOut);
}

LRESULT CVCMCodec::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	DEBUG_ENTER_LEAVE("CVCMCodec::DecompressQuery() this=%p", this);
	return m_pCodec->DecompressQuery(pbihIn, pbihOut);
}
