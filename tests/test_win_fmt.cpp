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

vector<pair<DWORD, DWORD> > vecTopPriorityRawFormat = {
	{ FCC('ULRG'), BI_RGB_BU(24) },
	{ FCC('ULRA'), BI_RGB_BU(32) },
	{ FCC('ULY2'), FCC('YUY2') },
	{ FCC('ULY0'), FCC('YV12') },
	{ FCC('ULH2'), FCC('HDYC') },
	{ FCC('ULH0'), FCC('YV12') },
	{ FCC('UQY2'), FCC('v210') },
};

template<class T>
vector<T> ExpandPair(const vector<typename T::first_type> &c1, const vector<vector<typename T::second_type> >& c2)
{
	vector<T> vec;

	// itr1 Ç∆ itr2 ÇÃå^Ç™à·Ç§ÇÃÇ≈ for ÇÃèâä˙âªï∂Ç…Ç‹Ç∆ÇﬂÇƒèëÇ≠Ç±Ç∆Ç™Ç≈Ç´Ç»Ç¢
	auto itr1 = c1.begin();
	auto end1 = c1.end();
	auto itr2 = c2.begin();
	for (; itr1 != end1; ++itr1, ++itr2)
	{
		for (auto itr3 = itr2->begin(), end3 = itr2->end(); itr3 != end3; ++itr3)
			vec.push_back(T({ *itr1, *itr3 }));
	}

	return (vec);
}

vector<CODECRAWFORMAT> vecSupportedInputFormatPair = ExpandPair<CODECRAWFORMAT>(vecCodecFourcc, vecSupportedInputFormat);
vector<CODECRAWFORMAT> vecSupportedOutputFormatPair = ExpandPair<CODECRAWFORMAT>(vecCodecFourcc, vecSupportedOutputFormat);
vector<CODECRAWFORMAT> vecUnsupportedInputFormatPair = ExpandPair<CODECRAWFORMAT>(vecCodecFourcc, vecUnsupportedInputFormat);
vector<CODECRAWFORMAT> vecUnsupportedOutputFormatPair = ExpandPair<CODECRAWFORMAT>(vecCodecFourcc, vecUnsupportedOutputFormat);

vector<pair<pair<DWORD, DWORD>, LONG> > vecSupportedWidthPair = ExpandPair<pair<pair<DWORD, DWORD>, LONG> >(vecTopPriorityRawFormat, vecSupportedWidth);
vector<pair<pair<DWORD, DWORD>, LONG> > vecSupportedHeightPair = ExpandPair<pair<pair<DWORD, DWORD>, LONG> >(vecTopPriorityRawFormat, vecSupportedHeight);
vector<pair<pair<DWORD, DWORD>, LONG> > vecUnsupportedWidthPair = ExpandPair<pair<pair<DWORD, DWORD>, LONG> >(vecTopPriorityRawFormat, vecUnsupportedWidth);
vector<pair<pair<DWORD, DWORD>, LONG> > vecUnsupportedHeightPair = ExpandPair<pair<pair<DWORD, DWORD>, LONG> >(vecTopPriorityRawFormat, vecUnsupportedHeight);

// ÉeÉXÉgÉPÅ[ÉXé©ëÃÇÃÉeÉXÉg
BOOST_AUTO_TEST_CASE(vcm_fmt)
{
	BOOST_CHECK(vecCodecFourcc.size() == vecCodecName.size());
	BOOST_CHECK(vecCodecFourcc.size() == vecSupportedInputFormat.size());
	BOOST_CHECK(vecCodecFourcc.size() == vecSupportedOutputFormat.size());
	BOOST_CHECK(vecCodecFourcc.size() == vecUnsupportedInputFormat.size());
	BOOST_CHECK(vecCodecFourcc.size() == vecUnsupportedOutputFormat.size());
}
