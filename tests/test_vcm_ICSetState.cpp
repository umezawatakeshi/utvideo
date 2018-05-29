/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICSetState, data::make(vecCodecFcc), fcc)
{
	HIC hic;
	LRESULT lr;
	vector<char> zeros(1024 * 1024, 0);

	hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);

	lr = ICSetState(hic, NULL, 0);
	BOOST_CHECK(lr == 0);

	lr = ICSetState(hic, reinterpret_cast<const void*>(std::numeric_limits<intptr_t>::min()), 0);
	BOOST_CHECK(lr == 0);

	lr = ICSetState(hic, &zeros.front(), zeros.size());
	BOOST_CHECK(lr == zeros.size());

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
