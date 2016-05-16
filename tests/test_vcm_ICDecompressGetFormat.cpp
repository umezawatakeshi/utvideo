/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "expand.h"

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
	bihEncoderIn.biHeight = height;
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
	BOOST_TEST_CHECK(bihDecoderOut.biHeight == height);
	BOOST_TEST_CHECK(bihDecoderOut.biPlanes == 1);
	BOOST_TEST_CHECK(bihDecoderOut.biBitCount == FCC2BitCount(fccPrimary));
	BOOST_TEST_CHECK(bihDecoderOut.biCompression == FCC2Compression(fccPrimary));

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
	bihEncoderIn.biHeight = TEST_WIDTH;
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
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biCompression_ok, data::make(vecCodecFcc) ^ data::make(vecTopPriorityRawFcc), fccCodec, fccRaw)
{
	vcm_ICDecompressGetFormat_ok(fccCodec, fccRaw, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biWidth_ok, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityRawFcc) ^ data::make(vecSupportedWidth)), fccCodec, fccRaw, width)
{
	vcm_ICDecompressGetFormat_ok(fccCodec, fccRaw, width, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biHeight_ok, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityRawFcc) ^ data::make(vecSupportedHeight)), fccCodec, fccRaw, height)
{
	vcm_ICDecompressGetFormat_ok(fccCodec, fccRaw, TEST_WIDTH, height);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biWidth_ng, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityRawFcc) ^ data::make(vecUnsupportedWidth)), fccCodec, fccRaw, width)
{
	vcm_ICDecompressGetFormat_ng(fccCodec, fccRaw, width, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressGetFormat_biHeight_ng, expand(data::make(vecCodecFcc) ^ data::make(vecTopPriorityRawFcc) ^ data::make(vecUnsupportedHeight)), fccCodec, fccRaw, height)
{
	vcm_ICDecompressGetFormat_ng(fccCodec, fccRaw, TEST_WIDTH, height);
}
