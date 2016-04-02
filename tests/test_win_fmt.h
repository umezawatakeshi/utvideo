/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#include "print_log_value.h"

extern vector<FOURCC> vecCodecFourcc;
extern vector<wstring> vecCodecShortName;
extern vector<wstring> vecCodecLongName;
extern vector<DWORD> vecTopPriorityRawFormat;
extern vector<vector<DWORD>> vecSupportedInputFormat;
extern vector<vector<DWORD>> vecSupportedOutputFormat;
extern vector<vector<DWORD>> vecUnsupportedInputFormat;
extern vector<vector<DWORD>> vecUnsupportedOutputFormat;
extern vector<vector<LONG>> vecSupportedWidth;
extern vector<vector<LONG>> vecSupportedHeight;
extern vector<vector<LONG>> vecUnsupportedWidth;
extern vector<vector<LONG>> vecUnsupportedHeight;

#define TEST_WIDTH ((LONG)1920)
#define TEST_HEIGHT ((LONG)1080)

static inline bool IsFourccRGB(DWORD dw)
{
	return dw <= 0xff;
}

static inline DWORD FCC2Compression(DWORD dw)
{
	if (IsFourccRGB(dw))
		return BI_RGB;
	else

		return dw;
}

static inline WORD FCC2BitCount(DWORD dw)
{
	if (IsFourccRGB(dw))
		return (WORD)abs((int)dw);

	switch (dw)
	{
	case FCC('ULRA'):
		return 32;
	case FCC('ULRG'):
	case FCC('ULY2'):
	case FCC('ULY0'):
	case FCC('ULH2'):
	case FCC('ULH0'):
		return 24;
	case FCC('UQY2'):
		return 30;

	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
	case FCC('UYVY'):
	case FCC('UYNV'):
	case FCC('yuvs'):
	case FCC('2vuy'):
	case FCC('HDYC'):
		return 16;
	case FCC('YV12'):
		return 12;
	case FCC('v210'):
		return 20;
	default:
		return 0; // XXX
	}
}
