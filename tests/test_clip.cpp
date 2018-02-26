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

	ret.reserve(a.size() + b.size());
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

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip000_ulxx = {
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-progressive", 1, 1, { { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-progressive", 1, 1, { { "clip000-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly4-progressive", 1, 1, { { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh4-progressive", 1, 1, { { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulrg-interlace", 1, 2,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulra-interlace", 1, 2,{ { "clip000-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly4-interlace", 1, 2,{ { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-uly0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh4-interlace", 1, 2,{ { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-ulh0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulrg-progressive", 1, 1,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulra-progressive", 1, 1,{ { "clip001-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly4-progressive", 1, 1,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly2-progressive", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly0-progressive", 2, 2,{ { "clip001-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh4-progressive", 1, 1,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh2-progressive", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh0-progressive", 2, 2,{ { "clip001-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulrg-interlace", 1, 2,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulra-interlace", 1, 2,{ { "clip001-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly4-interlace", 1, 2,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly2-interlace", 2, 2,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-uly0-interlace", 2, 4,{ { "clip001-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh4-interlace", 1, 2,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh2-interlace", 2, 2,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-ulh0-interlace", 2, 4,{ { "clip001-raw-yv12", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip100-uly0-progressive", 2, 2,{ { "clip100-raw-yuy2-yv12-yuy2-progressive", 0 },{ "clip100-raw-yuy2-yv12-uyvy-progressive", 0 },{ "clip100-raw-yuy2-yv12-yv16-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip100-ulh0-progressive", 2, 2,{ { "clip100-raw-yuy2-yv12-yuy2-progressive", 0 },{ "clip100-raw-yuy2-yv12-uyvy-progressive", 0 },{ "clip100-raw-yuy2-yv12-yv16-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip100-uly0-interlace", 2, 4,{ { "clip100-raw-yuy2-yv12-yuy2-interlace", 0 },{ "clip100-raw-yuy2-yv12-uyvy-interlace", 0 },{ "clip100-raw-yuy2-yv12-yv16-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip100-ulh0-interlace", 2, 4,{ { "clip100-raw-yuy2-yv12-yuy2-interlace", 0 },{ "clip100-raw-yuy2-yv12-uyvy-interlace", 0 },{ "clip100-raw-yuy2-yv12-yv16-interlace", 0 }, }),
};

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip002_ulxx = {
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulrg-progressive", 1, 1,{ { "clip002-raw-rgb24-progressive", 0 },{ "clip002-raw-rgb32-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulra-progressive", 1, 1,{ { "clip002-raw-rgba-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-uly4-progressive", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-uly2-progressive", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-uly0-progressive", 2, 2,{ { "clip002-raw-yv12-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulh4-progressive", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulh2-progressive", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulh0-progressive", 2, 2,{ { "clip002-raw-yv12-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulrg-interlace", 1, 2,{ { "clip002-raw-rgb24-interlace", 0 },{ "clip002-raw-rgb32-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulra-interlace", 1, 2,{ { "clip002-raw-rgba-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-uly4-interlace", 1, 2,{ { "clip002-raw-yv24-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-uly2-interlace", 2, 2,{ { "clip002-raw-yuy2-interlace", 0 },{ "clip002-raw-uyvy-interlace", 0 },{ "clip002-raw-yv16-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-uly0-interlace", 2, 4,{ { "clip002-raw-yv12-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulh4-interlace", 1, 2,{ { "clip002-raw-yv24-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulh2-interlace", 2, 2,{ { "clip002-raw-yuy2-interlace", 0 },{ "clip002-raw-uyvy-interlace", 0 },{ "clip002-raw-yv16-interlace", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-ulh0-interlace", 2, 4,{ { "clip002-raw-yv12-interlace", 0 }, }),
};

static const vector<pair<int, int>> sizes_clip000_ulxx = { { 384,256 },{ 383,256 },{ 382,256 },{ 381,256 },{ 384,255 },{ 384,254 },{ 384,253 },{ 384,512 } };

static vector<pair<string, vector<uint8_t>>> preds_ulxx = {
	{ "left",{ 0x00, 0x01, 0x00, 0x00, } },
	{ "gradient",{ 0x00, 0x02, 0x00, 0x00, } },
	{ "median",{ 0x00, 0x03, 0x00, 0x00, } },
};

static vector<pair<string, vector<uint8_t>>> divs_ulxx = {
	{ "div1",{ 0x00, 0x00, 0x00, 0x00, } },
	{ "div8",{ 0x07, 0x00, 0x00, 0x00, } },
	{ "div11",{ 0x0a, 0x00, 0x00, 0x00, } },
};

vector<tuple<string, string, unsigned int>> conv_clip000_ulxx(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
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
			for (auto divpair : divs_ulxx)
			{
				for (auto& predpair : preds_ulxx)
				{
					for (auto& size : sz)
					{
						auto width = size.first;
						auto height = size.second;
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%s-%dx%d.avi", compbase.c_str(), predpair.first.c_str(), divpair.first.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-%s-%s-%dx%d.avi", rawbase.c_str(), predpair.first.c_str(), divpair.first.c_str(), width, height);
						else
							sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(comp, raw, tolerance));
					}
				}
			}
		}
	}

	return r;
}

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip000_umxx = {
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-umrg", 1, 1,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-umra", 1, 1,{ { "clip000-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-umy4", 1, 1,{ { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-umy2", 2, 1,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-umh4", 1, 1,{ { "clip000-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip000-umh2", 2, 1,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-umrg", 1, 1,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-umra", 1, 1,{ { "clip001-raw-rgba", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-umy4", 1, 1,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-umy2", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-umh4", 1, 1,{ { "clip001-raw-yv24", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip001-umh2", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }),
};

static vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip002_umxx = {
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-umrg", 1, 1,{ { "clip002-raw-rgb24-progressive", 0 },{ "clip002-raw-rgb32-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-umra", 1, 1,{ { "clip002-raw-rgba-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-umy4", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-umy2", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-umh4", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }),
	make_tuple<string, int, int, vector<pair<string, unsigned int>>>("clip002-umh2", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }),
};

static const vector<pair<int, int>> sizes_clip000_umxx = { { 384,256 },{ 383,256 },{ 382,256 },{ 381,256 },{ 320,256 },{ 318,256 },{ 384,255 },{ 384,254 },{ 384,253 },{ 384,512 } };

static vector<pair<string, vector<uint8_t>>> divs_umxx = {
	{ "div1",{ 0x00, 0x00, 0x00, 0x00, } },
	{ "div8",{ 0x00, 0x07, 0x00, 0x00, } },
	{ "div11",{ 0x00, 0x0a, 0x00, 0x00, } },
};

static vector<vector<uint8_t>> intra_umxx = {
	{ 0x00, 0x00, 0x00, 0x00, }, // no-temporal, k1
	{ 0x00, 0x00, 0x1e, 0x00, }, // no-temporal, k30
	{ 0x02, 0x00, 0x00, 0x00, }, // temporal, k1
};

vector<tuple<string, string, unsigned int>> conv_clip000_umxx(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
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
			for (auto divpair : divs_umxx)
			{
				for (auto& size : sz)
				{
					auto width = size.first;
					auto height = size.second;
					if (width % widthstep != 0 || height % heightstep != 0)
						continue;

					char comp[256], raw[256];

					sprintf(comp, "%s-%s-%dx%d.avi", compbase.c_str(), divpair.first.c_str(), width, height);
					if (rawprop)
						sprintf(raw, "%s-gradient-%s-%dx%d.avi", rawbase.c_str(), divpair.first.c_str(), width, height);
					else
						sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
					r.push_back(make_tuple(comp, raw, tolerance));
				}
			}
		}
	}

	return r;
}

vector<tuple<string, string, unsigned int>> vecDecodeClips =
	conv_clip000_ulxx(vecDecodeClipsSource_clip000_ulxx, sizes_clip000_ulxx, false) +
	conv_clip000_ulxx(vecDecodeClipsSource_clip002_ulxx, sizes_clip000_ulxx, true) +
	conv_clip000_umxx(vecDecodeClipsSource_clip000_umxx, sizes_clip000_umxx, false) +
	conv_clip000_umxx(vecDecodeClipsSource_clip002_umxx, sizes_clip000_umxx, true);

/****************************************************************************/

static vector<uint8_t> ulxx_extradata_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
static vector<uint8_t> ulxx_config_progressive = { 0x00, 0x00, 0x00, 0x00, };
static vector<uint8_t> ulxx_config_interlace   = { 0x00, 0x08, 0x00, 0x00, };

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip000_ulxx = {
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulrg-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulra-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulrg-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulra-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-rgba",  }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-uly0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-ulh0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulrg-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulra-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip001-raw-yv12", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip001-raw-yv12", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulrg-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulra-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-uly0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip001-raw-yv12", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-ulh0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip001-raw-yv12", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip100-uly0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip100-ulh0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip100-uly0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip100-ulh0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }),
};

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip002_ulxx = {
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulrg-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip002-raw-rgb24-progressive", "clip002-raw-rgb32-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulra-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip002-raw-rgba-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-uly4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip002-raw-yv24-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-uly2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-uly0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip002-raw-yv12-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulh4-progressive", vector<uint8_t>(ulxx_config_progressive), 1, 1,{ "clip002-raw-yv24-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulh2-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulh0-progressive", vector<uint8_t>(ulxx_config_progressive), 2, 2,{ "clip002-raw-yv12-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulrg-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip002-raw-rgb24-interlace", "clip002-raw-rgb32-interlace", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulra-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip002-raw-rgba-interlace", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-uly4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip002-raw-yv24-interlace", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-uly2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip002-raw-yuy2-interlace", "clip002-raw-uyvy-interlace", "clip002-raw-yv16-interlace", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-uly0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip002-raw-yv12-interlace", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulh4-interlace", vector<uint8_t>(ulxx_config_interlace), 1, 2,{ "clip002-raw-yv24-interlace", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulh2-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 2,{ "clip002-raw-yuy2-interlace", "clip002-raw-uyvy-interlace", "clip002-raw-yv16-interlace", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-ulh0-interlace", vector<uint8_t>(ulxx_config_interlace), 2, 4,{ "clip002-raw-yv12-interlace", }),
};


vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> conv_enc_clip000_ulxx(const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
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
					for (auto& size : sz)
					{
						auto width = size.first;
						auto height = size.second;
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%s-%dx%d.avi", compbase.c_str(), predpair.first.c_str(), divpair.first.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-%s-%s-%dx%d.avi", rawbase.c_str(), predpair.first.c_str(), divpair.first.c_str(), width, height);
						else
							sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(raw, comp, confbase | divpair.second | predpair.second, ulxx_extradata_mask));
					}
				}
			}
		}
	}

	return r;
}

static vector<uint8_t> umxx_extradata_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
static const vector<uint8_t> umxx_config_none = { 0x00, 0x00, 0x00, 0x00, };

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip000_umxx = {
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-umrg", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-umra", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip000-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-umy4", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-umy2", vector<uint8_t>(umxx_config_none), 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-umh4", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip000-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip000-umh2", vector<uint8_t>(umxx_config_none), 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-umrg", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-umra", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip001-raw-rgba", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-umy4", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-umy2", vector<uint8_t>(umxx_config_none), 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-umh4", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip001-raw-yv24", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip001-umh2", vector<uint8_t>(umxx_config_none), 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }),
};

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip002_umxx = {
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-umrg", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip002-raw-rgb24-progressive", "clip002-raw-rgb32-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-umra", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip002-raw-rgba-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-umy4", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip002-raw-yv24-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-umy2", vector<uint8_t>(umxx_config_none), 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-umh4", vector<uint8_t>(umxx_config_none), 1, 1,{ "clip002-raw-yv24-progressive", }),
	make_tuple<string, vector<uint8_t>, int, int, vector<string>>("clip002-umh2", vector<uint8_t>(umxx_config_none), 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }),
};

vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> conv_enc_clip000_umxx(const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
	vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> r;

	for (auto& i : v)
	{
		auto& compbase = std::get<0>(i);
		auto& confbase = std::get<1>(i);
		auto widthstep = std::get<2>(i);
		auto heightstep = std::get<3>(i);
		for (auto& rawbase : std::get<4>(i))
		{
			for (auto divpair : divs_umxx)
			{
				for (auto& size : sz)
				{
					for (auto intra : intra_umxx)
					{
						auto width = size.first;
						auto height = size.second;
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%dx%d.avi", compbase.c_str(), divpair.first.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-gradient-%s-%dx%d.avi", rawbase.c_str(), divpair.first.c_str(), width, height);
						else
							sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(raw, comp, confbase | divpair.second | intra, umxx_extradata_mask));
					}
				}
			}
		}
	}

	return r;
}

vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> vecEncodeClips =
	conv_enc_clip000_ulxx(vecEncodeClipsSource_clip000_ulxx, sizes_clip000_ulxx, false) +
	conv_enc_clip000_ulxx(vecEncodeClipsSource_clip002_ulxx, sizes_clip000_ulxx, true) +
	conv_enc_clip000_umxx(vecEncodeClipsSource_clip000_umxx, sizes_clip000_umxx, false) +
	conv_enc_clip000_umxx(vecEncodeClipsSource_clip002_umxx, sizes_clip000_umxx, true);

/****************************************************************************/

static const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> vecEncDecClipsSource_clip200_ulxx = {
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt601-progressive" }, 'ULY0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt601-interlace" }, 'ULY0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt709-progressive" }, 'ULH0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt709-interlace" }, 'ULH0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt601" }, 'ULY0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt601" }, 'ULY0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt709" }, 'ULH0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt709" }, 'ULH0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb24-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb24-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb24-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb24-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt601-progressive", "clip202-raw-yv12-rgb24-bt601-progressive" }, 'ULY0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt601-interlace", "clip202-raw-yv12-rgb24-bt601-interlace" }, 'ULY0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt709-progressive", "clip202-raw-yv12-rgb24-bt709-progressive" }, 'ULH0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt709-interlace", "clip202-raw-yv12-rgb24-bt709-interlace" }, 'ULH0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt601", "clip203-raw-yv24-rgb24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt601", "clip203-raw-yv24-rgb24-bt601" }, 'ULY4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt709", "clip203-raw-yv24-rgb24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_progressive), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt709", "clip203-raw-yv24-rgb24-bt709" }, 'ULH4', vector<uint8_t>(ulxx_config_interlace), 1, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb24-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb24-bt601" }, 'ULY2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb24-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_progressive), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb24-bt709" }, 'ULH2', vector<uint8_t>(ulxx_config_interlace), 2, 2, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb24-bt601" }, 'ULY0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb24-bt601" }, 'ULY0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb24-bt709" }, 'ULH0', vector<uint8_t>(ulxx_config_progressive), 2, 2, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb24-bt709" }, 'ULH0', vector<uint8_t>(ulxx_config_interlace), 2, 4, 1),
};

vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> conv_encdec_clip200_ulxx(const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> &v, const vector<pair<int, int>>& sz, bool srcprop, bool dstprop)
{
	vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> r;

	for (auto& i : v)
	{
		auto codecfmt = std::get<2>(i);
		auto& confbase = std::get<3>(i);
		auto widthstep = std::get<4>(i);
		auto heightstep = std::get<5>(i);
		auto tolerance = std::get<6>(i);
		for (auto& srcbase : std::get<0>(i))
		{
			for (auto& dstbase : std::get<1>(i))
			{
				for (auto divpair : divs_ulxx)
				{
					for (auto& predpair : preds_ulxx)
					{
						for (auto& size : sz)
						{
							auto width = size.first;
							auto height = size.second;
							if (width % widthstep != 0 || height % heightstep != 0)
								continue;

							char src[256], dst[256];

							if (srcprop)
								sprintf(src, "%s-%s-%s-%dx%d.avi", srcbase.c_str(), predpair.first.c_str(), divpair.first.c_str(), width, height);
							else
								sprintf(src, "%s-%dx%d.avi", srcbase.c_str(), width, height);
							if (dstprop)
								sprintf(dst, "%s-%s-%s-%dx%d.avi", dstbase.c_str(), predpair.first.c_str(), divpair.first.c_str(), width, height);
							else
								sprintf(dst, "%s-%dx%d.avi", dstbase.c_str(), width, height);
							r.push_back(make_tuple(src, dst, codecfmt, confbase | divpair.second | predpair.second, tolerance));
						}
					}
				}
			}
		}
	}

	return r;
}

