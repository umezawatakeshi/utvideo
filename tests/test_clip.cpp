/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#pragma warning(disable:4073)
#pragma init_seg(lib)

#include "test_clip.h"

static vector<pair<string, vector<pair<string, unsigned int>>>> vecDecodeClipsSource = {
	{ "clip000-ulrg-progressive-left-div8-384x256.avi",{ { "clip000-raw-rgb24-384x256.avi", 0 },{ "clip000-raw-rgb32-384x256.avi", 0 }, } },
	{ "clip000-ulra-progressive-left-div8-384x256.avi",{ { "clip000-raw-rgba-384x256.avi", 0 }, } },
	{ "clip000-uly2-progressive-left-div8-384x256.avi",{ { "clip000-raw-yuy2-384x256.avi", 0 },{ "clip000-raw-uyvy-384x256.avi", 0 }, } },
	{ "clip000-uly0-progressive-left-div8-384x256.avi",{ { "clip000-raw-yv12-384x256.avi", 0 },{ "clip000-raw-yuy2-384x256.avi", 0 },{ "clip000-raw-uyvy-384x256.avi", 0 }, } },
	{ "clip000-ulh2-progressive-left-div8-384x256.avi",{ { "clip000-raw-yuy2-384x256.avi", 0 },{ "clip000-raw-uyvy-384x256.avi", 0 }, } },
	{ "clip000-ulh0-progressive-left-div8-384x256.avi",{ { "clip000-raw-yv12-384x256.avi", 0 },{ "clip000-raw-yuy2-384x256.avi", 0 },{ "clip000-raw-uyvy-384x256.avi", 0 }, } },
};

vector<tuple<string, string, unsigned int>> conv(const vector<pair<string, vector<pair<string, unsigned int>>>> &v)
{
	vector<tuple<string, string, unsigned int>> r;

	for (auto i : v)
	{
		for (auto j : i.second)
		{
			r.push_back(make_tuple(i.first, j.first, j.second));
		}
	}

	return r;
}

vector<tuple<string, string, unsigned int>> vecDecodeClips = conv(vecDecodeClipsSource);
