/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICGetStateSize, data::make(vecCodecFcc), fcc)
{
	HIC hic;
	LRESULT lr;

	hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);

	ICGetStateSize(hic);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
