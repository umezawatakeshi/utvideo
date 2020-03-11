/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

enum class VALUERANGE
{
	FULL,
	LIMITED,
	NOROUND, // ビット数を減らす変換を行う際に、丸め処理が不要であることを示す。 (e.g. P210)
};

template<VALUERANGE VR> static inline uint16_t Convert16To10(uint16_t x);
template<VALUERANGE VR> static inline uint16_t Convert10To16(uint16_t x);

static inline uint16_t Convert16To10Fullrange(uint16_t x)
{
	return (x - (x >> 10) + (1 << 5)) >> 6;
}

static inline uint16_t Convert10To16Fullrange(uint16_t x)
{
	// x が上位 6bit 領域に 1 を持っていても問題ない書き方にしてある。
	uint16_t tmp = x << 6;
	return tmp | (tmp >> 10);
}

static inline uint16_t Convert16To10Limited(uint16_t x)
{
	if (x >= 0xffc0)
		return 0x3ff;
	else
		return (x + (1 << 5)) >> 6;
}

static inline uint16_t Convert10To16Limited(uint16_t x)
{
	return x << 6;
}

static inline uint16_t Convert16To10Noround(uint16_t x)
{
	return x >> 6;
}

template<> static inline uint16_t Convert16To10<VALUERANGE::FULL>(uint16_t x)
{
	return Convert16To10Fullrange(x);
}

template<> static inline uint16_t Convert16To10<VALUERANGE::LIMITED>(uint16_t x)
{
	return Convert16To10Limited(x);
}

template<> static inline uint16_t Convert16To10<VALUERANGE::NOROUND>(uint16_t x)
{
	return Convert16To10Noround(x);
}

template<> static inline uint16_t Convert10To16<VALUERANGE::FULL>(uint16_t x)
{
	return Convert10To16Fullrange(x);
}

template<> static inline uint16_t Convert10To16<VALUERANGE::LIMITED>(uint16_t x)
{
	return Convert10To16Limited(x);
}

template<class C, class T> void cpp_ConvertULY2ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template<class C, class T> void cpp_ConvertRGBToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template<class C, class T> void cpp_ConvertULY4ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template<class C, class T> void cpp_ConvertRGBToULY4(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template<class C, class T> void cpp_ConvertULY0ToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);
template<class C, class T> void cpp_ConvertRGBToULY0(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t dwYPlaneGrossWidth, bool bInterlace);

template<class T> void cpp_ConvertRGBToULRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template<class T> void cpp_ConvertARGBToULRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template<class T> void cpp_ConvertYUV422ToULY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);
template<class T> void cpp_ConvertULRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template<class T> void cpp_ConvertULRAToARGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride, size_t cbPlaneWidth);
template<class T> void cpp_ConvertULY2ToYUV422(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride, size_t cbYWidth, size_t cbCWidth);

template<class T> void cpp_ConvertRGBToUQRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template<class T> void cpp_ConvertUQRGToRGB(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertB64aToUQRA(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
void cpp_ConvertUQRAToB64a(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);

void cpp_ConvertV210ToUQY2(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
void cpp_ConvertUQY2ToV210(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, unsigned int nWidth, ssize_t scbStride);
void cpp_ConvertR210ToUQRG(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, unsigned int nWidth, ssize_t scbStride);
void cpp_ConvertUQRGToR210(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, unsigned int nWidth, ssize_t scbStride);

template<VALUERANGE VR> void cpp_ConvertLittleEndian16ToHostEndian10(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
template<VALUERANGE VR> void cpp_ConvertHostEndian10ToLittleEndian16(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrc);

template<VALUERANGE VR> void cpp_ConvertPackedUVLittleEndian16ToPlanarHostEndian10(uint8_t* pUBegin, uint8_t* pVBegin, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd);
template<VALUERANGE VR> void cpp_ConvertPlanarHostEndian10ToPackedUVLittleEndian16(uint8_t* pSrcBegin, uint8_t* pSrcEnd, const uint8_t* pUBegin, const uint8_t* pVBegin);
