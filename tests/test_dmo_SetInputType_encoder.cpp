/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"
#include "expand.h"

void dmo_SetInputType_encoder(REFGUID clsidCodec, REFGUID guidIn, BOOL bFixedSizeSamples, BOOL bTemporalCompression, LONG width, LONG height, bool bExpectSuccess)
{
	HRESULT hr;
	IMediaObject *pObj;
	vector<GUID> inTypes;
	DMO_MEDIA_TYPE mt;
	VIDEOINFOHEADER *pvih;

	DWORD fccIn = DirectShowFormatToVCMFormat(guidIn);

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (LPVOID*)&pObj);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pObj != NULL);

	memset(&mt, 0, sizeof(mt));
	MoInitMediaType(&mt, sizeof(VIDEOINFOHEADER));
	pvih = (VIDEOINFOHEADER*)mt.pbFormat;
	memset(pvih, 0, sizeof(VIDEOINFOHEADER));

	mt.majortype = MEDIATYPE_Video;
	mt.subtype = guidIn;
	mt.bFixedSizeSamples = bFixedSizeSamples;
	mt.bTemporalCompression = bTemporalCompression;
	mt.lSampleSize = 10000000; /* XXX */
	mt.formattype = FORMAT_VideoInfo;
	pvih->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvih->bmiHeader.biWidth = width;
	pvih->bmiHeader.biHeight = height;
	pvih->bmiHeader.biPlanes = 1;
	pvih->bmiHeader.biBitCount = FCC2BitCount(fccIn);
	pvih->bmiHeader.biCompression = FCC2Compression(fccIn);
	pvih->bmiHeader.biSizeImage = 10000000; /* XXX */

	hr = pObj->SetInputType(0, &mt, 0);
	if (bExpectSuccess)
		BOOST_CHECK(hr == S_OK);
	else
		BOOST_CHECK(hr != S_OK);

	MoFreeMediaType(&mt);

	pObj->Release();
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_encoder_subtype_ok, expand(data::make(vecCodecFcc) ^ data::make(vecSupportedEncoderInputSubtypes)), fcc, guid)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_SetInputType_encoder(clsid, guid, TRUE, FALSE, TEST_WIDTH, TEST_HEIGHT, true);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_encoder_width_ok, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecSupportedWidth)), fcc, guid, width)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_SetInputType_encoder(clsid, guid, TRUE, FALSE, width, TEST_HEIGHT, true);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_encoder_height_ok, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecSupportedHeight)), fcc, guid, height)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_SetInputType_encoder(clsid, guid, TRUE, FALSE, TEST_WIDTH, height, true);
}


BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_encoder_subtype_ng, expand(data::make(vecCodecFcc) ^ data::make(vecUnsupportedEncoderInputSubtypes)), fcc, guid)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_SetInputType_encoder(clsid, guid, TRUE, FALSE, TEST_WIDTH, TEST_HEIGHT, false);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_encoder_width_ng, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecUnsupportedWidth)), fcc, guid, width)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_SetInputType_encoder(clsid, guid, TRUE, FALSE, width, TEST_HEIGHT, false);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_encoder_height_ng, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecUnsupportedHeight)), fcc, guid, height)
{
	DMOEncoderCLSID clsid(fcc);
	dmo_SetInputType_encoder(clsid, guid, TRUE, FALSE, TEST_WIDTH, height, false);
}
