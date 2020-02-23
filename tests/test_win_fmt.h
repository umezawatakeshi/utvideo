/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#include "print_log_value.h"

extern const vector<FOURCC> vecCodecFcc;
extern const vector<wstring> vecCodecShortName;
extern const vector<wstring> vecCodecLongName;
extern const vector<DWORD> vecTopPriorityRawFcc;
extern const vector<vector<DWORD>> vecSupportedInputFccs;
extern const vector<vector<DWORD>> vecSupportedOutputFccs;
extern const vector<vector<DWORD>> vecUnsupportedInputFccs;
extern const vector<vector<DWORD>> vecUnsupportedOutputFccs;
extern const vector<vector<LONG>> vecSupportedWidth;
extern const vector<vector<LONG>> vecSupportedHeight;
extern const vector<vector<LONG>> vecUnsupportedWidth;
extern const vector<vector<LONG>> vecUnsupportedHeight;
extern const vector<BOOL> vecTemporalCompressionSupported; // vector<bool> ‚¾‚ÆƒNƒ\‚È“Áê‰»‚ªg‚í‚ê‚Ä‚µ‚Ü‚¤

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
	case FCC('r210'):
		return 30;
	case MAKEFOURCC('Y', '3', 0, 10):
	case MAKEFOURCC('Y', '3', 0, 16):
		return 48;
	case MAKEFOURCC('Y', '3', 10, 10):
	case MAKEFOURCC('Y', '3', 10, 16):
		return 32;
	case MAKEFOURCC('Y', '3', 11, 10):
	case MAKEFOURCC('Y', '3', 11, 16):
		return 24;
	default:
		return 0; // XXX
	}
}
