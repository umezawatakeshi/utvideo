/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_DMOGetName(REFGUID clsidCodec, wstring wstrName)
{
	HRESULT hr;
	WCHAR name[128];

	hr = DMOGetName(clsidCodec, name);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_CHECK_EQUAL(wstring(name), wstrName);
}

BOOST_DATA_TEST_CASE(dmo_DMOGetName_encoder, data::make(vecCodecFourcc) ^ data::make(vecEncoderName), fcc, wstrName)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_DMOGetName(clsid, wstrName);
}

BOOST_DATA_TEST_CASE(dmo_DMOGetName_decoder, data::make(vecCodecFourcc) ^ data::make(vecDecoderName), fcc, wstrName)
{
	DMODecoderCLSID clsid(fcc);
	dmo_DMOGetName(clsid, wstrName);
}
