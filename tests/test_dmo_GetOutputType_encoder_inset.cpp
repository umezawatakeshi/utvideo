/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"
#include "expand.h"

void dmo_GetOutputType_encoder_inset_(REFGUID clsidCodec,
	REFGUID guidIn, BOOL bFixedSizeSamplesIn, BOOL bTemporalCompressionIn,
	REFGUID guidOut, BOOL bFixedSizeSamplesOut, BOOL bTemporalCompressionOut,
	LONG width, LONG height)
{
	HRESULT hr;
	IMediaObject *pObj;
	vector<GUID> inTypes;
	DMO_MEDIA_TYPE mt;
	VIDEOINFOHEADER *pvih;

	DWORD fccIn = DirectShowFormatToVCMFormat(guidIn);
	DWORD fccOut = DirectShowFormatToVCMFormat(guidOut);

	hr = CoCreateInstance(clsidCodec, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (LPVOID*)&pObj);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pObj != NULL);

	memset(&mt, 0, sizeof(mt));
	MoInitMediaType(&mt, sizeof(VIDEOINFOHEADER));
	pvih = (VIDEOINFOHEADER*)mt.pbFormat;
	memset(pvih, 0, sizeof(VIDEOINFOHEADER));

	mt.majortype = MEDIATYPE_Video;
	mt.subtype = guidIn;
	mt.bFixedSizeSamples = bFixedSizeSamplesIn;
	mt.bTemporalCompression = bTemporalCompressionIn;
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
	BOOST_REQUIRE(hr == S_OK);

	MoFreeMediaType(&mt);

	hr = pObj->GetOutputType(0, 1, &mt);
	BOOST_CHECK(hr == DMO_E_NO_MORE_ITEMS);

	hr = pObj->GetOutputType(0, 0, &mt);
	BOOST_REQUIRE(hr == S_OK);

	BOOST_CHECK(mt.majortype == MEDIATYPE_Video);
	BOOST_CHECK(mt.subtype == guidOut);
	BOOST_CHECK(mt.bFixedSizeSamples == bFixedSizeSamplesOut);
	BOOST_CHECK(mt.bTemporalCompression == bTemporalCompressionOut);
	BOOST_CHECK(IsEqualGUID(mt.formattype, FORMAT_VideoInfo));
	BOOST_CHECK(mt.pbFormat != NULL);
	pvih = (VIDEOINFOHEADER*)mt.pbFormat;
	BOOST_CHECK(pvih->bmiHeader.biSize >= sizeof(BITMAPINFOHEADER));
	BOOST_CHECK(pvih->bmiHeader.biWidth == width);
	BOOST_CHECK(pvih->bmiHeader.biHeight == height);
	BOOST_CHECK(pvih->bmiHeader.biPlanes == 1);
	BOOST_CHECK(pvih->bmiHeader.biBitCount == FCC2BitCount(fccOut));
	BOOST_CHECK(pvih->bmiHeader.biCompression == FCC2Compression(fccOut));

	pObj->Release();
}

BOOST_TEST_DECORATOR(*depends_on("dmo_CoCreateInstance_encoder")*depends_on("dmo_QueryInterface_encoder")*depends_on("dmo_SetInputType_encoder_subtype_ok"))
BOOST_DATA_TEST_CASE(dmo_GetOutputType_encoder_inset, data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecTemporalCompressionSupported), fcc, guid, temporal)
{
	DMOEncoderCLSID clsid(fcc);
	FourCCGUID outSubtype(fcc);
	dmo_GetOutputType_encoder_inset_(clsid, guid, TRUE, FALSE, outSubtype, FALSE, temporal, TEST_WIDTH, TEST_HEIGHT);
}
