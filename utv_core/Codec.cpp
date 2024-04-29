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
#include "ULYUV444Codec.h"
#include "UQY0Codec.h"
#include "UQY2Codec.h"
#include "UQRGCodec.h"
#include "UQRACodec.h"
#include "UMRACodec.h"
#include "UMRGCodec.h"
#include "UMYUV422Codec.h"
#include "UMYUV444Codec.h"

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
		"Copyright (C) 2008-2024  UMEZAWA Takeshi\n\n"
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

#define CODECENTRY(c) { c::m_utvfCodec, c::CreateInstance }

static const struct CODECLIST codeclist[] = {
	CODECENTRY(CDummyCodec),
	CODECENTRY(CULRACodec),
	CODECENTRY(CULRGCodec),
	CODECENTRY(CULYUV420Codec<CBT601Coefficient>), // ULY0
	CODECENTRY(CULYUV422Codec<CBT601Coefficient>), // ULY2
	CODECENTRY(CULYUV444Codec<CBT601Coefficient>), // ULY4
	CODECENTRY(CULYUV420Codec<CBT709Coefficient>), // ULH0
	CODECENTRY(CULYUV422Codec<CBT709Coefficient>), // ULH2
	CODECENTRY(CULYUV444Codec<CBT709Coefficient>), // ULH4
	CODECENTRY(CUQY0Codec),
	CODECENTRY(CUQY2Codec),
	CODECENTRY(CUQRGCodec),
	CODECENTRY(CUQRACodec),
	CODECENTRY(CUMRACodec),
	CODECENTRY(CUMRGCodec),
	CODECENTRY(CUMYUV422Codec<CBT601Coefficient>), // UMY2
	CODECENTRY(CUMYUV444Codec<CBT601Coefficient>), // UMY4
	CODECENTRY(CUMYUV422Codec<CBT709Coefficient>), // UMH2
	CODECENTRY(CUMYUV444Codec<CBT709Coefficient>), // UMH4
};

DLLEXPORT CCodec *CCodec::CreateInstance(utvf_t utvf, const char *pszInterfaceName)
{
	int idx;

	LOGPRINTF("CCodec::CreateInstance(utvf=%08X)", utvf);

	for (idx = 0; idx < _countof(codeclist); idx++)
	{
		if (codeclist[idx].utvf == utvf)
			break;
	}
	if (idx == _countof(codeclist))
		idx = 0;

	LOGPRINTF(" found utvf=%08X", codeclist[idx].utvf);

	return codeclist[idx].pfnCreateInstace(pszInterfaceName);
}

DLLEXPORT void CCodec::DeleteInstance(CCodec *pCodec)
{
	delete pCodec;
}
