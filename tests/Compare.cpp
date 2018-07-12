/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "Compare.h"

int CompareFrame(const void *frame1, const void *frame2, unsigned int width, size_t size, DWORD format, int tolerance)
{
	size_t net = 0;
	size_t stride = 0;

	switch (format)
	{
	case 24:
		net = width * (size_t)3;
		stride = (net + 3) & ~(size_t)3;
		break;

	case FCC('r210'):
		net = width * (size_t)4;
		stride = (width + 63) / 64 * (size_t)256;
		break;

	// v210 にも専用の処理が必要な気がするが今のところ問題は発生していないように見える…？
	default:
		net = size;
		stride = size;
	}

	const uint8_t *begin1 = (const uint8_t *)frame1;
	const uint8_t *begin2 = (const uint8_t *)frame2;
	const uint8_t *end1 = begin1 + size;
	for (auto p = begin1, q = begin2; p < end1; p += stride, q += stride)
	{
		for (auto pp = p, qq = q; pp != p + net; ++pp, ++qq)
		{
			int r = (int)*pp - (int)*qq;
			if (abs(r) > tolerance)
				return r;
		}
	}
	return 0;
}
