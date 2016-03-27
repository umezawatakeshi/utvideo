/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#pragma warning(disable:4073)
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

vector<wstring> vecCodecShortName = {
	L"UtVideo (ULRG)",
	L"UtVideo (ULRA)",
	L"UtVideo (ULY2)",
	L"UtVideo (ULY0)",
	L"UtVideo (ULH2)",
	L"UtVideo (ULH0)",
	L"UtVideo (UQY2)",
};

vector<wstring> vecCodecLongName = {
	L"UtVideo RGB VCM",
	L"UtVideo RGBA VCM",
	L"UtVideo YUV422 BT.601 VCM",
	L"UtVideo YUV420 BT.601 VCM",
	L"UtVideo YUV422 BT.709 VCM",
	L"UtVideo YUV420 BT.709 VCM",
	L"UtVideo Pro YUV422 10bit VCM",
};

#define BI_RGB_BU(bc) (bc)
#define BI_RGB_TD(bc) ((DWORD)-bc)

vector<vector<DWORD> > vecSupportedInputFormat = {
	{ BI_RGB_BU(32), BI_RGB_BU(24) },
	{ BI_RGB_BU(32) },
	{ BI_RGB_BU(32), BI_RGB_BU(24), FCC('YUY2'), FCC('YUYV'), FCC('YUNV'), FCC('UYVY'), FCC('UYNV') },
	{ BI_RGB_BU(32), BI_RGB_BU(24), FCC('YUY2'), FCC('YUYV'), FCC('YUNV'), FCC('UYVY'), FCC('UYNV'), FCC('YV12') },
	{ BI_RGB_BU(32), BI_RGB_BU(24), FCC('YUY2'), FCC('YUYV'), FCC('YUNV'), FCC('UYVY'), FCC('UYNV'), FCC('HDYC') },
	{ BI_RGB_BU(32), BI_RGB_BU(24), FCC('YUY2'), FCC('YUYV'), FCC('YUNV'), FCC('UYVY'), FCC('UYNV'), FCC('HDYC'), FCC('YV12') },
	{ FCC('v210') },
};

vector<vector<DWORD> > vecSupportedOutputFormat = vecSupportedInputFormat;

vector<vector<DWORD> > vecUnsupportedInputFormat = {
	{ BI_RGB_BU(15), BI_RGB_BU(16), BI_RGB_TD(24), BI_RGB_TD(32) },
	{ BI_RGB_BU(15), BI_RGB_BU(16), BI_RGB_TD(24), BI_RGB_TD(32), BI_RGB_BU(24) },
	{ BI_RGB_BU(15), BI_RGB_BU(16), BI_RGB_TD(24), BI_RGB_TD(32), FCC('YVYU'), FCC('VYUY'), FCC('YV12'), FCC('HDYC') },
	{ BI_RGB_BU(15), BI_RGB_BU(16), BI_RGB_TD(24), BI_RGB_TD(32), FCC('YVYU'), FCC('VYUY'), FCC('HDYC') },
	{ BI_RGB_BU(15), BI_RGB_BU(16), BI_RGB_TD(24), BI_RGB_TD(32), FCC('YVYU'), FCC('VYUY'), FCC('YV12') },
	{ BI_RGB_BU(15), BI_RGB_BU(16), BI_RGB_TD(24), BI_RGB_TD(32), FCC('YVYU'), FCC('VYUY') },
	{ BI_RGB_BU(15), BI_RGB_BU(16), BI_RGB_TD(24), BI_RGB_TD(32), BI_RGB_BU(24), BI_RGB_BU(32), FCC('YUY2'), FCC('YUYV'), FCC('UYVY'), FCC('YV12') },
};

vector<vector<DWORD> > vecUnsupportedOutputFormat = vecUnsupportedInputFormat;

vector<vector<LONG>> vecSupportedWidth = {
	{ 1920, 1921, 1922, 1923, 1924, 1925, 1926, 1927 },
	{ 1920, 1921, 1922, 1923, 1924, 1925, 1926, 1927 },
	{ 1920, 1922, 1924, 1926 },
	{ 1920, 1922, 1924, 1926 },
	{ 1920, 1922, 1924, 1926 },
	{ 1920, 1922, 1924, 1926 },
	{ 1920, 1922, 1924, 1926 },
};

vector<vector<LONG>> vecSupportedHeight = {
	{ 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087 },
	{ 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087 },
	{ 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087 },
	{ 1080, 1082, 1084, 1086 },
	{ 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087 },
	{ 1080, 1082, 1084, 1086 },
	{ 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087 },
};

vector<vector<LONG>> vecUnsupportedWidth = {
	{ -1920, 0 },
	{ -1920, 0 },
	{ -1920, 0, 1921, 1923, 1925, 1927 },
	{ -1920, 0, 1921, 1923, 1925, 1927 },
	{ -1920, 0, 1921, 1923, 1925, 1927 },
	{ -1920, 0, 1921, 1923, 1925, 1927 },
	{ -1920, 0, 1921, 1923, 1925, 1927 },
};

vector<vector<LONG>> vecUnsupportedHeight = {
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0, 1081, 1083, 1085, 1087 },
	{ 0 },
	{ 0, 1081, 1083, 1085, 1087 },
	{ 0 },
};

vector<DWORD> vecTopPriorityRawFormat = {
	BI_RGB_BU(24),
	BI_RGB_BU(32),
	FCC('YUY2'),
	FCC('YV12'),
	FCC('HDYC'),
	FCC('YV12'),
	FCC('v210'),
};

vector<bool> vecSupportTemporalCompression = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
};
