/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "utv_core.h"
#include "UTRDCodec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "resource.h"
#include "ByteOrder.h"
#include "TunedFunc_x86x64.h"
#include "Predict_x86x64.h"
#include <lz4.h>

const utvf_t CUTRDCodec::m_utvfEncoderInput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_INVALID,
};

const utvf_t CUTRDCodec::m_utvfDecoderOutput[] = {
	UTVF_NFCC_BGR_BU,
	UTVF_INVALID,
};

const utvf_t CUTRDCodec::m_utvfCompressed[] = {
	UTVF_UTRD,
	UTVF_INVALID,
};

CUTRDCodec::CUTRDCodec(const char *pszInterfaceName) : CCodecBase("UTRD", pszInterfaceName)
{
}

void CUTRDCodec::GetLongFriendlyName(char *pszName, size_t cchName)
{
	char buf[128];

	sprintf(buf, "UtVideo R&D");
	strncpy(pszName, buf, cchName);
	pszName[cchName - 1] = '\0';
}

#ifdef _WIN32
INT_PTR CUTRDCodec::Configure(HWND hwnd)
{
	return 0;
}
#endif

size_t CUTRDCodec::GetStateSize(void)
{
	return 0;
}

int CUTRDCodec::GetState(void *pState, size_t cb)
{
	return 0;
}

int CUTRDCodec::InternalSetState(const void *pState, size_t cb)
{
	return 0;
}

size_t CUTRDCodec::EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput)
{
	uint8_t *p;

	m_pInput = pInput;
	m_pOutput = pOutput;

	ConvertBGRToULRG(
		m_pCurFrame->GetPlane(1),
		m_pCurFrame->GetPlane(0),
		m_pCurFrame->GetPlane(2),
		((const uint8_t *)m_pInput) + m_nWidth * (m_nHeight - 1) * 3,
		((const uint8_t *)m_pInput) - m_nWidth * 3,
		m_nWidth * 3,
		-(ssize_t)m_nWidth * 3);

	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
//		uint32_t counts[256];
/*		PredictCylindricalLeftAndCount8(
			m_pMedianPredicted->GetPlane(nPlaneIndex),
			m_pCurFrame->GetPlane(nPlaneIndex),
			m_pCurFrame->GetPlane(nPlaneIndex) + m_nWidth * m_nHeight,
			 counts);*/
		//PredictPlanarGradientAndCount8
		//PredictCylindricalWrongMedianAndCount8
		tuned_PredictPlanarGradient8<CODEFEATURE_AVX1>
		(
			m_pMedianPredicted->GetPlane(nPlaneIndex),
			m_pCurFrame->GetPlane(nPlaneIndex),
			m_pCurFrame->GetPlane(nPlaneIndex) + m_nWidth * m_nHeight,
			m_nWidth);
	}

	p = (uint8_t *)pOutput;

//	volatile unsigned n[4] = { 0, 0, 0, 0 };
	uint8_t *index = (uint8_t*)malloc(m_nWidth * m_nHeight);
	for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		int shift = 0;
		memset(index, 0, m_nWidth * m_nHeight);
		uint8_t *idxp = index;
		uint32_t *pCompressedLen = (uint32_t*)p;
		uint32_t *pModeLen = (uint32_t*)(p + 4);
		p += 8;
		auto pCompressedBegin = p;
		for (unsigned y = 0; y < m_nHeight; y += 1)
		{
			for (unsigned x = 0; x < m_nWidth; x += 16)
			{
				__m128i w = _mm_loadu_si128((const __m128i *)&m_pMedianPredicted->GetPlane(nPlaneIndex)[y * m_nWidth + x]);
				__m128i visnotzero = _mm_cmpeq_epi64(_mm_cmpeq_epi64(w, _mm_setzero_si128()), _mm_setzero_si128());

				int mode0, mode1;
				int bits0, bits1;

				{
					__m128i z;
					__m128i signs = _mm_cmpgt_epi8(_mm_setzero_si128(), w);
					int isnotzero0 = _mm_cvtsi128_si32(visnotzero);
					int isnotzero1 = _mm_extract_epi32(visnotzero, 2);
					z = _mm_xor_si128(w, signs);
					z = _mm_or_si128(z, _mm_srli_epi64(z, 32));
					z = _mm_or_si128(z, _mm_srli_epi64(z, 16));
					z = _mm_or_si128(z, _mm_srli_epi64(z, 8));
					z = _mm_and_si128(z, _mm_set1_epi64x(0xff));
					z = _mm_or_si128(z, _mm_set1_epi64x(1));
					_BitScanReverse((unsigned long *)&mode0, _mm_cvtsi128_si32(z));
					_BitScanReverse((unsigned long *)&mode1, _mm_extract_epi32(z, 2));
					bits0 = mode0 + 2;
					int rembits0 = 6 - mode0;
					mode0++;
					bits1 = mode1 + 2;
					int rembits1 = 6 - mode1;
					mode1++;

					bits0 &= isnotzero0;
					mode0 &= isnotzero0;
					bits1 &= isnotzero1;
					mode1 &= isnotzero1;
					__m128i vrembits0 = _mm_cvtsi64x_si128(rembits0);
					__m128i vrembits1 = _mm_cvtsi64x_si128(rembits1);
					__m128i w0 = _mm_add_epi8(w, _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits0));
					__m128i w1 = _mm_add_epi8(_mm_srli_si128(w, 8), _mm_srl_epi64(_mm_set1_epi8((char)0x80), vrembits1));
					w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w0), vrembits0));
					w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi16(0x00ff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi16(0x00ff), w1), vrembits1));
					vrembits0 = _mm_slli_epi64(vrembits0, 1);
					vrembits1 = _mm_slli_epi64(vrembits1, 1);
					w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w0), vrembits0));
					w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi32(0x0000ffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi32(0x0000ffff), w1), vrembits1));
					vrembits0 = _mm_slli_epi64(vrembits0, 1);
					vrembits1 = _mm_slli_epi64(vrembits1, 1);
					w0 = _mm_or_si128(_mm_and_si128(w0, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w0), vrembits0));
					w1 = _mm_or_si128(_mm_and_si128(w1, _mm_set1_epi64x(0x00000000ffffffff)), _mm_srl_epi64(_mm_andnot_si128(_mm_set1_epi64x(0x00000000ffffffff), w1), vrembits1));
					*(uint64_t*)p = _mm_cvtsi128_si64x(w0);
					*(uint64_t*)(p+bits0) = _mm_cvtsi128_si64x(w1);
				}

				p += bits0 + bits1;
