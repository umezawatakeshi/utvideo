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

static const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip000_ulxx = {
	{"clip000-ulrg-progressive", 1, 1, { { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }},
	{"clip000-ulra-progressive", 1, 1, { { "clip000-raw-rgba", 0 }, }},
	{"clip000-uly4-progressive", 1, 1, { { "clip000-raw-yv24", 0 }, }},
	{"clip000-uly2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }},
	{"clip000-uly0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }},
	{"clip000-uly0-progressive", 4, 2, { { "clip000-raw-nv12", 0 }, }},
	{"clip000-ulh4-progressive", 1, 1, { { "clip000-raw-yv24", 0 }, }},
	{"clip000-ulh2-progressive", 2, 1, { { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }},
	{"clip000-ulh0-progressive", 2, 2, { { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }},
	{"clip000-ulh0-progressive", 4, 2, { { "clip000-raw-nv12", 0 }, }},
	{"clip000-ulrg-interlace", 1, 2,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }},
	{"clip000-ulra-interlace", 1, 2,{ { "clip000-raw-rgba", 0 }, }},
	{"clip000-uly4-interlace", 1, 2,{ { "clip000-raw-yv24", 0 }, }},
	{"clip000-uly2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }},
	{"clip000-uly0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }},
	{"clip000-uly0-interlace", 4, 4,{ { "clip000-raw-nv12", 0 }, }},
	{"clip000-ulh4-interlace", 1, 2,{ { "clip000-raw-yv24", 0 }, }},
	{"clip000-ulh2-interlace", 2, 2,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }},
	{"clip000-ulh0-interlace", 2, 4,{ { "clip000-raw-yv12", 0 },{ "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 }, }},
	{"clip000-ulh0-interlace", 4, 4,{ { "clip000-raw-nv12", 0 }, }},
	{"clip001-ulrg-progressive", 1, 1,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }},
	{"clip001-ulra-progressive", 1, 1,{ { "clip001-raw-rgba", 0 }, }},
	{"clip001-uly4-progressive", 1, 1,{ { "clip001-raw-yv24", 0 }, }},
	{"clip001-uly2-progressive", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }},
	{"clip001-uly0-progressive", 2, 2,{ { "clip001-raw-yv12", 0 }, }},
	{"clip001-uly0-progressive", 4, 2,{ { "clip001-raw-nv12", 0 }, }},
	{"clip001-ulh4-progressive", 1, 1,{ { "clip001-raw-yv24", 0 }, }},
	{"clip001-ulh2-progressive", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }},
	{"clip001-ulh0-progressive", 2, 2,{ { "clip001-raw-yv12", 0 }, }},
	{"clip001-ulh0-progressive", 4, 2,{ { "clip001-raw-nv12", 0 }, }},
	{"clip001-ulrg-interlace", 1, 2,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }},
	{"clip001-ulra-interlace", 1, 2,{ { "clip001-raw-rgba", 0 }, }},
	{"clip001-uly4-interlace", 1, 2,{ { "clip001-raw-yv24", 0 }, }},
	{"clip001-uly2-interlace", 2, 2,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }},
	{"clip001-uly0-interlace", 2, 4,{ { "clip001-raw-yv12", 0 }, }},
	{"clip001-uly0-interlace", 4, 4,{ { "clip001-raw-nv12", 0 }, }},
	{"clip001-ulh4-interlace", 1, 2,{ { "clip001-raw-yv24", 0 }, }},
	{"clip001-ulh2-interlace", 2, 2,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }},
	{"clip001-ulh0-interlace", 2, 4,{ { "clip001-raw-yv12", 0 }, }},
	{"clip001-ulh0-interlace", 4, 4,{ { "clip001-raw-nv12", 0 }, }},
	{"clip100-uly0-progressive", 2, 2,{ { "clip100-raw-yuy2-yv12-yuy2-progressive", 0 },{ "clip100-raw-yuy2-yv12-uyvy-progressive", 0 },{ "clip100-raw-yuy2-yv12-yv16-progressive", 0 }, }},
	{"clip100-ulh0-progressive", 2, 2,{ { "clip100-raw-yuy2-yv12-yuy2-progressive", 0 },{ "clip100-raw-yuy2-yv12-uyvy-progressive", 0 },{ "clip100-raw-yuy2-yv12-yv16-progressive", 0 }, }},
	{"clip100-uly0-interlace", 2, 4,{ { "clip100-raw-yuy2-yv12-yuy2-interlace", 0 },{ "clip100-raw-yuy2-yv12-uyvy-interlace", 0 },{ "clip100-raw-yuy2-yv12-yv16-interlace", 0 }, }},
	{"clip100-ulh0-interlace", 2, 4,{ { "clip100-raw-yuy2-yv12-yuy2-interlace", 0 },{ "clip100-raw-yuy2-yv12-uyvy-interlace", 0 },{ "clip100-raw-yuy2-yv12-yv16-interlace", 0 }, }},
};

static const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip002_ulxx = {
	{"clip002-ulrg-progressive", 1, 1,{ { "clip002-raw-rgb24-progressive", 0 },{ "clip002-raw-rgb32-progressive", 0 }, }},
	{"clip002-ulra-progressive", 1, 1,{ { "clip002-raw-rgba-progressive", 0 }, }},
	{"clip002-uly4-progressive", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }},
	{"clip002-uly2-progressive", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }},
	{"clip002-uly0-progressive", 2, 2,{ { "clip002-raw-yv12-progressive", 0 }, }},
	{"clip002-uly0-progressive", 4, 2,{ { "clip002-raw-nv12-progressive", 0 }, }},
	{"clip002-ulh4-progressive", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }},
	{"clip002-ulh2-progressive", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }},
	{"clip002-ulh0-progressive", 2, 2,{ { "clip002-raw-yv12-progressive", 0 }, }},
	{"clip002-ulh0-progressive", 4, 2,{ { "clip002-raw-nv12-progressive", 0 }, }},
	{"clip002-ulrg-interlace", 1, 2,{ { "clip002-raw-rgb24-interlace", 0 },{ "clip002-raw-rgb32-interlace", 0 }, }},
	{"clip002-ulra-interlace", 1, 2,{ { "clip002-raw-rgba-interlace", 0 }, }},
	{"clip002-uly4-interlace", 1, 2,{ { "clip002-raw-yv24-interlace", 0 }, }},
	{"clip002-uly2-interlace", 2, 2,{ { "clip002-raw-yuy2-interlace", 0 },{ "clip002-raw-uyvy-interlace", 0 },{ "clip002-raw-yv16-interlace", 0 }, }},
	{"clip002-uly0-interlace", 2, 4,{ { "clip002-raw-yv12-interlace", 0 }, }},
	{"clip002-uly0-interlace", 4, 4,{ { "clip002-raw-nv12-interlace", 0 }, }},
	{"clip002-ulh4-interlace", 1, 2,{ { "clip002-raw-yv24-interlace", 0 }, }},
	{"clip002-ulh2-interlace", 2, 2,{ { "clip002-raw-yuy2-interlace", 0 },{ "clip002-raw-uyvy-interlace", 0 },{ "clip002-raw-yv16-interlace", 0 }, }},
	{"clip002-ulh0-interlace", 2, 4,{ { "clip002-raw-yv12-interlace", 0 }, }},
	{"clip002-ulh0-interlace", 4, 4,{ { "clip002-raw-nv12-interlace", 0 }, }},
};

