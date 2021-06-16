/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"
#include "test_clip.h"
#include "expand.h"
#include "tuple_container.h"
#include "VideoClip.h"
#include "ICCloser.h"

int CompareExtraDataWithMask(const void *p, size_t szp, const void *q, size_t szq, const void *m, size_t szm)
{
	size_t len = std::max(szp, szq);
	const uint8_t *pp = (const uint8_t *)p;
	const uint8_t *qq = (const uint8_t *)q;
	const uint8_t *mm = (const uint8_t *)m;

	for (size_t i = 0; i < len; ++i)
	{
		uint8_t ppp = (i < szp) ? pp[i] : 0;
		uint8_t qqq = (i < szq) ? qq[i] : 0;
		uint8_t mmm = (i < szm) ? mm[i] : 0xff;
		if ((ppp & mmm) != (qqq & mmm))
			return -1;
	}
	return 0;
}

BOOST_TEST_DECORATOR(*depends_on("vcm_ICOpen_encoder"))
BOOST_DATA_TEST_CASE(vcm_encode, make_data_from_tuple_container(vecEncodeClips), rawfilename, comfilename, config, mask)
{
	VideoClip comClip(comfilename);
	VideoClip rawClip(rawfilename);

	BOOST_REQUIRE(comClip.GetWidth() == rawClip.GetWidth());
	BOOST_REQUIRE(comClip.GetHeight() == rawClip.GetHeight());

	DWORD fccCodec = comClip.GetFourCC();
	DWORD fccRaw = rawClip.GetFourCC();
	unsigned int nWidth = comClip.GetWidth();
	unsigned int nHeight = comClip.GetHeight();

	void *pCompressedData = NULL;
	void *pRawData = NULL;
	size_t cbCompressedData;
	size_t cbRawData;
	bool bKeyFrame;

	HIC hic;
	LRESULT lr;
	union
	{
		BITMAPINFOHEADER bihCompressed;
		char bihCompressedBuf[128];
	};
	BITMAPINFOHEADER bihEncoderIn;
	size_t cbExtraData;
	char byExtraData[128];

	hic = ICOpen(ICTYPE_VIDEO, fccCodec, ICMODE_COMPRESS);
	BOOST_REQUIRE(hic != NULL);
	ICCloser iccloser(hic);

	lr = ICSetState(hic, &config.front(), config.size());
	BOOST_REQUIRE(lr == config.size());

	cbExtraData = comClip.GetExtraData(byExtraData, sizeof(byExtraData));

	memset(&bihEncoderIn, 0, sizeof(BITMAPINFOHEADER));
	bihEncoderIn.biSize = sizeof(BITMAPINFOHEADER);
	bihEncoderIn.biWidth = nWidth;
	bihEncoderIn.biHeight = nHeight;
	bihEncoderIn.biPlanes = 1;
	bihEncoderIn.biBitCount = FCC2BitCount(fccRaw);
	bihEncoderIn.biCompression = FCC2Compression(fccRaw);
	bihEncoderIn.biSizeImage = 10000000;

	lr = ICCompressGetFormat(hic, &bihEncoderIn, &bihCompressed);
	BOOST_REQUIRE(CompareExtraDataWithMask(&bihCompressed + 1, bihCompressed.biSize - sizeof(BITMAPINFOHEADER), byExtraData, cbExtraData, &mask.front(), mask.size()) == 0);

	size_t cbEncoderOut = ICCompressGetSize(hic, &bihEncoderIn, &bihCompressed);
	void *pEncoderOut = malloc(cbEncoderOut);

	lr = ICCompressBegin(hic, &bihEncoderIn, &bihCompressed);
	BOOST_REQUIRE_EQUAL(lr, ICERR_OK);

	int retCompressed, retRaw;
	LONG lFrameNum = 0;
	while ((retCompressed = comClip.GetNextFrame(&pCompressedData, &cbCompressedData, &bKeyFrame)) == 0 &&
		(retRaw = rawClip.GetNextFrame(&pRawData, &cbRawData, NULL) == 0))
	{
		DWORD dwFlags = 0;
		lr = ICCompress(hic, 0, &bihCompressed, pEncoderOut, &bihEncoderIn, pRawData, NULL, &dwFlags, lFrameNum++, 0, 0, &bihEncoderIn, NULL);
		BOOST_REQUIRE(lr == ICERR_OK);
		/*
		 * 現在の FFmpeg は UMxx であっても UtVideo を intra only だと思っており、
		 * そのようなコーデックでは AVI index にキーフレームフラグが立ってなくても
		 * キーフレームフラグを付けて返してくる (compute_pkt_fields() in libavformat/utils.c) ので、
		 * このチェックは正しく動作しない。
		 */
		//BOOST_CHECK(((dwFlags & AVIIF_KEYFRAME) != 0) == bKeyFrame); // 珍妙な書き方だがこうしないと警告が出る…

		BOOST_CHECK(bihCompressed.biSizeImage == cbCompressedData);
		BOOST_CHECK(memcmp(pEncoderOut, pCompressedData, cbCompressedData) == 0);
	}
	VideoClip::ReleaseFrame(&pCompressedData);
	VideoClip::ReleaseFrame(&pRawData);

	BOOST_CHECK(retCompressed != 0 && retRaw != 0);
	if (pEncoderOut != NULL)
		free(pEncoderOut);

	lr = ICCompressEnd(hic);
	BOOST_CHECK(lr == ICERR_OK);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
