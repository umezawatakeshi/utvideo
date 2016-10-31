/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_clip.h"
#include "expand.h"
#include "tuple_container.h"
#include "VideoClip.h"
#include "ICCloser.h"
#include "Compare.h"

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_decoder")*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_encdec, make_data_from_tuple_container(vecEncDecClips), src, dst, fmt, config, tolerance)
{
	VideoClip srcClip(src);
	VideoClip dstClip(dst);
	DWORD fccCodec = FCC(fmt);

	BOOST_REQUIRE(srcClip.GetWidth() == dstClip.GetWidth());
	BOOST_REQUIRE(srcClip.GetHeight() == dstClip.GetHeight());

	DWORD fccSrc = srcClip.GetFourCC();
	DWORD fccDst = dstClip.GetFourCC();
	unsigned int nWidth = srcClip.GetWidth();
	unsigned int nHeight = srcClip.GetHeight();

	size_t cbSrcData;
	size_t cbDstData;
	size_t cbCompressedData;

	HIC hicEncode, hicDecode;
	LRESULT lr;
	union
	{
		BITMAPINFOHEADER bihCompressed;
		char bihCompressedBuf[128];
	};
	BITMAPINFOHEADER bihSrc;
	BITMAPINFOHEADER bihDst;

	hicEncode = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_COMPRESS);
	BOOST_REQUIRE(hicEncode != NULL);
	ICCloser iccloserEnc(hicEncode);

	lr = ICSetState(hicEncode, &config.front(), config.size());
	BOOST_REQUIRE(lr == config.size());

	hicDecode = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_DECOMPRESS);
	BOOST_REQUIRE(hicDecode != NULL);
	ICCloser iccloserDec(hicDecode);

	memset(&bihSrc, 0, sizeof(BITMAPINFOHEADER));
	bihSrc.biSize = sizeof(BITMAPINFOHEADER);
	bihSrc.biWidth = nWidth;
	bihSrc.biHeight = nHeight;
	bihSrc.biPlanes = 1;
	bihSrc.biBitCount = FCC2BitCount(fccSrc);
	bihSrc.biCompression = FCC2Compression(fccSrc);
	bihSrc.biSizeImage = 10000000;

	memset(&bihDst, 0, sizeof(BITMAPINFOHEADER));
	bihDst.biSize = sizeof(BITMAPINFOHEADER);
	bihDst.biWidth = nWidth;
	bihDst.biHeight = nHeight;
	bihDst.biPlanes = 1;
	bihDst.biBitCount = FCC2BitCount(fccDst);
	bihDst.biCompression = FCC2Compression(fccDst);
	bihDst.biSizeImage = 10000000;

	lr = ICCompressGetFormat(hicEncode, &bihSrc, &bihCompressed);
	BOOST_REQUIRE_EQUAL(lr, ICERR_OK);
	cbCompressedData = ICCompressGetSize(hicEncode, &bihSrc, &bihCompressed);

	lr = ICCompressBegin(hicEncode, &bihSrc, &bihCompressed);
	BOOST_REQUIRE_EQUAL(lr, ICERR_OK);

	lr = ICDecompressBegin(hicDecode, &bihCompressed, &bihDst);
	BOOST_REQUIRE_EQUAL(lr, ICERR_OK);

	void *pSrcData;
	void *pDstData;
	void *pEncoderOut = malloc(cbCompressedData);
	void *pDecoderOut = NULL;
	int retSrc, retDst;
	LONG lFrameNum = 0;
	while ((retSrc = srcClip.GetNextFrame(&pSrcData, &cbSrcData, NULL)) == 0 &&
		(retDst = dstClip.GetNextFrame(&pDstData, &cbDstData, NULL) == 0))
	{
		DWORD dwFlags = 0;
		lr = ICCompress(hicEncode, 0, &bihCompressed, pEncoderOut, &bihSrc, pSrcData, NULL, &dwFlags, lFrameNum++, 0, 0, &bihSrc, NULL);
		BOOST_REQUIRE(lr == ICERR_OK);

		if (pDecoderOut == NULL)
			pDecoderOut = malloc(cbDstData);

		lr = ICDecompress(hicDecode, (dwFlags & AVIIF_KEYFRAME) ? 0 : ICDECOMPRESS_NOTKEYFRAME, &bihCompressed, pEncoderOut, &bihDst, pDecoderOut);
		BOOST_REQUIRE(lr == ICERR_OK);

		BOOST_CHECK(bihDst.biSizeImage == cbDstData);
		BOOST_CHECK(CompareFrame(pDstData, pDecoderOut, nWidth, cbDstData, fccDst, tolerance) == 0);
	}

	BOOST_CHECK(retSrc != 0 && retDst != 0);
	if (pDecoderOut != NULL)
		free(pDecoderOut);
	free(pEncoderOut);

	lr = ICDecompressEnd(hicDecode);
	BOOST_CHECK(lr == ICERR_OK);

	lr = ICCompressEnd(hicEncode);
	BOOST_CHECK(lr == ICERR_OK);
}
