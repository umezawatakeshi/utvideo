/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"

void dmo_DMOGetTypes(REFGUID clsidCodec, const vector<GUID> &guidIn, const vector<GUID> &guidOut)
{
	HRESULT hr;
	DMO_PARTIAL_MEDIATYPE inTypes[128];
	DMO_PARTIAL_MEDIATYPE outTypes[128];
	unsigned long nInTypes;
	unsigned long nOutTypes;
	vector<GUID> inSubTypes;
	vector<GUID> outSubTypes;

	hr = DMOGetTypes(clsidCodec, _countof(inTypes), &nInTypes, inTypes, _countof(outTypes), &nOutTypes, outTypes);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_CHECK_EQUAL(nInTypes, guidIn.size());
	BOOST_CHECK_EQUAL(nOutTypes, guidOut.size());

	for (unsigned int i = 0; i < nInTypes; ++i)
	{
		BOOST_CHECK_EQUAL(inTypes[i].type, MEDIATYPE_Video);
		inSubTypes.push_back(inTypes[i].subtype);
	}

	for (unsigned int i = 0; i < nOutTypes; ++i)
	{
		BOOST_CHECK_EQUAL(outTypes[i].type, MEDIATYPE_Video);
		outSubTypes.push_back(outTypes[i].subtype);
	}

	BOOST_CHECK_EQUAL(inSubTypes, guidIn);
	BOOST_CHECK_EQUAL(outSubTypes, guidOut);
}

BOOST_DATA_TEST_CASE(dmo_DMOGetTypes_encoder, data::make(vecCodecFourcc) ^ data::make(vecSupportedEncoderInputTypes), fcc, guids)
{
	DMOEncoderCLSID clsid(fcc);
	vector<GUID> codecType;

	codecType.push_back(FourCCGUID(fcc));
	dmo_DMOGetTypes(clsid, guids, codecType);
}

BOOST_DATA_TEST_CASE(dmo_DMOGetTypes_decoder, data::make(vecCodecFourcc) ^ data::make(vecSupportedDecoderOutputTypes), fcc, guids)
{
	DMODecoderCLSID clsid(fcc);
	vector<GUID> codecType;

	codecType.push_back(FourCCGUID(fcc));
	dmo_DMOGetTypes(clsid, codecType, guids);
}
