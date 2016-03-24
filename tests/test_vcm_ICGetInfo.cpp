/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

BOOST_DATA_TEST_CASE(vcm_ICGetInfo, data::make(vecCodecFourcc) ^ data::make(vecCodecShortName) ^ data::make(vecCodecLongName), fcc, wstrShortName, wstrLongName)
{
	HIC hic;
	LRESULT lr;
	ICINFO info;

	hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_QUERY);
	BOOST_REQUIRE(hic != (HIC)NULL);

	lr = ICGetInfo(hic, &info, sizeof(info));
	BOOST_TEST_REQUIRE(lr > (LRESULT)offsetof(ICINFO, szDriver));

	BOOST_CHECK_EQUAL(info.fccHandler, fcc);
	BOOST_CHECK_EQUAL(wstring(info.szName), wstrShortName);
	BOOST_CHECK_EQUAL(wstring(info.szDescription), wstrLongName);
	BOOST_TEST_CHECK(info.dwFlags == 0);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
