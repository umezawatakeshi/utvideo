/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "CodecBase.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"


class CUL00Codec :
	public CCodecBase
{
protected:
	struct EXTRADATA
	{
		uint32_t EncoderVersionAndImplementation;
		uint32_t fccOriginalFormat;
		uint32_t cbFrameInfo;
		uint32_t flags0;
	};

	static const uint32_t BIE_FLAGS0_DIVIDE_COUNT_MASK     = 0xff000000;
	static const uint32_t BIE_FLAGS0_DIVIDE_COUNT_SHIFT    = 24;

	static const uint32_t BIE_FLAGS0_COMPRESS_MASK         = 0x00000001;
	static const uint32_t BIE_FLAGS0_COMPRESS_NONE         = 0x00000000;
	static const uint32_t BIE_FLAGS0_COMPRESS_HUFFMAN_CODE = 0x00000001;

	static const uint32_t BIE_FLAGS0_ASSUME_INTERLACE      = 0x00000800;

	static const uint32_t BIE_FLAGS0_RESERVED              = 0x00fff7fe;


	struct FRAMEINFO
	{
		uint32_t dwFlags0;
	};

	static const uint32_t FI_FLAGS0_INTRAFRAME_PREDICT_MASK         = 0x00000300;
	static const uint32_t FI_FLAGS0_INTRAFRAME_PREDICT_NONE         = 0x00000000;
	static const uint32_t FI_FLAGS0_INTRAFRAME_PREDICT_LEFT         = 0x00000100;
	static const uint32_t FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT     = 0x00000200;
	static const uint32_t FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN = 0x00000300;

	static const uint32_t FI_FLAGS0_RESERVED                        = 0xfffffcff;


	struct ENCODERCONF
	{
		uint32_t dwFlags0;
	};

	static const uint32_t EC_FLAGS0_DIVIDE_COUNT_MASK               = 0x000000ff;

	static const uint32_t EC_FLAGS0_INTRAFRAME_PREDICT_MASK         = 0x00000300;
	static const uint32_t EC_FLAGS0_INTRAFRAME_PREDICT_RESERVED     = 0x00000000;
	static const uint32_t EC_FLAGS0_INTRAFRAME_PREDICT_LEFT         = 0x00000100;
	static const uint32_t EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT     = 0x00000200;
	static const uint32_t EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN = 0x00000300;

	static const uint32_t EC_FLAGS0_ASSUME_INTERLACE                = 0x00000800;
	static const uint32_t EC_FLAGS0_DIVIDE_COUNT_IS_NUM_PROCESSORS  = 0x00001000;

	static const uint32_t EC_FLAGS0_RESERVED                        = 0xffffe400;

protected:
	ENCODERCONF m_ec;

	utvf_t m_utvfRaw;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	const void *m_pInput;
	void *m_pOutput;
	uint32_t m_dwNumStripes;
	uint32_t m_dwDivideCount;
	bool m_bInterlace;
	size_t m_cbRawStripeSize;
	size_t m_cbPlaneSize[4];
	size_t m_cbPlaneWidth[4];
	size_t m_cbPlaneStripeSize[4];
	size_t m_cbPlanePredictStride[4];
	uint32_t m_dwPlaneStripeBegin[256];
	uint32_t m_dwPlaneStripeEnd[256];
	uint32_t m_dwRawStripeBegin[256];
	uint32_t m_dwRawStripeEnd[256];

	std::unique_ptr<CThreadManager> m_ptm;
	std::unique_ptr<CFrameBuffer> m_pCurFrame;
	std::unique_ptr<CFrameBuffer> m_pPredicted;
	struct COUNTS
	{
		uint32_t dwCount[4][256];
	} *m_counts;
	/* const */ HUFFMAN_CODELEN_TABLE<8> *m_pCodeLengthTable[4];
	HUFFMAN_ENCODE_TABLE<8> m_het[4];

	FRAMEINFO m_fi;
	HUFFMAN_DECODE_TABLE<8> m_hdt[4];
	const uint8_t *m_pDecodeCode[4][256];

	EXTRADATA m_ed;

protected:
	CUL00Codec(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CUL00Codec(void) {}

public:
	virtual void GetLongFriendlyName(char *pszName, size_t cchName);

	virtual const utvf_t *GetEncoderInputFormat(void) = 0;
	virtual const utvf_t *GetDecoderOutputFormat(void) = 0;
	virtual const utvf_t *GetCompressedFormat(void) = 0;
	virtual bool IsTemporalCompressionSupported(void) { return false; }

#ifdef _WIN32
	virtual INT_PTR Configure(HWND hwnd);
	static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

	virtual size_t GetStateSize(void);
	virtual int GetState(void *pState, size_t cb);

	virtual int InternalEncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual size_t EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput);
	virtual int InternalEncodeEnd(void);
	virtual size_t EncodeGetExtraDataSize(void);
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height);
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height);
	virtual int InternalEncodeQuery(utvf_t infmt, unsigned int width, unsigned int height);

	virtual int InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual size_t DecodeFrame(void *pOutput, const void *pInput);
	virtual int DecodeGetFrameType(bool *pbKeyFrame, const void *pInput);
	virtual int InternalDecodeEnd(void);
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual int InternalDecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData);

protected:
	virtual int InternalSetState(const void *pState, size_t cb);
	int CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);

	virtual const char *GetColorFormatName(void) = 0;
	virtual int GetRealBitCount(void) = 0;
	virtual int GetNumPlanes(void) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height) = 0;
	virtual void ConvertToPlanar(uint32_t nBandIndex) = 0;
	virtual void ConvertFromPlanar(uint32_t nBandIndex) = 0;
	virtual bool PredictDirect(uint32_t nBandIndex);
	virtual bool DecodeDirect(uint32_t nBandIndex);

	void PredictFromPlanar(uint32_t nBandIndex, const uint8_t* const* pSrcBegin);
	void DecodeToPlanar(uint32_t nBandIndex, uint8_t* const* pDstBegin);

private:
	void PredictProc(uint32_t nBandIndex);
	void EncodeProc(uint32_t nBandIndex);
	void GenerateDecodeTableProc(uint32_t nPlaneIndex);
	void DecodeProc(uint32_t nBandIndex);

	class CThreadJob : public ::CThreadJob
	{
	public:
		typedef void (CUL00Codec::*JobProcType)(uint32_t nBandIndex);

	private:
		CUL00Codec *m_pCodec;
		JobProcType m_pfnJobProc;
		uint32_t m_nBandIndex;

	public:
		CThreadJob(CUL00Codec *pCodec, JobProcType pfnJobProc, uint32_t nBandIndex)
		{
			m_pCodec = pCodec;
			m_pfnJobProc = pfnJobProc;
			m_nBandIndex = nBandIndex;
		}

		void JobProc(CThreadManager *)
		{
			(m_pCodec->*m_pfnJobProc)(m_nBandIndex);
		}
	};
};