#define MODEBITS 3
				*(uint32_t *)idxp |= (((mode1 << MODEBITS) | mode0) << shift);
				shift += MODEBITS*2;
				if (shift == MODEBITS*8)
				{
					idxp += MODEBITS;
					shift = 0;
				}
			}
		}
		*pCompressedLen = p - pCompressedBegin;
		size_t idxlen = idxp - index;
#define USE_LZ4 1
#if USE_LZ4
		*pModeLen = LZ4_compress_default((const char*)index, (char*)p, idxlen, m_nWidth * m_nHeight);
#else
		*pModeLen = idxlen;
		memcpy(p, index, idxlen);
#endif
		p += *pModeLen;
	}

	free(index);
	*pbKeyFrame = true;

	return p - ((uint8_t *)pOutput);
}

int CUTRDCodec::InternalEncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	int ret;

	ret = EncodeQuery(infmt, width, height);
	if (ret != 0)
		return ret;

	m_utvfRaw = infmt;
	m_nWidth = width;
	m_nHeight = height;


	m_pCurFrame = new CFrameBuffer();
	for (int i = 0; i < 3; i++)
		m_pCurFrame->AddPlane(width*height, width*2);

	m_pMedianPredicted = new CFrameBuffer();
	for (int i = 0; i < 3; i++)
		m_pMedianPredicted->AddPlane(width*height, width * 2);

	return 0;
}

int CUTRDCodec::InternalEncodeEnd(void)
{
	delete m_pCurFrame;
	delete m_pMedianPredicted;

	return 0;
}

size_t CUTRDCodec::EncodeGetExtraDataSize(void)
{
	return 0;
}

int CUTRDCodec::EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height)
{
	return 0;
}

size_t CUTRDCodec::EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height)
{
	return ROUNDUP(width, 4) * ROUNDUP(height, 2) * 24 / 8 + 4096; // +4096 ÇÕÇ«ÇÒÇ‘ÇËä®íËÅB
}

int CUTRDCodec::InternalEncodeQuery(utvf_t infmt, unsigned int width, unsigned int height)
{
	for (const utvf_t *utvf = GetEncoderInputFormat(); *utvf; utvf++)
	{
		if (infmt == *utvf)
			return 0;
	}

	return -1;
}

