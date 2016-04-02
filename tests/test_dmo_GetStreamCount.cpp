/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_GetStreamCount(REFGUID clsidCodec)
{
	HRESULT hr;
	IMediaObject *pObj;
	DWORD nInputStreams;
	DWORD nOutputStreams;

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (LPVOID*)&pObj);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pObj != NULL);

	hr = pObj->GetStreamCount(&nInputStreams, &nOutputStreams);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_CHECK(nInputStreams == 1);
	BOOST_CHECK(nOutputStreams == 1);

	pObj->Release();
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_GetStreamCount_encoder, data::make(vecCodecFcc), fcc)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_GetStreamCount(clsid);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_decoder")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_GetStreamCount_decoder, data::make(vecCodecFcc), fcc)
{
	DMODecoderCLSID clsid(fcc);
	dmo_GetStreamCount(clsid);
}
