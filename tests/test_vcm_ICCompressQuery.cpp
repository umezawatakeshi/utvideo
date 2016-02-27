/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

#define TEST_WIDTH 1920
#define TEST_HEIGHT 1080

void vcm_ICCompressQuery_anyout_ok(DWORD fccCodec, DWORD fccRaw, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;
	DWORD dw;

	BITMAPINFOHEADER bihIn;

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

	dw = ICCompressQuery(hic, &bihIn, NULL);
	BOOST_CHECK(dw == ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

void vcm_ICCompressQuery_anyout_ng(DWORD fccCodec, DWORD fccRaw, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;
	DWORD dw;

	BITMAPINFOHEADER bihIn;

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

	dw = ICCompressQuery(hic, &bihIn, NULL);
	BOOST_CHECK(dw != ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biCompression_ok, data::make(vecSupportedInputFormatPair), crf)
{
	vcm_ICCompressQuery_anyout_ok(crf.fccCodec, crf.fccRaw, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biCompression_ng, data::make(vecUnsupportedInputFormatPair), crf)
{
	vcm_ICCompressQuery_anyout_ng(crf.fccCodec, crf.fccRaw, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biWidth_ok, data::make(vecSupportedWidthPair), swp)
{
	vcm_ICCompressQuery_anyout_ok(swp.first.first, swp.first.second, swp.second, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biWidth_ng, data::make(vecUnsupportedWidthPair), swp)
{
	vcm_ICCompressQuery_anyout_ng(swp.first.first, swp.first.second, swp.second, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biHeight_ok, data::make(vecSupportedHeightPair), swp)
{
	vcm_ICCompressQuery_anyout_ok(swp.first.first, swp.first.second, TEST_WIDTH, swp.second);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biHeight_ng, data::make(vecUnsupportedHeightPair), swp)
{
	vcm_ICCompressQuery_anyout_ng(swp.first.first, swp.first.second, TEST_WIDTH, swp.second);
}