size_t CUTRDCodec::DecodeFrame(void *pOutput, const void *volatile pInput)
{
	const uint8_t* pPlaneBegin[3];

	{
		const uint8_t *p = (const uint8_t*)pInput;
		for (int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
		{
			pPlaneBegin[nPlaneIndex] = p;
			uint32_t cbCompressed = *(uint32_t*)p;
			uint32_t cbMode = *(uint32_t*)(p + 4);
			p += 8;
			p += cbCompressed + cbMode;
		}
	}

	const uint8_t *p = (const uint8_t*)pInput;

	uint8_t *index = (uint8_t*)malloc(m_nWidth * m_nHeight);
	for (volatile int nPlaneIndex = 0; nPlaneIndex < 3; nPlaneIndex++)
	{
		_ASSERT(p == pPlaneBegin[nPlaneIndex]);
		uint32_t cbCompressed = *(uint32_t*)p;
		uint32_t cbMode = *(uint32_t*)(p + 4);
		p += 8;
#if USE_LZ4
		LZ4_decompress_safe((const char *)(p+cbCompressed),(char*)index, cbMode, m_nWidth * m_nHeight);
#else
		memcpy(index, p + cbCompressed, cbMode);
#endif
		int shift = 0;
		uint8_t *idxp = index;

		for (unsigned y = 0; y < m_nHeight; y += 1)
		{
			for (unsigned x = 0; x < m_nWidth; x += 32)
			{
				__m256i w;
				int modes = ((*(uint32_t *)idxp) >> shift);
				int bits0 = modes & 7;
				if (bits0 != 0)
					bits0++;
				int bits1 = (modes>>3) & 7;
				if (bits1 != 0)
					bits1++;
				int bits2 = (modes>>6) & 7;
				if (bits2 != 0)
					bits2++;
				int bits3 = (modes >> 9) & 7;
				if (bits3 != 0)
					bits3++;

				{
					__m128i w0, w1, vbits0, vbits1;
					__m256i z, mask;
					__m256i vbits;

					w0 = _mm_cvtsi64_si128(*(uint64_t *)p);
					p += bits0;
					w0 = _mm_insert_epi64(w0, *(uint64_t *)p, 1);
					p += bits1;
					w1 = _mm_cvtsi64_si128(*(uint64_t *)p);
					p += bits2;
					w1 = _mm_insert_epi64(w1, *(uint64_t *)p, 1);
					p += bits3;
					w = _mm256_inserti128_si256(_mm256_castsi128_si256(w0), w1, 1);

					vbits0 = _mm_cvtsi64_si128(bits0);
					vbits0 = _mm_insert_epi64(vbits0, bits1, 1);
					vbits1 = _mm_cvtsi64_si128(bits2);
					vbits1 = _mm_insert_epi64(vbits1, bits3, 1);
					vbits = _mm256_inserti128_si256(_mm256_castsi128_si256(vbits0), vbits1, 1);

					__m256i vrembits = _mm256_sub_epi64(_mm256_set1_epi64x(8), vbits);

					__m256i vrembitsn = _mm256_slli_epi64(vrembits, 2);
					z = _mm256_sllv_epi64(w, vrembitsn);
					w = _mm256_blend_epi16(w, z, 0xcc);
					vrembitsn = _mm256_srli_epi64(vrembitsn, 1);
					z = _mm256_sllv_epi64(w, vrembitsn);
					w = _mm256_blend_epi16(w, z, 0xaa);
					z = _mm256_sllv_epi64(w, vrembits);
					w = _mm256_blendv_epi8(w, z, _mm256_set1_epi16((short)0xff00));
					mask = _mm256_sub_epi64(_mm256_sllv_epi64(_mm256_set1_epi8(1), vbits), _mm256_set1_epi8(1));
					w = _mm256_and_si256(w, mask);
					__m256i offset = _mm256_and_si256(_mm256_srlv_epi64(_mm256_set1_epi8((char)0x80), vrembits), mask);
					w = _mm256_sub_epi8(w, offset);
				}

				_mm256_storeu_si256((__m256i*)&m_pDecodedFrame->GetPlane(nPlaneIndex)[y * m_nWidth + x], w);

				shift += MODEBITS*4;
				if (shift == MODEBITS*8)
				{
					idxp += MODEBITS;
					shift = 0;
				}
			}
		}
		p += cbMode;

		//RestoreCylindricalWrongMedian8
		RestorePlanarGradient8
		(
			m_pRestoredFrame->GetPlane(nPlaneIndex),
			m_pDecodedFrame->GetPlane(nPlaneIndex),
			m_pDecodedFrame->GetPlane(nPlaneIndex) + m_nWidth * m_nHeight,
			m_nWidth);
	}

	ConvertULRGToBGR(
		((uint8_t*)pOutput) + m_nWidth * 3 * (m_nHeight - 1),
		((uint8_t*)pOutput) - m_nWidth * 3,
		m_pRestoredFrame->GetPlane(1),
		m_pRestoredFrame->GetPlane(0),
		m_pRestoredFrame->GetPlane(2),
		m_nWidth * 3,
		-(ssize_t)m_nWidth * 3);
	free(index);
	return m_cbRawSize;
}

int CUTRDCodec::DecodeGetFrameType(bool *pbKeyFrame, const void *pInput)
{
	*pbKeyFrame = true;
	return 0;
}

int CUTRDCodec::InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	int ret;

	ret = DecodeQuery(outfmt, width, height, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	m_utvfRaw = outfmt;
	m_nWidth = width;
	m_nHeight = height;

	m_pRestoredFrame = new CFrameBuffer();
	for (int i = 0; i < 3; i++)
		m_pRestoredFrame->AddPlane(width*height, width * 2);

	m_pDecodedFrame = new CFrameBuffer();
	for (int i = 0; i < 3; i++)
		m_pDecodedFrame->AddPlane(width*height, width * 2);

	return 0;
}

int CUTRDCodec::InternalDecodeEnd(void)
{
	delete m_pRestoredFrame;
	delete m_pDecodedFrame;

	return 0;
}

size_t CUTRDCodec::DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	int ret;

	ret = CalcRawFrameMetric(outfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return 0;

	return m_cbRawSize;
}

int CUTRDCodec::InternalDecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData)
{
	if (!outfmt)
		return 0;

	for (const utvf_t *utvf = GetDecoderOutputFormat(); *utvf; utvf++)
	{
		if (outfmt == *utvf)
			return 0;
	}

	return -1;
}