static const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> vecEncDecClipsSource_clip200_umxx = {
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt601" }, 'UMY2', vector<uint8_t>(umxx_config_none), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt709" }, 'UMH2', vector<uint8_t>(umxx_config_none), 2, 1, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt601" }, 'UMY4', vector<uint8_t>(umxx_config_none), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt709" }, 'UMH4', vector<uint8_t>(umxx_config_none), 1, 1, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt601" }, 'UMY2', vector<uint8_t>(umxx_config_none), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt709" }, 'UMH2', vector<uint8_t>(umxx_config_none), 2, 1, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt601" }, 'UMY4', vector<uint8_t>(umxx_config_none), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt709" }, 'UMH4', vector<uint8_t>(umxx_config_none), 1, 1, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb24-bt601" }, 'UMY4', vector<uint8_t>(umxx_config_none), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb24-bt709" }, 'UMH4', vector<uint8_t>(umxx_config_none), 1, 1, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb24-bt601" }, 'UMY2', vector<uint8_t>(umxx_config_none), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb24-bt709" }, 'UMH2', vector<uint8_t>(umxx_config_none), 2, 1, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt601", "clip203-raw-yv24-rgb24-bt601" }, 'UMY4', vector<uint8_t>(umxx_config_none), 1, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt709", "clip203-raw-yv24-rgb24-bt709" }, 'UMH4', vector<uint8_t>(umxx_config_none), 1, 1, 1),

	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb24-bt601" }, 'UMY2', vector<uint8_t>(umxx_config_none), 2, 1, 1),
	make_tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>({ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb24-bt709" }, 'UMH2', vector<uint8_t>(umxx_config_none), 2, 1, 1),
};

vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> conv_encdec_clip200_umxx(const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> &v, const vector<pair<int, int>>& sz, bool srcprop, bool dstprop)
{
	vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> r;

	for (auto& i : v)
	{
		auto codecfmt = std::get<2>(i);
		auto& confbase = std::get<3>(i);
		auto widthstep = std::get<4>(i);
		auto heightstep = std::get<5>(i);
		auto tolerance = std::get<6>(i);
		for (auto& srcbase : std::get<0>(i))
		{
			for (auto& dstbase : std::get<1>(i))
			{
				for (auto divpair : divs_umxx)
				{
					for (auto& size : sz)
					{
						for (auto intra : intra_umxx)
						{
							auto width = size.first;
							auto height = size.second;
							if (width % widthstep != 0 || height % heightstep != 0)
								continue;

							char src[256], dst[256];

							if (srcprop)
								sprintf(src, "%s-%s-%dx%d.avi", srcbase.c_str(), divpair.first.c_str(), width, height);
							else
								sprintf(src, "%s-%dx%d.avi", srcbase.c_str(), width, height);
							if (dstprop)
								sprintf(dst, "%s-%s-%dx%d.avi", dstbase.c_str(), divpair.first.c_str(), width, height);
							else
								sprintf(dst, "%s-%dx%d.avi", dstbase.c_str(), width, height);
							r.push_back(make_tuple(src, dst, codecfmt, confbase | divpair.second | intra, tolerance));
						}
					}
				}
			}
		}
	}

	return r;
}

vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> vecEncDecClips =
	conv_encdec_clip200_ulxx(vecEncDecClipsSource_clip200_ulxx, sizes_clip000_ulxx, false, false) +
	conv_encdec_clip200_umxx(vecEncDecClipsSource_clip200_umxx, sizes_clip000_umxx, false, false);
