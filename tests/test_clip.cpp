/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#pragma warning(disable:4073)
#pragma init_seg(lib)

#include "test_clip.h"

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource = {
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

vector<tuple<string, string, unsigned int>> conv(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v)
{
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
					char comp[256], raw[256];

					sprintf(comp, "%s-%s-div%d-384x512.avi", compbase.c_str(), pred.c_str(), div);
					sprintf(raw, "%s-384x512.avi", rawbase.c_str());
					r.push_back(make_tuple(comp, raw, tolerance));

					for (int width = 384; width > 380; width -= widthstep)
					{
						sprintf(comp, "%s-%s-div%d-%dx256.avi", compbase.c_str(), pred.c_str(), div, width);
						sprintf(raw, "%s-%dx256.avi", rawbase.c_str(), width);
						r.push_back(make_tuple(comp, raw, tolerance));
					}

					for (int height = 256; height > 252; height -= heightstep)
					{
						if (height == 256)
							continue;
						sprintf(comp, "%s-%s-div%d-384x%d.avi", compbase.c_str(), pred.c_str(), div, height);
						sprintf(raw, "%s-384x%d.avi", rawbase.c_str(), height);
						r.push_back(make_tuple(comp, raw, tolerance));
					}
				}
			}
		}
	}

	return r;
}

vector<tuple<string, string, unsigned int>> vecDecodeClips = conv(vecDecodeClipsSource);
