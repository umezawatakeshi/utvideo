/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "Codec.h"
#include "ULRACodec.h"
#include "ULRGCodec.h"
#include "ULY0Codec.h"
#include "ULY2Codec.h"

CCodec::CCodec(void)
{
}

CCodec::~CCodec(void)
{
}

LRESULT CCodec::About(HWND hwnd)
{
	char buf[256];

	wsprintf(buf, "Ut Video Codec Suite, Version %s %s \nCopyright (C) 2008-2010  UMEZAWA Takeshi",
		UTVIDEO_VERSION_STR,
		UTVIDEO_IMPLEMENTATION_STR);
	MessageBox(hwnd, buf, "Ut Video Codec Suite", MB_OK);

	return ICERR_OK;
}

struct CODECLIST
{
	DWORD fcc;
	CCodec *(*pfnCreateInstace)(const char *pszInterfaceName);
};

static const struct CODECLIST codeclist[] = {
	{ -1,          CDummyCodec::CreateInstance },
	{ FCC('ULRA'), CULRACodec::CreateInstance  },
	{ FCC('ULRG'), CULRGCodec::CreateInstance  },
	{ FCC('ULY0'), CULY0Codec::CreateInstance  },
	{ FCC('ULY2'), CULY2Codec::CreateInstance  },
};

__declspec(dllexport) CCodec *CCodec::CreateInstance(DWORD fcc, const char *pszInterfaceName)
{
	int idx;

	DEBUG_ENTER_LEAVE("CCodec::CreateInstance(DWORD) fcc=%08X (%c%c%c%c)", fcc, FCC4PRINTF(fcc));

	for (idx = 0; idx < _countof(codeclist); idx++)
	{
		if (codeclist[idx].fcc == fcc)
			break;
	}
	if (idx == _countof(codeclist))
		idx = 0;

	_RPT2(_CRT_WARN, "infcc=%08X foundfcc=%08X\n", fcc, codeclist[idx].fcc);

	return codeclist[idx].pfnCreateInstace(pszInterfaceName);
}

CDummyCodec::CDummyCodec(void)
{
}

CDummyCodec::~CDummyCodec(void)
{
}

CCodec *CDummyCodec::CreateInstance(const char *pszInterfaceName)
{
	return new CDummyCodec();
}

void CDummyCodec::GetShortFriendlyName(char *pszName, size_t cchName)
{
	pszName[0] = '\0';
}

void CDummyCodec::GetShortFriendlyName(wchar_t *pszName, size_t cchName)
{
	pszName[0] = L'\0';
}

void CDummyCodec::GetLongFriendlyName(char *pszName, size_t cchName)
{
	pszName[0] = '\0';
}

void CDummyCodec::GetLongFriendlyName(wchar_t *pszName, size_t cchName)
{
	pszName[0] = L'\0';
}

DWORD CDummyCodec::GetFCC(void)
{
	return DWORD(-1);
}

BOOL CDummyCodec::IsTemporalCompressionSupported(void)
{
	return FALSE;
}

const FORMATINFO *CDummyCodec::GetEncoderInputFormat(void)
{
	return NULL;
}

const FORMATINFO *CDummyCodec::GetDecoderOutputFormat(void)
{
	return NULL;
}

const FORMATINFO *CDummyCodec::GetCompressedFormat(void)
{
	return NULL;
}

LRESULT CDummyCodec::Configure(HWND hwnd)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::GetStateSize(void)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::GetState(void *pState, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::SetState(const void *pState, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::Compress(const ICCOMPRESS *icc, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressEnd(void)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressEnd(void)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}
