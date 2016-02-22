/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

#define TEST_WIDTH 1920
#define TEST_HEIGHT 1080

BOOST_TEST_DECORATOR(*depends_on("vcm_open_enc"))
BOOST_DATA_TEST_CASE(vcm_enc_fmt_ok, data::make(vecSupportedInputFormatPair), crf)
{
	DWORD fccCodec = crf.fccCodec;
	DWORD fccRaw = crf.fccRaw;

	HIC hic;
	LRESULT lr;
	DWORD dw;
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
	bihIn.biWidth = TEST_WIDTH;
	bihIn.biHeight = TEST_HEIGHT * FCC2HeightSign(fccRaw);
	bihIn.biPlanes = 1;
	bihIn.biBitCount = FCC2BitCount(fccRaw);
	bihIn.biCompression = FCC2Compression(fccRaw);
	bihIn.biSizeImage = 10000000;

	gfret = ICCompressGetFormatSize(hic, &bihIn);
	BOOST_TEST_REQUIRE(gfret >= sizeof(BITMAPINFOHEADER));
	BOOST_TEST_REQUIRE(gfret <= sizeof(bihOutBuf));

	dw = ICCompressGetFormat(hic, &bihIn, &bihOut);
	BOOST_REQUIRE(dw == ICERR_OK);

	BOOST_TEST_CHECK(bihOut.biSize == gfret);
	BOOST_TEST_CHECK(bihOut.biWidth == TEST_WIDTH);
	BOOST_TEST_CHECK(bihOut.biHeight == TEST_HEIGHT);
	BOOST_TEST_CHECK(bihOut.biPlanes == 1);
//	BOOST_TEST_CHECK(bihOut.biBitCount == xxx); // XXX
	BOOST_TEST_CHECK(bihOut.biCompression == fccCodec);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_open_enc"))
BOOST_DATA_TEST_CASE(vcm_enc_fmt_ng, data::make(vecUnsupportedInputFormatPair), crf)
{
	DWORD fccCodec = crf.fccCodec;
	DWORD fccRaw = crf.fccRaw;

	HIC hic;
	LRESULT lr;
	DWORD dw;
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
	bihIn.biWidth = TEST_WIDTH;
	bihIn.biHeight = TEST_HEIGHT * FCC2HeightSign(fccRaw);
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

	dw = ICCompressGetFormat(hic, &bihIn, &bihOut);
	BOOST_CHECK(dw != ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
