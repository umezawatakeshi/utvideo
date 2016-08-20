/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#pragma warning(disable:4073)
#pragma init_seg(lib)

#include "test_clip.h"

template<typename T, typename A>
vector<T, A> operator+(const vector<T, A> &a, const vector<T, A> &b)
{
	vector<T, A> ret;

	ret = a;
	ret.insert(ret.end(), b.begin(), b.end());
	return ret;
}

template<typename T, typename A>
vector<T, A> operator|(const vector<T, A> &a, const vector<T, A> &b)
{
	if (a.size() != b.size())
		throw std::exception(); // XXX

	vector<T, A> ret(a.size());

	for (size_t i = 0; i < a.size(); i++)
		ret.at(i) = a.at(i) | b.at(i);

	return ret;
}

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip000 = {
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-progressive", 1, 1, { { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-progressive", 1, 1, { { "clip000-raw-rgba", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly4-progressive", 1, 1, { { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh4-progressive", 1, 1, { { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, } ),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-interlace", 1, 2,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-interlace", 1, 2,{ { "clip000-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly4-interlace", 1, 2,{ { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh4-interlace", 1, 2,{ { "clip000-raw-yv24", 0 }, }),
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
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulrg-progressive", 1, 1,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulra-progressive", 1, 1,{ { "clip001-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly4-progressive", 1, 1,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly2-progressive", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly0-progressive", 2, 2,{ { "clip001-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh4-progressive", 1, 1,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh2-progressive", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh0-progressive", 2, 2,{ { "clip001-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulrg-interlace", 1, 2,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulra-interlace", 1, 2,{ { "clip001-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly4-interlace", 1, 2,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly2-interlace", 2, 2,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly0-interlace", 2, 4,{ { "clip001-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh4-interlace", 1, 2,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh2-interlace", 2, 2,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh0-interlace", 2, 4,{ { "clip001-raw-yv12", 0 }, }),
};

vector<tuple<string, string, unsigned int>> vecDecodeClips = conv_clip000(vecDecodeClipsSource_clip000) + conv_clip000(vecDecodeClipsSource_clip001);

/****************************************************************************/

static vector<uint8_t> ulxx_extradata_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
static vector<uint8_t> ulxx_config_progressive = { 0x00, 0x00, 0x00, 0x00, };
static vector<uint8_t> ulxx_config_interlace   = { 0x00, 0x08, 0x00, 0x00, };

static vector<pair<string, vector<uint8_t>>> preds_ulxx = {
	{ "left",  { 0x00, 0x01, 0x00, 0x00, } },
	{ "median",{ 0x00, 0x03, 0x00, 0x00, } },
};

static vector<pair<string, vector<uint8_t>>> divs_ulxx = {
	{ "div1",  { 0x00, 0x00, 0x00, 0x00, } },
	{ "div8",  { 0x07, 0x00, 0x00, 0x00, } },
	{ "div11", { 0x0a, 0x00, 0x00, 0x00, } },
};

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip000 = {
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulrg-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulra-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulrg-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulra-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-rgba",  }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
};

static vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip001 = {
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulrg-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulra-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip001-raw-yv12", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip001-raw-yv12", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulrg-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulra-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip001-raw-yuy2", "clip001-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip001-raw-yv12", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip001-raw-yuy2", "clip001-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip001-raw-yv12", }),
};


vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> conv_enc_clip000(const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> &v)
{
	static pair<int, int> sizes[] = { { 384,256 },{ 383,256 },{ 382,256 },{ 381,256 },{ 384,255 },{ 384,254 },{ 384,253 },{ 384,512 } };

	vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> r;

	for (auto& i : v)
	{
		auto& compbase = std::get<0>(i);
		auto& confbase = std::get<1>(i);
		auto widthstep = std::get<2>(i);
		auto heightstep = std::get<3>(i);
		for (auto& rawbase : std::get<4>(i))
		{
			for (auto divpair : divs_ulxx)
			{
				for (auto& predpair : preds_ulxx)
				{
					for (auto& size : sizes)
					{
						auto width = size.first;
						auto height = size.second;
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%s-%dx%d.avi", compbase.c_str(), predpair.first.c_str(), divpair.first.c_str(), width, height);
						sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(raw, comp, confbase | divpair.second | predpair.second, ulxx_extradata_mask));
					}
				}
			}
		}
	}

	return r;
}

vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> vecEncodeClips = conv_enc_clip000(vecEncodeClipsSource_clip000) + conv_enc_clip000(vecEncodeClipsSource_clip001);
