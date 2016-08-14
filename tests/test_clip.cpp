/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#pragma warning(disable:4073)
#pragma init_seg(lib)

#include "test_clip.h"

vector<tuple<string, string, unsigned int>> operator+(vector<tuple<string, string, unsigned int>> a, vector<tuple<string, string, unsigned int>> b)
{
	vector<tuple<string, string, unsigned int>> ret;

	ret = a;
	ret.insert(ret.end(), b.begin(), b.end());
	return ret;
}

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip000 = {
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-progressive", 1, 1, { { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-progressive", 1, 1, { { "clip000-raw-rgba", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-interlace", 1, 2,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-interlace", 1, 2,{ { "clip000-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
};

static int divs[] = { 1, 8, 11 };
static string preds[] = { "left", "median" };

vector<tuple<string, string, unsigned int>> conv_clip000(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v)
{
	static pair<int, int> sizes[] = { { 384,256 },{ 383,256 },{ 382,256 },{ 381,256 },{ 384,255 },{ 384,254 },{ 384,253 },{ 384,512 } };

	vector<tuple<string, string, unsigned int>> r;

	for (auto& i : v)
	{
		auto& compbase = std::get<0>(i);
		auto widthstep = std::get<1>(i);
		auto heightstep = std::get<2>(i);
		for (auto& j : std::get<3>(i))
		{
			auto& rawbase = j.first;
			auto& tolerance = j.second;
			for (auto div : divs)
			{
				for (auto& pred : preds)
				{
					for (auto& size : sizes)
					{
						auto width = size.first;
						auto height = size.second;
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-div%d-%dx%d.avi", compbase.c_str(), pred.c_str(), div, width, height);
						sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(comp, raw, tolerance));
					}
				}
			}
		}
	}

	return r;
}

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip001 = {
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-progressive", 1, 1,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-progressive", 1, 1,{ { "clip000-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-progressive", 2, 1,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-progressive", 2, 2,{ { "clip000-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh2-progressive", 2, 1,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh0-progressive", 2, 2,{ { "clip000-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-interlace", 1, 2,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-interlace", 1, 2,{ { "clip000-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 }, }),
};

vector<tuple<string, string, unsigned int>> vecDecodeClips = conv_clip000(vecDecodeClipsSource_clip000) + conv_clip000(vecDecodeClipsSource_clip001);
