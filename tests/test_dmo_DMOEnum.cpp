/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_DMOEnum(REFGUID category, REFGUID clsidCodec, wstring wstrName)
{
	IEnumDMO *pEnum;
	HRESULT hr;
	bool found = false;
	CLSID clsid;
	DWORD dw;
	LPWSTR name;

	hr = DMOEnum(category, 0, 0, NULL, 0, NULL, &pEnum);
	BOOST_REQUIRE(hr == S_OK);

	hr = pEnum->Reset();
	BOOST_REQUIRE(hr == S_OK);

	while ((hr = pEnum->Next(1, &clsid, &name, &dw)) == S_OK)
	{
		if (IsEqualGUID(clsid, clsidCodec))
		{
			found = true;
			BOOST_CHECK_EQUAL(wstring(name), wstrName);
		}
		CoTaskMemFree(name);
	}

	BOOST_CHECK(hr == S_FALSE);

	BOOST_CHECK(found);

	pEnum->Release();
}

BOOST_DATA_TEST_CASE(dmo_DMOEnum_encoder, data::make(vecCodecFourcc) ^ data::make(vecEncoderName), fcc, wstrName)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_DMOEnum(DMOCATEGORY_VIDEO_ENCODER, clsid, wstrName);
}

BOOST_DATA_TEST_CASE(dmo_DMOEnum_decoder, data::make(vecCodecFourcc) ^ data::make(vecDecoderName), fcc, wstrName)
{
	DMODecoderCLSID clsid(fcc);
	dmo_DMOEnum(DMOCATEGORY_VIDEO_DECODER, clsid, wstrName);
}
