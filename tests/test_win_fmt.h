/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#include "print_log_value.h"

extern vector<FOURCC> vecCodecFcc;
extern vector<wstring> vecCodecShortName;
extern vector<wstring> vecCodecLongName;
extern vector<DWORD> vecTopPriorityRawFcc;
extern vector<vector<DWORD>> vecSupportedInputFccs;
extern vector<vector<DWORD>> vecSupportedOutputFccs;
extern vector<vector<DWORD>> vecUnsupportedInputFccs;
extern vector<vector<DWORD>> vecUnsupportedOutputFccs;
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
	case FCC('UMRA'):
		return 32;
	case FCC('ULRG'):
	case FCC('ULY2'):
	case FCC('ULY4'):
	case FCC('ULY0'):
	case FCC('ULH2'):
	case FCC('ULH4'):
	case FCC('ULH0'):
	case FCC('UMRG'):
	case FCC('UMY2'):
	case FCC('UMY4'):
	case FCC('UMH2'):
	case FCC('UMH4'):
		return 24;
	case FCC('UQY2'):
	case FCC('UQRG'):
		return 30;
	case FCC('UQRA'):
		return 40;


	case FCC('YV24'):
		return 24;
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
	case FCC('UYVY'):
	case FCC('UYNV'):
	case FCC('yuvs'):
	case FCC('2vuy'):
	case FCC('HDYC'):
	case FCC('YV16'):
		return 16;
	case FCC('YV12'):
		return 12;
	case FCC('v210'):
		return 20;
	case FCC('b48r'):
		return 48;
	case FCC('b64a'):
		return 64;
	default:
		return 0; // XXX
	}
}