static const vector<pair<int, int>> sizes_clip000_ulxx = { { 384,256 },{ 383,256 },{ 382,256 },{ 381,256 },{ 384,255 },{ 384,254 },{ 384,253 },{ 384,512 } };

static const vector<pair<string, vector<uint8_t>>> preds_ulxx = {
	{ "left",{ 0x00, 0x01, 0x00, 0x00, } },
	{ "gradient",{ 0x00, 0x02, 0x00, 0x00, } },
	{ "median",{ 0x00, 0x03, 0x00, 0x00, } },
};

static const vector<pair<string, vector<uint8_t>>> divs_ulxx = {
	{ "div1",{ 0x00, 0x00, 0x00, 0x00, } },
	{ "div8",{ 0x07, 0x00, 0x00, 0x00, } },
	{ "div11",{ 0x0a, 0x00, 0x00, 0x00, } },
};

vector<tuple<string, string, unsigned int>> conv_clip000_ulxx(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
	vector<tuple<string, string, unsigned int>> r;

	for (auto& [compbase, widthstep, heightstep, raws] : v)
	{
		for (auto& [rawbase, tolerance] : raws)
		{
			for (auto& [divname, divconf] : divs_ulxx)
			{
				for (auto& [predname, predconf] : preds_ulxx)
				{
					for (auto& [width, height] : sz)
					{
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%s-%dx%d.avi", compbase.c_str(), predname.c_str(), divname.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-%s-%s-%dx%d.avi", rawbase.c_str(), predname.c_str(), divname.c_str(), width, height);
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

static const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip000_uqxx = {
	{ "clip000-uqrg", 1, 1,{ { "clip000-raw-rgb10bit_b64a", 0 },{ "clip000-raw-rgb10bit_r210", 0 }, } },
	{ "clip000-uqra", 1, 1,{ { "clip000-raw-rgba10bit_b64a", 0 }, } },
	{ "clip000-uqy2", 2, 1,{ { "clip000-raw-yuv10bit_v210", 0 },{ "clip000-raw-yuv10bit_p210", 0 },{ "clip000-raw-yuv10bit_p216", 0 },{ "clip000-raw-yuv10bit_yuv422p16le", 0 }, } },
	{ "clip000-uqy0", 2, 2,{ { "clip000-raw-yuv10bit_p010", 0 },{ "clip000-raw-yuv10bit_p016", 0 },{ "clip000-raw-yuv10bit_yuv420p16le", 0 }, } },

	{ "clip001-uqrg", 1, 1,{ { "clip001-raw-rgb10bit_b64a", 0 },{ "clip001-raw-rgb10bit_r210", 0 }, } },
	{ "clip001-uqra", 1, 1,{ { "clip001-raw-rgba10bit_b64a", 0 }, } },
	{ "clip001-uqy2", 2, 1,{ { "clip001-raw-yuv10bit_v210", 0 },{ "clip001-raw-yuv10bit_p210", 0 },{ "clip001-raw-yuv10bit_p216", 0 },{ "clip001-raw-yuv10bit_yuv422p16le", 0 }, } },
	{ "clip001-uqy0", 2, 2,{ { "clip001-raw-yuv10bit_p010", 0 },{ "clip001-raw-yuv10bit_p016", 0 },{ "clip001-raw-yuv10bit_yuv420p16le", 0 }, } },
};

static const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip002_uqxx = {
	{ "clip002-uqrg", 1, 1,{ { "clip002-raw-rgb10bit_b64a", 0 },{ "clip002-raw-rgb10bit_r210", 0 }, } },
	{ "clip002-uqra", 1, 1,{ { "clip002-raw-rgba10bit_b64a", 0 }, } },
	{ "clip002-uqy2", 2, 1,{ { "clip002-raw-yuv10bit_v210", 0 },{ "clip002-raw-yuv10bit_p210", 0 },{ "clip002-raw-yuv10bit_p216", 0 },{ "clip002-raw-yuv10bit_yuv422p16le", 0 }, } },
	{ "clip002-uqy0", 2, 2,{ { "clip002-raw-yuv10bit_p010", 0 },{ "clip002-raw-yuv10bit_p016", 0 },{ "clip002-raw-yuv10bit_yuv420p16le", 0 }, } },
};

static const vector<pair<int, int>> sizes_clip000_uqxx = { { 384,256 },{ 383,256 },{ 382,256 },{ 381,256 },{ 384,255 },{ 384,254 },{ 384,253 },{ 384,512 } };

static const vector<pair<string, vector<uint8_t>>> preds_uqxx = {
	{ "left",{ 0x00, 0x00, 0x00, 0x02, } },
	{ "gradient",{ 0x00, 0x00, 0x00, 0x04, } },
};

static const vector<pair<string, vector<uint8_t>>> divs_uqxx = {
	{ "div1",{ 0x00, 0x00, 0x00, 0x00, } },
	{ "div8",{ 0x00, 0x00, 0x07, 0x00, } },
	{ "div11",{ 0x00, 0x00, 0x0a, 0x00, } },
};

vector<tuple<string, string, unsigned int>> conv_clip000_uqxx(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
	vector<tuple<string, string, unsigned int>> r;

	for (auto& [compbase, widthstep, heightstep, rows] : v)
	{
		for (auto& [rawbase, tolerance] : rows)
		{
			for (auto& [divname, divconf] : divs_uqxx)
			{
				for (auto& [predname, predconf] : preds_uqxx)
				{
					for (auto& [width, height] : sz)
					{
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%s-%dx%d.avi", compbase.c_str(), predname.c_str(), divname.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-%s-%s-%dx%d.avi", rawbase.c_str(), predname.c_str(), divname.c_str(), width, height);
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

static const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip000_umxx = {
	{"clip000-umrg", 1, 1,{ { "clip000-raw-rgb24", 0 },{ "clip000-raw-rgb32", 0 }, }},
	{"clip000-umra", 1, 1,{ { "clip000-raw-rgba", 0 }, }},
	{"clip000-umy4", 1, 1,{ { "clip000-raw-yv24", 0 }, }},
	{"clip000-umy2", 2, 1,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }},
	{"clip000-umh4", 1, 1,{ { "clip000-raw-yv24", 0 }, }},
	{"clip000-umh2", 2, 1,{ { "clip000-raw-yuy2", 0 },{ "clip000-raw-uyvy", 0 },{ "clip000-raw-yv16", 0 }, }},
	{"clip001-umrg", 1, 1,{ { "clip001-raw-rgb24", 0 },{ "clip001-raw-rgb32", 0 }, }},
	{"clip001-umra", 1, 1,{ { "clip001-raw-rgba", 0 }, }},
	{"clip001-umy4", 1, 1,{ { "clip001-raw-yv24", 0 }, }},
	{"clip001-umy2", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }},
	{"clip001-umh4", 1, 1,{ { "clip001-raw-yv24", 0 }, }},
	{"clip001-umh2", 2, 1,{ { "clip001-raw-yuy2", 0 },{ "clip001-raw-uyvy", 0 },{ "clip001-raw-yv16", 0 }, }},
};

static const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip002_umxx = {
	{"clip002-umrg", 1, 1,{ { "clip002-raw-rgb24-progressive", 0 },{ "clip002-raw-rgb32-progressive", 0 }, }},
	{"clip002-umra", 1, 1,{ { "clip002-raw-rgba-progressive", 0 }, }},
	{"clip002-umy4", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }},
	{"clip002-umy2", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }},
	{"clip002-umh4", 1, 1,{ { "clip002-raw-yv24-progressive", 0 }, }},
	{"clip002-umh2", 2, 1,{ { "clip002-raw-yuy2-progressive", 0 },{ "clip002-raw-uyvy-progressive", 0 },{ "clip002-raw-yv16-progressive", 0 }, }},
};

static const vector<pair<int, int>> sizes_clip000_umxx = { { 384,256 },{ 383,256 },{ 382,256 },{ 381,256 },{ 352,256 },{ 322,256 },{ 320,256 },{ 318,256 },{ 384,255 },{ 384,254 },{ 384,253 },{ 384,512 } };

static const vector<pair<string, vector<uint8_t>>> divs_umxx = {
	{ "div1",{ 0x00, 0x00, 0x00, 0x00, } },
	{ "div8",{ 0x00, 0x07, 0x00, 0x00, } },
	{ "div11",{ 0x00, 0x0a, 0x00, 0x00, } },
};

static const vector<vector<uint8_t>> intra_umxx = {
	{ 0x00, 0x00, 0x00, 0x00, }, // no-temporal, k1
	{ 0x00, 0x00, 0x1e, 0x00, }, // no-temporal, k30
	{ 0x02, 0x00, 0x00, 0x00, }, // temporal, k1
};

vector<tuple<string, string, unsigned int>> conv_clip000_umxx(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
	vector<tuple<string, string, unsigned int>> r;

	for (auto& [compbase, widthstep, heightstep, rows] : v)
	{
		for (auto& [rawbase, tolerance] : rows)
		{
			for (auto& [divname, divconf] : divs_umxx)
			{
				for (auto& [width, height] : sz)
				{
					if (width % widthstep != 0 || height % heightstep != 0)
						continue;

					char comp[256], raw[256];

					sprintf(comp, "%s-%s-%dx%d.avi", compbase.c_str(), divname.c_str(), width, height);
					if (rawprop)
						sprintf(raw, "%s-gradient-%s-%dx%d.avi", rawbase.c_str(), divname.c_str(), width, height);
					else
						sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
					r.push_back(make_tuple(comp, raw, tolerance));
				}
			}
		}
	}

	return r;
}

static const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> vecDecodeClipsSource_clip003_umxx = {
	{ "clip003-umrg", 1, 1,{ { "clip003-raw-rgb24", 0 },{ "clip003-raw-rgb32", 0 }, } },
	{ "clip003-umra", 1, 1,{ { "clip003-raw-rgba", 0 }, } },
	{ "clip003-umy4", 1, 1,{ { "clip003-raw-yv24", 0 }, } },
	{ "clip003-umy2", 2, 1,{ { "clip003-raw-yuy2", 0 },{ "clip003-raw-uyvy", 0 },{ "clip003-raw-yv16", 0 }, } },
	{ "clip003-umh4", 1, 1,{ { "clip003-raw-yv24", 0 }, } },
	{ "clip003-umh2", 2, 1,{ { "clip003-raw-yuy2", 0 },{ "clip003-raw-uyvy", 0 },{ "clip003-raw-yv16", 0 }, } },
};

static const vector<uint8_t> inter_umxx = { 0x02, 0x00, 0x02, 0x00, };

vector<tuple<string, string, unsigned int>> conv_clip003_umxx(const vector<tuple<string, int, int, vector<pair<string, unsigned int>>>> &v, const vector<pair<int, int>>& sz)
{
	vector<tuple<string, string, unsigned int>> r;

	for (auto& [compbase, widthstep, heightstep, rows] : v)
	{
		for (auto& [rawbase, tolerance] : rows)
		{
			for (auto& [divname, divconf] : divs_umxx)
			{
				for (auto& [width, height] : sz)
				{
					if (width % widthstep != 0 || height % heightstep != 0)
						continue;

					char comp[256], raw[256];

					sprintf(comp, "%s-%s-%dx%d.avi", compbase.c_str(), divname.c_str(), width, height);
					sprintf(raw, "%s-%s-%dx%d.avi", rawbase.c_str(), divname.c_str(), width, height);
					r.push_back(make_tuple(comp, raw, tolerance));
				}
			}
		}
	}

	return r;
}

const vector<tuple<string, string, unsigned int>> vecDecodeClips =
	conv_clip000_ulxx(vecDecodeClipsSource_clip000_ulxx, sizes_clip000_ulxx, false) +
	conv_clip000_ulxx(vecDecodeClipsSource_clip002_ulxx, sizes_clip000_ulxx, true) +
	conv_clip000_uqxx(vecDecodeClipsSource_clip000_uqxx, sizes_clip000_uqxx, false) +
	conv_clip000_uqxx(vecDecodeClipsSource_clip002_uqxx, sizes_clip000_uqxx, true) +
	conv_clip000_umxx(vecDecodeClipsSource_clip000_umxx, sizes_clip000_umxx, false) +
	conv_clip000_umxx(vecDecodeClipsSource_clip002_umxx, sizes_clip000_umxx, true) +
	conv_clip003_umxx(vecDecodeClipsSource_clip003_umxx, sizes_clip000_umxx);

/****************************************************************************/

static const vector<uint8_t> ulxx_extradata_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
static const vector<uint8_t> ulxx_config_progressive = { 0x00, 0x00, 0x00, 0x00, };
static const vector<uint8_t> ulxx_config_interlace   = { 0x00, 0x08, 0x00, 0x00, };

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip000_ulxx = {
	{"clip000-ulrg-progressive", ulxx_config_progressive, 1, 1,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }},
	{"clip000-ulra-progressive", ulxx_config_progressive, 1, 1,{ "clip000-raw-rgba", }},
	{"clip000-uly4-progressive", ulxx_config_progressive, 1, 1,{ "clip000-raw-yv24", }},
	{"clip000-uly2-progressive", ulxx_config_progressive, 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }},
	{"clip000-uly0-progressive", ulxx_config_progressive, 2, 2,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }},
	{"clip000-uly0-progressive", ulxx_config_progressive, 4, 2,{ "clip000-raw-nv12", }},
	{"clip000-ulh4-progressive", ulxx_config_progressive, 1, 1,{ "clip000-raw-yv24", }},
	{"clip000-ulh2-progressive", ulxx_config_progressive, 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }},
	{"clip000-ulh0-progressive", ulxx_config_progressive, 2, 2,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }},
	{"clip000-ulh0-progressive", ulxx_config_progressive, 4, 2,{ "clip000-raw-nv12", }},
	{"clip000-ulrg-interlace", ulxx_config_interlace, 1, 2,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }},
	{"clip000-ulra-interlace", ulxx_config_interlace, 1, 2,{ "clip000-raw-rgba",  }},
	{"clip000-uly4-interlace", ulxx_config_interlace, 1, 2,{ "clip000-raw-yv24", }},
	{"clip000-uly2-interlace", ulxx_config_interlace, 2, 2,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }},
	{"clip000-uly0-interlace", ulxx_config_interlace, 2, 4,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }},
	{"clip000-uly0-interlace", ulxx_config_interlace, 4, 4,{ "clip000-raw-nv12", }},
	{"clip000-ulh4-interlace", ulxx_config_interlace, 1, 2,{ "clip000-raw-yv24", }},
	{"clip000-ulh2-interlace", ulxx_config_interlace, 2, 2,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }},
	{"clip000-ulh0-interlace", ulxx_config_interlace, 2, 4,{ "clip000-raw-yv12", "clip000-raw-yuy2", "clip000-raw-uyvy", }},
	{"clip000-ulh0-interlace", ulxx_config_interlace, 4, 4,{ "clip000-raw-nv12", }},
	{"clip001-ulrg-progressive", ulxx_config_progressive, 1, 1,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }},
	{"clip001-ulra-progressive", ulxx_config_progressive, 1, 1,{ "clip001-raw-rgba", }},
	{"clip001-uly4-progressive", ulxx_config_progressive, 1, 1,{ "clip001-raw-yv24", }},
	{"clip001-uly2-progressive", ulxx_config_progressive, 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }},
	{"clip001-uly0-progressive", ulxx_config_progressive, 2, 2,{ "clip001-raw-yv12", }},
	{"clip001-uly0-progressive", ulxx_config_progressive, 4, 2,{ "clip001-raw-nv12", }},
	{"clip001-ulh4-progressive", ulxx_config_progressive, 1, 1,{ "clip001-raw-yv24", }},
	{"clip001-ulh2-progressive", ulxx_config_progressive, 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }},
	{"clip001-ulh0-progressive", ulxx_config_progressive, 2, 2,{ "clip001-raw-yv12", }},
	{"clip001-ulh0-progressive", ulxx_config_progressive, 4, 2,{ "clip001-raw-nv12", }},
	{"clip001-ulrg-interlace", ulxx_config_interlace, 1, 2,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }},
	{"clip001-ulra-interlace", ulxx_config_interlace, 1, 2,{ "clip001-raw-rgba", }},
	{"clip001-uly4-interlace", ulxx_config_interlace, 1, 2,{ "clip001-raw-yv24", }},
	{"clip001-uly2-interlace", ulxx_config_interlace, 2, 2,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }},
	{"clip001-uly0-interlace", ulxx_config_interlace, 2, 4,{ "clip001-raw-yv12", }},
	{"clip001-uly0-interlace", ulxx_config_interlace, 4, 4,{ "clip001-raw-nv12", }},
	{"clip001-ulh4-interlace", ulxx_config_interlace, 1, 2,{ "clip001-raw-yv24", }},
	{"clip001-ulh2-interlace", ulxx_config_interlace, 2, 2,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }},
	{"clip001-ulh0-interlace", ulxx_config_interlace, 2, 4,{ "clip001-raw-yv12", }},
	{"clip001-ulh0-interlace", ulxx_config_interlace, 4, 4,{ "clip001-raw-nv12", }},
	{"clip100-uly0-progressive", ulxx_config_progressive, 2, 2,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }},
	{"clip100-ulh0-progressive", ulxx_config_progressive, 2, 2,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }},
	{"clip100-uly0-interlace", ulxx_config_interlace, 2, 4,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }},
	{"clip100-ulh0-interlace", ulxx_config_interlace, 2, 4,{ "clip100-raw-yuy2", "clip100-raw-uyvy", "clip100-raw-yv16", }},
};

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip002_ulxx = {
	{"clip002-ulrg-progressive", ulxx_config_progressive, 1, 1,{ "clip002-raw-rgb24-progressive", "clip002-raw-rgb32-progressive", }},
	{"clip002-ulra-progressive", ulxx_config_progressive, 1, 1,{ "clip002-raw-rgba-progressive", }},
	{"clip002-uly4-progressive", ulxx_config_progressive, 1, 1,{ "clip002-raw-yv24-progressive", }},
	{"clip002-uly2-progressive", ulxx_config_progressive, 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }},
	{"clip002-uly0-progressive", ulxx_config_progressive, 2, 2,{ "clip002-raw-yv12-progressive", }},
	{"clip002-uly0-progressive", ulxx_config_progressive, 4, 2,{ "clip002-raw-nv12-progressive", }},
	{"clip002-ulh4-progressive", ulxx_config_progressive, 1, 1,{ "clip002-raw-yv24-progressive", }},
	{"clip002-ulh2-progressive", ulxx_config_progressive, 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }},
	{"clip002-ulh0-progressive", ulxx_config_progressive, 2, 2,{ "clip002-raw-yv12-progressive", }},
	{"clip002-ulh0-progressive", ulxx_config_progressive, 4, 2,{ "clip002-raw-nv12-progressive", }},
	{"clip002-ulrg-interlace", ulxx_config_interlace, 1, 2,{ "clip002-raw-rgb24-interlace", "clip002-raw-rgb32-interlace", }},
	{"clip002-ulra-interlace", ulxx_config_interlace, 1, 2,{ "clip002-raw-rgba-interlace", }},
	{"clip002-uly4-interlace", ulxx_config_interlace, 1, 2,{ "clip002-raw-yv24-interlace", }},
	{"clip002-uly2-interlace", ulxx_config_interlace, 2, 2,{ "clip002-raw-yuy2-interlace", "clip002-raw-uyvy-interlace", "clip002-raw-yv16-interlace", }},
	{"clip002-uly0-interlace", ulxx_config_interlace, 2, 4,{ "clip002-raw-yv12-interlace", }},
	{"clip002-uly0-interlace", ulxx_config_interlace, 4, 4,{ "clip002-raw-nv12-interlace", }},
	{"clip002-ulh4-interlace", ulxx_config_interlace, 1, 2,{ "clip002-raw-yv24-interlace", }},
	{"clip002-ulh2-interlace", ulxx_config_interlace, 2, 2,{ "clip002-raw-yuy2-interlace", "clip002-raw-uyvy-interlace", "clip002-raw-yv16-interlace", }},
	{"clip002-ulh0-interlace", ulxx_config_interlace, 2, 4,{ "clip002-raw-yv12-interlace", }},
	{"clip002-ulh0-interlace", ulxx_config_interlace, 4, 4,{ "clip002-raw-nv12-interlace", }},
};


vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> conv_enc_clip000_ulxx(const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
	vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> r;

	for (auto& [compbase, confbase, widthstep, heightstep, rawbases] : v)
	{
		for (auto& rawbase : rawbases)
		{
			for (auto& [divname, divconf] : divs_ulxx)
			{
				for (auto& [predname, predconf] : preds_ulxx)
				{
					for (auto& [width, height] : sz)
					{
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%s-%dx%d.avi", compbase.c_str(), predname.c_str(), divname.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-%s-%s-%dx%d.avi", rawbase.c_str(), predname.c_str(), divname.c_str(), width, height);
						else
							sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(raw, comp, confbase | divconf | predconf, ulxx_extradata_mask));
					}
				}
			}
		}
	}

	return r;
}


static const vector<uint8_t> uqxx_extradata_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
static const vector<uint8_t> uqxx_config_none = { 0x00, 0x00, 0x00, 0x00, };

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip000_uqxx = {
	{ "clip000-uqrg", uqxx_config_none, 1, 1,{ "clip000-raw-rgb10bit_b64a", "clip000-raw-rgb10bit_r210", } },
	{ "clip000-uqra", uqxx_config_none, 1, 1,{ "clip000-raw-rgba10bit_b64a", } },
	{ "clip000-uqy2", uqxx_config_none, 2, 1,{ "clip000-raw-yuv10bit_v210", "clip000-raw-yuv10bit_p210", "clip000-raw-yuv10bit_p216", "clip000-raw-yuv10bit_yuv422p16le", } },
	{ "clip000-uqy0", uqxx_config_none, 2, 2,{ "clip000-raw-yuv10bit_p010", "clip000-raw-yuv10bit_p016", "clip000-raw-yuv10bit_yuv420p16le", } },

	{ "clip001-uqrg", uqxx_config_none, 1, 1,{ "clip001-raw-rgb10bit_b64a", "clip001-raw-rgb10bitn_b64a", "clip001-raw-rgb10bit_r210", } },
	{ "clip001-uqra", uqxx_config_none, 1, 1,{ "clip001-raw-rgba10bit_b64a", "clip001-raw-rgba10bitn_b64a", } },
	{ "clip001-uqy2", uqxx_config_none, 2, 1,{ "clip001-raw-yuv10bit_v210", "clip001-raw-yuv10bit_p210", "clip001-raw-yuv10bit_p216", "clip001-raw-yuv10bitn_p216", "clip001-raw-yuv10bit_yuv422p16le", "clip001-raw-yuv10bitn_yuv422p16le", } },
	{ "clip001-uqy0", uqxx_config_none, 2, 2,{ "clip001-raw-yuv10bit_p010", "clip001-raw-yuv10bit_p016", "clip001-raw-yuv10bitn_p016", "clip001-raw-yuv10bit_yuv420p16le", "clip001-raw-yuv10bitn_yuv420p16le", } },
};

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip002_uqxx = {
	{ "clip002-uqrg", uqxx_config_none, 1, 1,{ "clip002-raw-rgb10bit_b64a", "clip002-raw-rgb10bit_r210", } },
	{ "clip002-uqra", uqxx_config_none, 1, 1,{ "clip002-raw-rgba10bit_b64a", } },
	{ "clip002-uqy2", uqxx_config_none, 2, 1,{ "clip002-raw-yuv10bit_v210", "clip002-raw-yuv10bit_p210", "clip002-raw-yuv10bit_p216", "clip002-raw-yuv10bit_yuv422p16le", } },
	{ "clip002-uqy0", uqxx_config_none, 2, 2,{ "clip002-raw-yuv10bit_p010", "clip002-raw-yuv10bit_p016", "clip002-raw-yuv10bit_yuv420p16le", } },
};

vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> conv_enc_clip000_uqxx(const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
	vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> r;

	for (auto& [compbase, confbase, widthstep, heightstep, rawbases] : v)
	{
		for (auto& rawbase : rawbases)
		{
			for (auto& [divname, divconf] : divs_uqxx)
			{
				for (auto& [predname, predconf] : preds_uqxx)
				{
					for (auto& [width, height] : sz)
					{
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%s-%dx%d.avi", compbase.c_str(), predname.c_str(), divname.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-%s-%s-%dx%d.avi", rawbase.c_str(), predname.c_str(), divname.c_str(), width, height);
						else
							sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(raw, comp, confbase | divconf | predconf, uqxx_extradata_mask));
					}
				}
			}
		}
	}

	return r;
}

static const vector<uint8_t> umxx_extradata_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
static const vector<uint8_t> umxx_config_none = { 0x00, 0x00, 0x00, 0x00, };

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip000_umxx = {
	{"clip000-umrg", umxx_config_none, 1, 1,{ "clip000-raw-rgb24", "clip000-raw-rgb32", }},
	{"clip000-umra", umxx_config_none, 1, 1,{ "clip000-raw-rgba", }},
	{"clip000-umy4", umxx_config_none, 1, 1,{ "clip000-raw-yv24", }},
	{"clip000-umy2", umxx_config_none, 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }},
	{"clip000-umh4", umxx_config_none, 1, 1,{ "clip000-raw-yv24", }},
	{"clip000-umh2", umxx_config_none, 2, 1,{ "clip000-raw-yuy2", "clip000-raw-uyvy", "clip000-raw-yv16", }},
	{"clip001-umrg", umxx_config_none, 1, 1,{ "clip001-raw-rgb24", "clip001-raw-rgb32", }},
	{"clip001-umra", umxx_config_none, 1, 1,{ "clip001-raw-rgba", }},
	{"clip001-umy4", umxx_config_none, 1, 1,{ "clip001-raw-yv24", }},
	{"clip001-umy2", umxx_config_none, 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }},
	{"clip001-umh4", umxx_config_none, 1, 1,{ "clip001-raw-yv24", }},
	{"clip001-umh2", umxx_config_none, 2, 1,{ "clip001-raw-yuy2", "clip001-raw-uyvy", "clip001-raw-yv16", }},
};

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip002_umxx = {
	{"clip002-umrg", umxx_config_none, 1, 1,{ "clip002-raw-rgb24-progressive", "clip002-raw-rgb32-progressive", }},
	{"clip002-umra", umxx_config_none, 1, 1,{ "clip002-raw-rgba-progressive", }},
	{"clip002-umy4", umxx_config_none, 1, 1,{ "clip002-raw-yv24-progressive", }},
	{"clip002-umy2", umxx_config_none, 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }},
	{"clip002-umh4", umxx_config_none, 1, 1,{ "clip002-raw-yv24-progressive", }},
	{"clip002-umh2", umxx_config_none, 2, 1,{ "clip002-raw-yuy2-progressive", "clip002-raw-uyvy-progressive", "clip002-raw-yv16-progressive", }},
};

vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> conv_enc_clip000_umxx(const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> &v, const vector<pair<int, int>>& sz, bool rawprop)
{
	vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> r;

	for (auto& [compbase, confbase, widthstep, heightstep, rawbases] : v)
	{
		for (auto& rawbase : rawbases)
		{
			for (auto& [divname, divconf] : divs_umxx)
			{
				for (auto& intra : intra_umxx)
				{
					for (auto& [width, height] : sz)
					{
						if (width % widthstep != 0 || height % heightstep != 0)
							continue;

						char comp[256], raw[256];

						sprintf(comp, "%s-%s-%dx%d.avi", compbase.c_str(), divname.c_str(), width, height);
						if (rawprop)
							sprintf(raw, "%s-gradient-%s-%dx%d.avi", rawbase.c_str(), divname.c_str(), width, height);
						else
							sprintf(raw, "%s-%dx%d.avi", rawbase.c_str(), width, height);
						r.push_back(make_tuple(raw, comp, confbase | divconf | intra, umxx_extradata_mask));
					}
				}
			}
		}
	}

	return r;
}

static const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> vecEncodeClipsSource_clip003_umxx = {
	{ "clip003-umrg", umxx_config_none, 1, 1,{ "clip003-raw-rgb24", "clip003-raw-rgb32", } },
	{ "clip003-umra", umxx_config_none, 1, 1,{ "clip003-raw-rgba", } },
	{ "clip003-umy4", umxx_config_none, 1, 1,{ "clip003-raw-yv24", } },
	{ "clip003-umy2", umxx_config_none, 2, 1,{ "clip003-raw-yuy2", "clip003-raw-uyvy", "clip003-raw-yv16", } },
	{ "clip003-umh4", umxx_config_none, 1, 1,{ "clip003-raw-yv24", } },
	{ "clip003-umh2", umxx_config_none, 2, 1,{ "clip003-raw-yuy2", "clip003-raw-uyvy", "clip003-raw-yv16", } },
};

vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> conv_enc_clip003_umxx(const vector<tuple<string, vector<uint8_t>, int, int, vector<string>>> &v, const vector<pair<int, int>>& sz)
{
	vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> r;

	for (auto& [compbase, confbase, widthstep, heightstep, rawbases] : v)
	{
		for (auto& rawbase : rawbases)
		{
			for (auto& [divname, divconf] : divs_umxx)
			{
				for (auto& [width, height] : sz)
				{
					if (width % widthstep != 0 || height % heightstep != 0)
						continue;

					char comp[256], raw[256];

					sprintf(comp, "%s-%s-%dx%d.avi", compbase.c_str(), divname.c_str(), width, height);
					sprintf(raw, "%s-%s-%dx%d.avi", rawbase.c_str(), divname.c_str(), width, height);
					r.push_back(make_tuple(raw, comp, confbase | divconf | inter_umxx, umxx_extradata_mask));
				}
			}
		}
	}

	return r;
}

const vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> vecEncodeClips =
	conv_enc_clip000_ulxx(vecEncodeClipsSource_clip000_ulxx, sizes_clip000_ulxx, false) +
	conv_enc_clip000_ulxx(vecEncodeClipsSource_clip002_ulxx, sizes_clip000_ulxx, true) +
	conv_enc_clip000_uqxx(vecEncodeClipsSource_clip000_uqxx, sizes_clip000_uqxx, false) +
	conv_enc_clip000_uqxx(vecEncodeClipsSource_clip002_uqxx, sizes_clip000_uqxx, true) +
	conv_enc_clip000_umxx(vecEncodeClipsSource_clip000_umxx, sizes_clip000_umxx, false) +
	conv_enc_clip000_umxx(vecEncodeClipsSource_clip002_umxx, sizes_clip000_umxx, true) +
	conv_enc_clip003_umxx(vecEncodeClipsSource_clip003_umxx, sizes_clip000_umxx);

