/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_QueryInterface(REFGUID clsidCodec, REFIID iid)
{
	HRESULT hr;
	IUnknown *pUnk;
	IUnknown *pUnk2;

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pUnk);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pUnk != NULL);
	hr = pUnk->QueryInterface(iid, (LPVOID*)&pUnk2);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pUnk2 != NULL);
	pUnk2->Release();
	pUnk->Release();

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, iid, (LPVOID*)&pUnk);
	BOOST_REQUIRE_EQUAL(hr, S_OK);
	BOOST_REQUIRE(pUnk != NULL);
	pUnk->Release();

}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder"))
BOOST_DATA_TEST_CASE(dmo_QueryInterface_encoder, data::make(vecCodecFourcc) * data::make(vecSupportedEncoderInterfaces), fcc, iid)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_QueryInterface(clsid, iid);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_decoder"))
BOOST_DATA_TEST_CASE(dmo_QueryInterface_decoder, data::make(vecCodecFourcc) * data::make(vecSupportedDecoderInterfaces), fcc, iid)
{
	DMODecoderCLSID clsid(fcc);
	dmo_QueryInterface(clsid, iid);
}
