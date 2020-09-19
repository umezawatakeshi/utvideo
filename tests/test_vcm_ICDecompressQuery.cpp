/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "expand.h"

void vcm_ICDecompressQuery_ng(DWORD fccCodec, DWORD fccPrimary, DWORD fccRaw, LONG width, LONG height)
{
	HIC hic;
	LRESULT lr;

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

	memset(&bihDecoderOut, 0, sizeof(BITMAPINFOHEADER));
	bihDecoderOut.biSize = sizeof(BITMAPINFOHEADER);
	bihDecoderOut.biWidth = width;
	bihDecoderOut.biHeight = height;
	bihDecoderOut.biPlanes = 1;
	bihDecoderOut.biBitCount = FCC2BitCount(fccRaw);
	bihDecoderOut.biCompression = FCC2Compression(fccRaw);
	bihDecoderOut.biSizeImage = 10000000;

	lr = ICDecompressQuery(hic, &bihCompressed, &bihDecoderOut);
	BOOST_CHECK(lr != ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder")* depends_on("vcm_ICOpen_decoder")* depends_on("vcm_ICCompressGetFormat_biCompression_ok"))
BOOST_DATA_TEST_CASE(vcm_ICDecompressQuery_combination_ng, data::make(vecUnsupportedCombination), fccCodec, fccPrimary, fccRaw, width, height)
{
	vcm_ICDecompressQuery_ng(fccCodec, fccPrimary, fccRaw, width, height);
}
