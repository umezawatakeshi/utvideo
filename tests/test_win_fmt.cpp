/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#pragma init_seg(lib)

#include "test_win_fmt.h"

vector<DWORD> vecCodecFourcc = {
	FCC('ULRG'),
	FCC('ULRA'),
	FCC('ULY2'),
	FCC('ULY0'),
	FCC('ULH2'),
	FCC('ULH0'),
	FCC('UQY2'),
};

vector<CODECNAME> vecCodecName = {
	{ L"UtVideo (ULRG)", L"UtVideo RGB VCM" },
	{ L"UtVideo (ULRA)", L"UtVideo RGBA VCM" },
	{ L"UtVideo (ULY2)", L"UtVideo YUV422 BT.601 VCM" },
	{ L"UtVideo (ULY0)", L"UtVideo YUV420 BT.601 VCM" },
	{ L"UtVideo (ULH2)", L"UtVideo YUV422 BT.709 VCM" },
	{ L"UtVideo (ULH0)", L"UtVideo YUV420 BT.709 VCM" },
	{ L"UtVideo (UQY2)", L"UtVideo Pro YUV422 10bit VCM" },
};
