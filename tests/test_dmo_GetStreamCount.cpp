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

BOOST_DATA_TEST_CASE(dmo_GetStreamCount_encoder, data::make(vecCodecFourcc), fcc)
{
	DMOEncoderCLSID guid(fcc);
	dmo_GetStreamCount(guid);
}

BOOST_DATA_TEST_CASE(dmo_GetStreamCount_decoder, data::make(vecCodecFourcc), fcc)
{
	DMODecoderCLSID guid(fcc);
	dmo_GetStreamCount(guid);
}