/****************************************************************************/

static const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> vecEncDecClipsSource_clip200_ulxx = {
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt601" }, 'ULY2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt601" }, 'ULY2', ulxx_config_interlace, 2, 2, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt709" }, 'ULH2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt709" }, 'ULH2', ulxx_config_interlace, 2, 2, 1},

	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt601" }, 'ULY4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt601" }, 'ULY4', ulxx_config_interlace, 1, 2, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt709" }, 'ULH4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt709" }, 'ULH4', ulxx_config_interlace, 1, 2, 1},

	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt601-progressive" }, 'ULY0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt601-interlace" }, 'ULY0', ulxx_config_interlace, 2, 4, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt709-progressive" }, 'ULH0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv12-bt709-interlace" }, 'ULH0', ulxx_config_interlace, 2, 4, 1},

	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt601" }, 'ULY2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt601" }, 'ULY2', ulxx_config_interlace, 2, 2, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt709" }, 'ULH2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt709" }, 'ULH2', ulxx_config_interlace, 2, 2, 1},

	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt601" }, 'ULY4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt601" }, 'ULY4', ulxx_config_interlace, 1, 2, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt709" }, 'ULH4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt709" }, 'ULH4', ulxx_config_interlace, 1, 2, 1},

	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt601" }, 'ULY0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt601" }, 'ULY0', ulxx_config_interlace, 2, 4, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt709" }, 'ULH0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv12-bt709" }, 'ULH0', ulxx_config_interlace, 2, 4, 1},

	{{ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb32-bt601" }, 'ULY4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb32-bt601" }, 'ULY4', ulxx_config_interlace, 1, 2, 1},
	{{ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb32-bt709" }, 'ULH4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb32-bt709" }, 'ULH4', ulxx_config_interlace, 1, 2, 1},

	{{ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb32-bt601" }, 'ULY2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb32-bt601" }, 'ULY2', ulxx_config_interlace, 2, 2, 1},
	{{ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb32-bt709" }, 'ULH2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb32-bt709" }, 'ULH2', ulxx_config_interlace, 2, 2, 1},

	{{ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt601-progressive", "clip202-raw-yv12-rgb32-bt601-progressive" }, 'ULY0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt601-interlace", "clip202-raw-yv12-rgb32-bt601-interlace" }, 'ULY0', ulxx_config_interlace, 2, 4, 1},
	{{ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt709-progressive", "clip202-raw-yv12-rgb32-bt709-progressive" }, 'ULH0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip202-raw-yv12" },{ "clip202-raw-yv12-rgb24-bt709-interlace", "clip202-raw-yv12-rgb32-bt709-interlace" }, 'ULH0', ulxx_config_interlace, 2, 4, 1},

	{{ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt601", "clip203-raw-yv24-rgb32-bt601" }, 'ULY4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt601", "clip203-raw-yv24-rgb32-bt601" }, 'ULY4', ulxx_config_interlace, 1, 2, 1},
	{{ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt709", "clip203-raw-yv24-rgb32-bt709" }, 'ULH4', ulxx_config_progressive, 1, 1, 1},
	{{ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt709", "clip203-raw-yv24-rgb32-bt709" }, 'ULH4', ulxx_config_interlace, 1, 2, 1},

	{{ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb32-bt601" }, 'ULY2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb32-bt601" }, 'ULY2', ulxx_config_interlace, 2, 2, 1},
	{{ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb32-bt709" }, 'ULH2', ulxx_config_progressive, 2, 1, 1},
	{{ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb32-bt709" }, 'ULH2', ulxx_config_interlace, 2, 2, 1},

	{{ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb32-bt601" }, 'ULY0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb32-bt601" }, 'ULY0', ulxx_config_interlace, 2, 4, 1},
	{{ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb32-bt709" }, 'ULH0', ulxx_config_progressive, 2, 2, 1},
	{{ "clip203-raw-yv12" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb32-bt709" }, 'ULH0', ulxx_config_interlace, 2, 4, 1},
};

vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> conv_encdec_clip200_ulxx(const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> &v, const vector<pair<int, int>>& sz, bool srcprop, bool dstprop)
{
	vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> r;

	for (auto& [srcbases, dstbases, codecfmt, confbase, widthstep, heightstep, tolerance] : v)
	{
		for (auto& srcbase : srcbases)
		{
			for (auto& dstbase : dstbases)
			{
				for (auto& [divname, divconf] : divs_ulxx)
				{
					for (auto& [predname, predconf] : preds_ulxx)
					{
						for (auto& [width, height] : sz)
						{
							if (width % widthstep != 0 || height % heightstep != 0)
								continue;

							char src[256], dst[256];

							if (srcprop)
								sprintf(src, "%s-%s-%s-%dx%d.avi", srcbase.c_str(), predname.c_str(), divname.c_str(), width, height);
							else
								sprintf(src, "%s-%dx%d.avi", srcbase.c_str(), width, height);
							if (dstprop)
								sprintf(dst, "%s-%s-%s-%dx%d.avi", dstbase.c_str(), predname.c_str(), divname.c_str(), width, height);
							else
								sprintf(dst, "%s-%dx%d.avi", dstbase.c_str(), width, height);
							r.push_back(make_tuple(src, dst, codecfmt, confbase | divconf | predconf, tolerance));
						}
					}
				}
			}
		}
	}

	return r;
}

static const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> vecEncDecClipsSource_clip200_umxx = {
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt601" }, 'UMY2', umxx_config_none, 2, 1, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yuy2-bt709" }, 'UMH2', umxx_config_none, 2, 1, 1},

	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt601" }, 'UMY4', umxx_config_none, 1, 1, 1},
	{{ "clip200-raw-rgb24", "clip200-raw-rgb32" },{ "clip200-raw-yv24-bt709" }, 'UMH4', umxx_config_none, 1, 1, 1},

	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt601" }, 'UMY2', umxx_config_none, 2, 1, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yuy2-bt709" }, 'UMH2', umxx_config_none, 2, 1, 1},

	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt601" }, 'UMY4', umxx_config_none, 1, 1, 1},
	{{ "clip201-raw-rgb24", "clip201-raw-rgb32" },{ "clip201-raw-yv24-bt709" }, 'UMH4', umxx_config_none, 1, 1, 1},

	{{ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb32-bt601" }, 'UMY4', umxx_config_none, 1, 1, 1},
	{{ "clip202-raw-yv24" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb32-bt709" }, 'UMH4', umxx_config_none, 1, 1, 1},

	{{ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt601", "clip202-raw-yv24-rgb32-bt601" }, 'UMY2', umxx_config_none, 2, 1, 1},
	{{ "clip202-raw-yuy2" },{ "clip202-raw-yv24-rgb24-bt709", "clip202-raw-yv24-rgb32-bt709" }, 'UMH2', umxx_config_none, 2, 1, 1},

	{{ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt601", "clip203-raw-yv24-rgb32-bt601" }, 'UMY4', umxx_config_none, 1, 1, 1},
	{{ "clip203-raw-yv24" },{ "clip203-raw-yv24-rgb24-bt709", "clip203-raw-yv24-rgb32-bt709" }, 'UMH4', umxx_config_none, 1, 1, 1},

	{{ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt601", "clip203-raw-yuy2-rgb32-bt601" }, 'UMY2', umxx_config_none, 2, 1, 1},
	{{ "clip203-raw-yuy2" },{ "clip203-raw-yuy2-rgb24-bt709", "clip203-raw-yuy2-rgb32-bt709" }, 'UMH2', umxx_config_none, 2, 1, 1},
};

vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> conv_encdec_clip200_umxx(const vector<tuple<vector<string>, vector<string>, uint32_t, vector<uint8_t>, int, int, unsigned int>> &v, const vector<pair<int, int>>& sz, bool srcprop, bool dstprop)
{
	vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> r;

	for (auto& [srcbases, dstbases, codecfmt, confbase, widthstep, heightstep, tolerance] : v)
	{
		for (auto& srcbase : srcbases)
		{
			for (auto& dstbase : dstbases)
			{
				for (auto& [divname, divconf] : divs_umxx)
				{
					for (auto intra : intra_umxx)
					{
						for (auto& [width, height] : sz)
						{
							if (width % widthstep != 0 || height % heightstep != 0)
								continue;

							char src[256], dst[256];

							if (srcprop)
								sprintf(src, "%s-%s-%dx%d.avi", srcbase.c_str(), divname.c_str(), width, height);
							else
								sprintf(src, "%s-%dx%d.avi", srcbase.c_str(), width, height);
							if (dstprop)
								sprintf(dst, "%s-%s-%dx%d.avi", dstbase.c_str(), divname.c_str(), width, height);
							else
								sprintf(dst, "%s-%dx%d.avi", dstbase.c_str(), width, height);
							r.push_back(make_tuple(src, dst, codecfmt, confbase | divconf | intra, tolerance));
						}
					}
				}
			}
		}
	}

	return r;
}

const vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> vecEncDecClips =
	conv_encdec_clip200_ulxx(vecEncDecClipsSource_clip200_ulxx, sizes_clip000_ulxx, false, false) +
	conv_encdec_clip200_umxx(vecEncDecClipsSource_clip200_umxx, sizes_clip000_umxx, false, false);
