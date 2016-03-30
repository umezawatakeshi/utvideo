/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_CoCreateInstance(REFGUID clsidCodec)
{
	HRESULT hr;
	IUnknown *pUnk;

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pUnk);
	BOOST_REQUIRE_EQUAL(hr, S_OK);
	BOOST_REQUIRE(pUnk != NULL);

	pUnk->Release();
}

BOOST_DATA_TEST_CASE(dmo_CoCreateInstance_encoder, data::make(vecCodecFourcc), fcc)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_CoCreateInstance(clsid);
}

BOOST_DATA_TEST_CASE(dmo_CoCreateInstance_decoder, data::make(vecCodecFourcc), fcc)
{
	DMODecoderCLSID clsid(fcc);
	dmo_CoCreateInstance(clsid);
}
