/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_GetStreamInfo(REFGUID clsidCodec, DWORD dwInputFlags, DWORD dwOutputFlags)
{
	HRESULT hr;
	IMediaObject *pObj;
	DWORD dw;

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (LPVOID*)&pObj);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pObj != NULL);

	hr = pObj->GetInputStreamInfo(0, &dw);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_CHECK(dw == dwInputFlags);

	hr = pObj->GetOutputStreamInfo(0, &dw);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_CHECK(dw == dwOutputFlags);

	pObj->Release();
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_GetStreamInfo_encoder, data::make(vecCodecFourcc), fcc)
{
	DMOEncoderCLSID guid(fcc);
	dmo_GetStreamInfo(guid,
		DMO_INPUT_STREAMF_WHOLE_SAMPLES | DMO_INPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER | DMO_INPUT_STREAMF_FIXED_SAMPLE_SIZE,
		DMO_INPUT_STREAMF_WHOLE_SAMPLES | DMO_INPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_decoder")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_GetStreamInfo_decoder, data::make(vecCodecFourcc), fcc)
{
	DMODecoderCLSID guid(fcc);
	dmo_GetStreamInfo(guid,
		DMO_INPUT_STREAMF_WHOLE_SAMPLES | DMO_INPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER,
		DMO_INPUT_STREAMF_WHOLE_SAMPLES | DMO_INPUT_STREAMF_SINGLE_SAMPLE_PER_BUFFER | DMO_INPUT_STREAMF_FIXED_SAMPLE_SIZE);
}
