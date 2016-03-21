/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

#define TEST_WIDTH 1920
#define TEST_HEIGHT 1080

void vcm_ICCompressGetFormat_ok(DWORD fccCodec, DWORD fccRaw, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;
	DWORD gfret;

	BITMAPINFOHEADER bihIn;
	union
	{
		BITMAPINFOHEADER bihOut;
		char bihOutBuf[128];
	};

	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);

	memset(&bihIn, 0, sizeof(BITMAPINFOHEADER));
	bihIn.biSize = sizeof(BITMAPINFOHEADER);
	bihIn.biWidth = width;
	bihIn.biHeight = height * FCC2HeightSign(fccRaw);
	bihIn.biPlanes = 1;
	bihIn.biBitCount = FCC2BitCount(fccRaw);
	bihIn.biCompression = FCC2Compression(fccRaw);
	bihIn.biSizeImage = 10000000;

	gfret = ICCompressGetFormatSize(hic, &bihIn);
	BOOST_TEST_REQUIRE(gfret >= sizeof(BITMAPINFOHEADER));
	BOOST_TEST_REQUIRE(gfret <= sizeof(bihOutBuf));

	lr = ICCompressGetFormat(hic, &bihIn, &bihOut);
	BOOST_REQUIRE(lr == ICERR_OK);

	BOOST_TEST_CHECK(bihOut.biSize == gfret);
	BOOST_TEST_CHECK(bihOut.biWidth == width);
	BOOST_TEST_CHECK(bihOut.biHeight == height);
	BOOST_TEST_CHECK(bihOut.biPlanes == 1);
//	BOOST_TEST_CHECK(bihOut.biBitCount == xxx); // XXX
	BOOST_TEST_CHECK(bihOut.biCompression == fccCodec);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

void vcm_ICCompressGetFormat_ng(DWORD fccCodec, DWORD fccRaw, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;
	DWORD gfret;

	BITMAPINFOHEADER bihIn;
	union
	{
		BITMAPINFOHEADER bihOut;
		char bihOutBuf[128];
	};

	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);

	memset(&bihIn, 0, sizeof(BITMAPINFOHEADER));
	bihIn.biSize = sizeof(BITMAPINFOHEADER);
	bihIn.biWidth = width;
	bihIn.biHeight = height * FCC2HeightSign(fccRaw);
	bihIn.biPlanes = 1;
	bihIn.biBitCount = FCC2BitCount(fccRaw);
	bihIn.biCompression = FCC2Compression(fccRaw);
	bihIn.biSizeImage = 10000000;

	/*
	 * サポートしない入力フォーマットについて ICCompressGetFormatSize を呼んだ時に
	 * 何が返ってくるべきかは MSDN Library を読んでもイマイチ判然としないが、
	 * utv_vcm の仕様としてはフォーマットブロックのサイズを返すものとする。
	 */
	gfret = ICCompressGetFormatSize(hic, &bihIn);
	BOOST_TEST_REQUIRE(gfret >= sizeof(BITMAPINFOHEADER));
	BOOST_TEST_REQUIRE(gfret <= sizeof(bihOutBuf));

	lr = ICCompressGetFormat(hic, &bihIn, &bihOut);
	BOOST_CHECK(lr != ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressGetFormat_biCompression_ok, data::make(vecSupportedInputFormatPair), crf)
{
	vcm_ICCompressGetFormat_ok(crf.fccCodec, crf.fccRaw, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressGetFormat_biCompression_ng, data::make(vecUnsupportedInputFormatPair), crf)
{
	vcm_ICCompressGetFormat_ng(crf.fccCodec, crf.fccRaw, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressGetFormat_biWidth_ok, data::make(vecSupportedWidthPair), swp)
{
	vcm_ICCompressGetFormat_ok(swp.first.first, swp.first.second, swp.second, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressGetFormat_biWidth_ng, data::make(vecUnsupportedWidthPair), swp)
{
	vcm_ICCompressGetFormat_ng(swp.first.first, swp.first.second, swp.second, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressGetFormat_biHeight_ok, data::make(vecSupportedHeightPair), swp)
{
	vcm_ICCompressGetFormat_ok(swp.first.first, swp.first.second, TEST_WIDTH, swp.second);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressGetFormat_biHeight_ng, data::make(vecUnsupportedHeightPair), swp)
{
	vcm_ICCompressGetFormat_ng(swp.first.first, swp.first.second, TEST_WIDTH, swp.second);
}
