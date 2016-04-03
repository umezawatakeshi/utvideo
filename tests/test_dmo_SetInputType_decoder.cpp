/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_dmo_fmt.h"
#include "expand.h"

void dmo_SetInputType_decoder_(REFGUID clsidEnc, REFGUID clsidDec, REFGUID guidRaw, LONG width, LONG height, bool bExpectSuccess)
{
	HRESULT hr;
	IMediaObject *pObj;
	vector<GUID> inTypes;
	DMO_MEDIA_TYPE mt;
	VIDEOINFOHEADER *pvih;

	DWORD fccIn = DirectShowFormatToVCMFormat(guidRaw);

	hr = CoCreateInstance(clsidEnc, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (LPVOID*)&pObj);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pObj != NULL);

	memset(&mt, 0, sizeof(mt));
	MoInitMediaType(&mt, sizeof(VIDEOINFOHEADER));
	pvih = (VIDEOINFOHEADER*)mt.pbFormat;
	memset(pvih, 0, sizeof(VIDEOINFOHEADER));

	mt.majortype = MEDIATYPE_Video;
	mt.subtype = guidRaw;
	mt.bFixedSizeSamples = TRUE;
	mt.bTemporalCompression = FALSE;
	mt.lSampleSize = 10000000; /* XXX */
	mt.formattype = FORMAT_VideoInfo;
	pvih->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvih->bmiHeader.biWidth = TEST_WIDTH;
	pvih->bmiHeader.biHeight = TEST_HEIGHT;
	pvih->bmiHeader.biPlanes = 1;
	pvih->bmiHeader.biBitCount = FCC2BitCount(fccIn);
	pvih->bmiHeader.biCompression = FCC2Compression(fccIn);
	pvih->bmiHeader.biSizeImage = 10000000; /* XXX */

	hr = pObj->SetInputType(0, &mt, 0);
	BOOST_REQUIRE(hr == S_OK);

	MoFreeMediaType(&mt);

	hr = pObj->GetOutputType(0, 0, &mt);
	BOOST_REQUIRE(hr == S_OK);

	pObj->Release();

	pvih = (VIDEOINFOHEADER*)mt.pbFormat;
	pvih->bmiHeader.biWidth = width;
	pvih->bmiHeader.biHeight = height;

	hr = CoCreateInstance(clsidDec, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (LPVOID*)&pObj);
	BOOST_REQUIRE(hr == S_OK);
	BOOST_REQUIRE(pObj != NULL);

	hr = pObj->SetInputType(0, &mt, 0);
	if (bExpectSuccess)
		BOOST_CHECK(hr == S_OK);
	else
		BOOST_CHECK(hr != S_OK);

	pObj->Release();
}

BOOST_TEST_DECORATOR(*depends_on("dmo_SetInputType_encoder_subtype_ok")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_decoder_ok, data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype), fcc, guid)
{
	DMOEncoderCLSID clsidEnc(fcc);
	DMODecoderCLSID clsidDec(fcc);
	FourCCGUID outSubtype(fcc);
	dmo_SetInputType_decoder_(clsidEnc, clsidDec, guid, TEST_WIDTH, TEST_HEIGHT, true);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_SetInputType_encoder_subtype_ok")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_decoder_width_ok, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecSupportedWidth)), fcc, guid, width)
{
	DMOEncoderCLSID clsidEnc(fcc);
	DMODecoderCLSID clsidDec(fcc);
	FourCCGUID outSubtype(fcc);
	dmo_SetInputType_decoder_(clsidEnc, clsidDec, guid, width, TEST_HEIGHT, true);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_SetInputType_encoder_subtype_ok")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_decoder_height_ok, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecSupportedHeight)), fcc, guid, height)
{
	DMOEncoderCLSID clsidEnc(fcc);
	DMODecoderCLSID clsidDec(fcc);
	FourCCGUID outSubtype(fcc);
	dmo_SetInputType_decoder_(clsidEnc, clsidDec, guid, TEST_WIDTH, height, true);
}


BOOST_TEST_DECORATOR(*depends_on("dmo_SetInputType_encoder_subtype_ok")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_decoder_width_ng, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecUnsupportedWidth)), fcc, guid, width)
{
	DMOEncoderCLSID clsidEnc(fcc);
	DMODecoderCLSID clsidDec(fcc);
	FourCCGUID outSubtype(fcc);
	dmo_SetInputType_decoder_(clsidEnc, clsidDec, guid, width, TEST_HEIGHT, false);
}

BOOST_TEST_DECORATOR(*depends_on("dmo_SetInputType_encoder_subtype_ok")*depends_on("dmo_QueryInterface_decoder"))
BOOST_DATA_TEST_CASE(dmo_SetInputType_decoder_height_ng, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityEncoderInputSubtype) ^ data::make(vecUnsupportedHeight)), fcc, guid, height)
{
	DMOEncoderCLSID clsidEnc(fcc);
	DMODecoderCLSID clsidDec(fcc);
	FourCCGUID outSubtype(fcc);
	dmo_SetInputType_decoder_(clsidEnc, clsidDec, guid, TEST_WIDTH, height, false);
}

// XXX: subtype を変えてエラーになるかどうかのテストもあった方がいいと思うのだが、その場合の適切な値とは？
