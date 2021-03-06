/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

BOOST_DATA_TEST_CASE(vcm_ICInfo_enum, data::make(vecCodecFcc), fcc)
{
	ICINFO info;
	bool found = false;

	for (int i = 0; ICInfo(ICTYPE_VIDEO, i, &info); i++)
	{
		// info.fccHandler は小文字で返ってくるようなので大文字に変換する
		DWORD fccUpper =
			((toupper(info.fccHandler & 0xff) & 0xff)) |
			((toupper((info.fccHandler >>  8) & 0xff) & 0xff) <<  8) |
			((toupper((info.fccHandler >> 16) & 0xff) & 0xff) << 16) |
			((toupper((info.fccHandler >> 24) & 0xff) & 0xff) << 24);

		if (fccUpper == fcc)
		{
			found = true;
			break;
		}
	}

	BOOST_CHECK(found);
}

BOOST_DATA_TEST_CASE(vcm_ICInfo_get, data::make(vecCodecFcc), fcc)
{
	ICINFO info;
	BOOL b;

	b = ICInfo(ICTYPE_VIDEO, fcc, &info);
	BOOST_REQUIRE(b);

	// info.fccHandler は小文字で返ってくるようなので大文字に変換する
	DWORD fccUpper =
		((toupper(info.fccHandler & 0xff) & 0xff)) |
		((toupper((info.fccHandler >> 8) & 0xff) & 0xff) << 8) |
		((toupper((info.fccHandler >> 16) & 0xff) & 0xff) << 16) |
		((toupper((info.fccHandler >> 24) & 0xff) & 0xff) << 24);

	BOOST_TEST_CHECK(fccUpper == fcc);
}
