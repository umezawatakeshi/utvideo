/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Codec.h"
#include "DummyCodec.h"
#include "ULRACodec.h"
#include "ULRGCodec.h"
#include "ULYUV420Codec.h"
#include "ULYUV422Codec.h"

CCodec::CCodec(void)
{
}

CCodec::~CCodec(void)
{
}

#ifdef _WIN32
INT_PTR CCodec::About(HWND hwnd)
{
	char buf[256];

	wsprintf(buf,
		"Ut Video Codec Suite, version %s\n"
		"Copyright (C) 2008-2013  UMEZAWA Takeshi\n\n"
		"Licensed under GNU General Public License version 2 or later.",
		UTVIDEO_VERSION_STR);
	MessageBox(hwnd, buf, "Ut Video Codec Suite", MB_OK);

	return 0;
}
#endif

struct CODECLIST
{
	utvf_t utvf;
	CCodec *(*pfnCreateInstace)(const char *pszInterfaceName);
};

static const struct CODECLIST codeclist[] = {
	{ UTVF_INVALID, CDummyCodec::CreateInstance },
	{ UTVF_ULRA,    CULRACodec::CreateInstance  },
	{ UTVF_ULRG,    CULRGCodec::CreateInstance  },
	{ UTVF_ULY0,    CULY0Codec::CreateInstance  },
	{ UTVF_ULY2,    CULY2Codec::CreateInstance  },
};

DLLEXPORT CCodec *CCodec::CreateInstance(utvf_t utvf, const char *pszInterfaceName)
{
	int idx;

	DEBUG_ENTER_LEAVE("CCodec::CreateInstance(uint32_t) utvf=%08X", utvf);

	for (idx = 0; idx < _countof(codeclist); idx++)
	{
		if (codeclist[idx].utvf == utvf)
			break;
	}
	if (idx == _countof(codeclist))
		idx = 0;

//	_RPT2(_CRT_WARN, "in=%08X found=%08X\n", utvf, codeclist[idx].utvf);

	return codeclist[idx].pfnCreateInstace(pszInterfaceName);
}

DLLEXPORT void CCodec::DeleteInstance(CCodec *pCodec)
{
	delete pCodec;
}
