/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_clip.h"
#include "expand.h"
#include "tuple_container.h"
#include "VideoClip.h"

int CompareFrame(const void *frame1, const void *frame2, unsigned int width, size_t size, DWORD format, int tolerance)
{
	(void)tolerance; // XXX

	switch (format)
	{
	case 24:
		{
			const char *begin1 = (const char *)frame1;
			const char *begin2 = (const char *)frame2;
			const char *end1 = begin1 + size;
			size_t net = width * (size_t)3;
			size_t stride = (net + 3) & ~(size_t)3;
			for (auto p = begin1, q = begin2; p < end1; p += stride, q += stride)
			{
				int r = memcmp(p, q, net);
				if (r != 0)
					return r;
			}
			return 0;
		}
	}

	return memcmp(frame1, frame2, size);
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_decoder"))
BOOST_DATA_TEST_CASE(vcm_decode, make_data_from_tuple_container(vecDecodeClips), compressed, decoded, tolerance)
{
	VideoClip comClip(compressed);
	VideoClip decClip(decoded);

	BOOST_REQUIRE(comClip.GetWidth() == decClip.GetWidth());
	BOOST_REQUIRE(comClip.GetHeight() == decClip.GetHeight());

	DWORD fccCodec = comClip.GetFourCC();
	DWORD fccDecoded = decClip.GetFourCC();
	unsigned int nWidth = comClip.GetWidth();
	unsigned int nHeight = comClip.GetHeight();

	void *pCompressedData;
	void *pDecodedData;
	size_t cbCompressedData;
	size_t cbDecodedData;
	bool bKeyFrame;

	HIC hic;
	LRESULT lr;
	union
	{
		BITMAPINFOHEADER bihCompressed;
		char bihCompressedBuf[128];
	};
	BITMAPINFOHEADER bihDecoderOut;
	size_t cbExtraData;

	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_DECOMPRESS);
	BOOST_REQUIRE(hic != NULL);

	cbExtraData = comClip.GetExtraData(&bihCompressed + 1, sizeof(bihCompressedBuf) - sizeof(bihCompressed));

	memset(&bihCompressed, 0, sizeof(BITMAPINFOHEADER));
	bihCompressed.biSize = sizeof(BITMAPINFOHEADER) + (DWORD)cbExtraData;
	bihCompressed.biWidth = nWidth;
	bihCompressed.biHeight = nHeight;
	bihCompressed.biPlanes = 1;
	bihCompressed.biBitCount = FCC2BitCount(fccCodec);
	bihCompressed.biCompression = FCC2Compression(fccCodec);
	bihCompressed.biSizeImage = 10000000;

	memset(&bihDecoderOut, 0, sizeof(BITMAPINFOHEADER));
	bihDecoderOut.biSize = sizeof(BITMAPINFOHEADER);
	bihDecoderOut.biWidth = nWidth;
	bihDecoderOut.biHeight = nHeight;
	bihDecoderOut.biPlanes = 1;
	bihDecoderOut.biBitCount = FCC2BitCount(fccDecoded);
	bihDecoderOut.biCompression = FCC2Compression(fccDecoded);
	bihDecoderOut.biSizeImage = 10000000;

	lr = ICDecompressBegin(hic, &bihCompressed, &bihDecoderOut);
	BOOST_REQUIRE_EQUAL(lr, ICERR_OK);

	void *pDecoderOut = NULL;
	int retCompressed, retDecoded;
	while ((retCompressed = comClip.GetNextFrame(&pCompressedData, &cbCompressedData, &bKeyFrame)) == 0 &&
		(retDecoded = decClip.GetNextFrame(&pDecodedData, &cbDecodedData, NULL) == 0))
	{
		if (pDecoderOut == NULL)
			pDecoderOut = malloc(cbDecodedData);

		lr = ICDecompress(hic, bKeyFrame ? 0 : ICDECOMPRESS_NOTKEYFRAME, &bihCompressed, pCompressedData, &bihDecoderOut, pDecoderOut);
		BOOST_REQUIRE(lr == ICERR_OK);

		BOOST_CHECK(bihDecoderOut.biSizeImage == cbDecodedData);
		BOOST_CHECK(CompareFrame(pDecodedData, pDecoderOut, nWidth, cbDecodedData, fccDecoded, tolerance) == 0);
	}

	BOOST_CHECK(retCompressed != 0 && retDecoded != 0);
	if (pDecoderOut != NULL)
		free(pDecoderOut);

	lr = ICDecompressEnd(hic);
	BOOST_CHECK(lr == ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
