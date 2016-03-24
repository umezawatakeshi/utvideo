/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "expand.h"

void vcm_ICCompressQuery_anyout_ok(DWORD fccCodec, DWORD fccRaw, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;

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

	lr = ICCompressQuery(hic, &bihIn, NULL);
	BOOST_CHECK(lr == ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

void vcm_ICCompressQuery_anyout_ng(DWORD fccCodec, DWORD fccRaw, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;

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

	lr = ICCompressQuery(hic, &bihIn, NULL);
	BOOST_CHECK(lr != ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biCompression_ok, expand(data::make(vecCodecFourcc) ^ data::make(vecSupportedInputFormat)), fccCodec, fccRaw)
{
	vcm_ICCompressQuery_anyout_ok(fccCodec, fccRaw, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biCompression_ng, expand(data::make(vecCodecFourcc) ^ data::make(vecUnsupportedInputFormat)), fccCodec, fccRaw)
{
	vcm_ICCompressQuery_anyout_ng(fccCodec, fccRaw, TEST_WIDTH, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biWidth_ok, expand(data::make(vecCodecFourcc) ^ data::make(vecTopPriorityRawFormat) ^ data::make(vecSupportedWidth)), fccCodec, fccRaw, width)
{
	vcm_ICCompressQuery_anyout_ok(fccCodec, fccRaw, width, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biWidth_ng, expand(data::make(vecCodecFourcc) ^ data::make(vecTopPriorityRawFormat) ^ data::make(vecUnsupportedWidth)), fccCodec, fccRaw, width)
{
	vcm_ICCompressQuery_anyout_ng(fccCodec, fccRaw, width, TEST_HEIGHT);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biHeight_ok, expand(data::make(vecCodecFourcc) ^ data::make(vecTopPriorityRawFormat) ^ data::make(vecSupportedHeight)), fccCodec, fccRaw, height)
{
	vcm_ICCompressQuery_anyout_ok(fccCodec, fccRaw, TEST_WIDTH, height);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_ICCompressQuery_anyout_biHeight_ng, expand(data::make(vecCodecFourcc) ^ data::make(vecTopPriorityRawFormat) ^ data::make(vecUnsupportedHeight)), fccCodec, fccRaw, height)
{
	vcm_ICCompressQuery_anyout_ng(fccCodec, fccRaw, TEST_WIDTH, height);
}
