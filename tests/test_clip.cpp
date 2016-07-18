/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"

#pragma warning(disable:4073)
#pragma init_seg(lib)

#include "test_clip.h"

static vector<pair<string, vector<pair<string, unsigned int>>>> vecDecodeClipsSource = {
	{ "a.avi", { { "b.avi", 0 }, } },
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
