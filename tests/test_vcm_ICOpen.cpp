/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

BOOST_DATA_TEST_CASE(vcm_ICOpen_encoder, data::make(vecCodecFcc), fcc)
{
	HIC hic;
	LRESULT lr;

	hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

BOOST_DATA_TEST_CASE(vcm_ICOpen_decoder, data::make(vecCodecFcc), fcc)
{
	HIC hic;
	LRESULT lr;

	hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_DECOMPRESS);
	BOOST_REQUIRE(hic != NULL);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
