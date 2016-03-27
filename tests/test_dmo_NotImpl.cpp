/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_NotImpl(REFGUID clsidCodec)
{
	HRESULT hr;
	IMediaObject *pObj;
	REFERENCE_TIME rt;

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (LPVOID*)&pObj);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pObj != NULL);

	hr = pObj->GetInputMaxLatency(0, &rt);
	BOOST_CHECK(hr == E_NOTIMPL);

	hr = pObj->SetInputMaxLatency(0, rt);
	BOOST_CHECK(hr == E_NOTIMPL);

	pObj->Release();
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_NotImpl_encoder, data::make(vecCodecFourcc), fcc)
{
	DMOEncoderCLSID guid(fcc);
	dmo_NotImpl(guid);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_decoder")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_NotImpl_decoder, data::make(vecCodecFourcc), fcc)
{
	DMODecoderCLSID guid(fcc);
	dmo_NotImpl(guid);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_NotImpl_encoder_IAMVfwCompressDialogs, data::make(vecCodecFourcc), fcc)
{
	DMOEncoderCLSID guid(fcc);

	HRESULT hr;
	IAMVfwCompressDialogs *pDlg;

	hr = CoCreateInstance(guid, NULL, CLSCTX_INPROC_SERVER, IID_IAMVfwCompressDialogs, (LPVOID*)&pDlg);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pDlg != NULL);

	hr = pDlg->SendDriverMessage(0, 0, 0);
	BOOST_CHECK(hr == E_NOTIMPL);

	pDlg->Release();
}
