/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

#define TEST_WIDTH 1920
#define TEST_HEIGHT 1080

void vcm_ICDecompressGetFormat_ok(DWORD fccCodec, DWORD fccPrimary, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;
	DWORD gfret;

	BITMAPINFOHEADER bihEncoderIn;
	union
	{
		BITMAPINFOHEADER bihCompressed;
		char bihCompressedBuf[128];
	};
	BITMAPINFOHEADER bihDecoderOut;

	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);

	memset(&bihEncoderIn, 0, sizeof(BITMAPINFOHEADER));
	bihEncoderIn.biSize = sizeof(BITMAPINFOHEADER);
	bihEncoderIn.biWidth = width;
	bihEncoderIn.biHeight = height * FCC2HeightSign(fccPrimary);
	bihEncoderIn.biPlanes = 1;
	bihEncoderIn.biBitCount = FCC2BitCount(fccPrimary);
	bihEncoderIn.biCompression = FCC2Compression(fccPrimary);
	bihEncoderIn.biSizeImage = 10000000;

	lr = ICCompressGetFormat(hic, &bihEncoderIn, &bihCompressed);
	BOOST_REQUIRE(lr == ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);


	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_DECOMPRESS);
	BOOST_REQUIRE(hic != NULL);

	gfret = ICDecompressGetFormatSize(hic, &bihCompressed);
	BOOST_REQUIRE(gfret == sizeof(BITMAPINFOHEADER));

	lr = ICDecompressGetFormat(hic, &bihCompressed, &bihDecoderOut);
	BOOST_REQUIRE(lr == ICERR_OK);
	BOOST_TEST_CHECK(bihDecoderOut.biSize == sizeof(BITMAPINFOHEADER));
	BOOST_TEST_CHECK(bihDecoderOut.biWidth == width);
	BOOST_TEST_CHECK(bihDecoderOut.biHeight == height * FCC2HeightSign(fccPrimary));
	BOOST_TEST_CHECK(bihDecoderOut.biPlanes == 1);
	BOOST_TEST_CHECK(bihDecoderOut.biBitCount == FCC2BitCount(fccPrimary));
	BOOST_TEST_CHECK(bihDecoderOut.biCompression == FCC2Compression(fccPrimary););

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

void vcm_ICDecompressGetFormat_ng(DWORD fccCodec, DWORD fccPrimary, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;
	DWORD gfret;

	BITMAPINFOHEADER bihEncoderIn;
	union
	{
		BITMAPINFOHEADER bihCompressed;
		char bihCompressedBuf[128];
	};
	BITMAPINFOHEADER bihDecoderOut;

	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);

	memset(&bihEncoderIn, 0, sizeof(BITMAPINFOHEADER));
	bihEncoderIn.biSize = sizeof(BITMAPINFOHEADER);
	bihEncoderIn.biWidth = TEST_HEIGHT;
	bihEncoderIn.biHeight = TEST_WIDTH * FCC2HeightSign(fccPrimary);
	bihEncoderIn.biPlanes = 1;
	bihEncoderIn.biBitCount = FCC2BitCount(fccPrimary);
	bihEncoderIn.biCompression = FCC2Compression(fccPrimary);
	bihEncoderIn.biSizeImage = 10000000;

	lr = ICCompressGetFormat(hic, &bihEncoderIn, &bihCompressed);
	BOOST_REQUIRE(lr == ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);

	bihCompressed.biWidth = width;
	bihCompressed.biHeight = height;

	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_DECOMPRESS);
	BOOST_REQUIRE(hic != NULL);

	gfret = ICDecompressGetFormatSize(hic, &bihCompressed);
	BOOST_REQUIRE(gfret == sizeof(BITMAPINFOHEADER));

	/*
	 * サポートしない入力フォーマットについて ICDecompressGetFormatSize を呼んだ時に
	 * 何が返ってくるべきかは MSDN Library を読んでもイマイチ判然としないが、
	 * utv_vcm の仕様としてはフォーマットブロックのサイズを返すものとする。
	 */
	gfret = ICDecompressGetFormatSize(hic, &bihCompressed);
	BOOST_REQUIRE(gfret == sizeof(BITMAPINFOHEADER));

	lr = ICDecompressGetFormat(hic, &bihCompressed, &bihDecoderOut);
	BOOST_CHECK(lr != ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biCompression_ok, data::make(vecTopPriorityRawFormat), crf)
{
	vcm_ICDecompressGetFormat_ok(crf.first, crf.second, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biWidth_ok, data::make(vecSupportedWidthPair), crf)
{
	vcm_ICDecompressGetFormat_ok(crf.first.first, crf.first.second, crf.second, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biHeight_ok, data::make(vecSupportedHeightPair), crf)
{
	vcm_ICDecompressGetFormat_ok(crf.first.first, crf.first.second, TEST_WIDTH, crf.second);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biWidth_ng, data::make(vecUnsupportedWidthPair), crf)
{
	vcm_ICDecompressGetFormat_ng(crf.first.first, crf.first.second, crf.second, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biHeight_ng, data::make(vecUnsupportedHeightPair), crf)
{
	vcm_ICDecompressGetFormat_ng(crf.first.first, crf.first.second, TEST_WIDTH, crf.second);
}
