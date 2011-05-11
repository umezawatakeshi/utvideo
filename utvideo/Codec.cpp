/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "Codec.h"
#include "DummyCodec.h"
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

	wsprintf(buf,
		"Ut Video Codec Suite, version %s\n"
		"Copyright (C) 2008-2011  UMEZAWA Takeshi\n\n"
		"Licensed under GNU General Public License version 2 or later.",
		UTVIDEO_VERSION_STR);
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
