/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "CodecBase.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"


class CUQ00Codec :
	public CCodecBase
{
protected:
	struct ENCODERCONF
	{
		uint8_t ecPreferredEncodingMode; /* == 0 */
		uint8_t ecReserved; /* == 0 */
		uint8_t ecDivideCountMinusOne;
		uint8_t ecFlags;
	};

	static const uint8_t EC_FLAGS_DIVIDE_COUNT_IS_NUM_PROCESSORS = 0x01;
	static const uint8_t EC_FLAGS_RESERVED                       = 0xfe;

	struct STREAMINFO
	{
		union
		{
			uint32_t siEncoderVersionAndImplementation;
			struct
			{
				uint8_t siEncoderImplementation;
				uint8_t siEncoderVersion[3];
			};
		};
		uint32_t siOriginalFormat; /* in case of "original" implementation, UTVF_* values are stored in "readable order" */
	};

	/* placeholder structure */
	struct FRAMEINFO
	{
		uint8_t fiEncodingMode;
		uint8_t fiReserved[3];
	};

	struct FRAMEINFO_MODE0
	{
		uint8_t fiEncodingMode; /* == 0 */
		uint8_t fiPredictionType;
		uint8_t fiDivideCountMinusOne;
		uint8_t fiReserved; /* == 0 */
	};

	static const uint8_t PREDICT_CYLINDRICAL_LEFT   = 1;
	//static const uint8_t PREDICT_CYLINDRICAL_MEDIAN = 3;
	//static const uint8_t PREDICT_ABOVE              = 4;
	//static const uint8_t PREDICT_PLANAR_LEFT        = 5;
	//static const uint8_t PREDICT_PLANAR_MEDIAN      = 7;

protected:
	ENCODERCONF m_ec;

	utvf_t m_utvfRaw;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	const void *m_pInput;
	void *m_pOutput;
	uint32_t m_dwNumStripes;
	uint32_t m_dwDivideCount;
	size_t m_cbRawStripeSize;
	size_t m_cbPlaneSize[4];
	size_t m_cbPlaneWidth[4];
	size_t m_cbPlaneStripeSize[4];
	size_t m_cbPlanePredictStride[4];
	uint32_t m_dwStripeBegin[256];
	uint32_t m_dwStripeEnd[256];

	std::unique_ptr<CThreadManager> m_ptm;
	std::unique_ptr<CFrameBuffer> m_pCurFrame;
	std::unique_ptr<CFrameBuffer> m_pPredicted;
	struct COUNTS
	{
		uint32_t dwCount[4][1024];
	} *m_counts;
	/* const */ HUFFMAN_CODELEN_TABLE<10> *m_pCodeLengthTable[4];
	HUFFMAN_ENCODE_TABLE<10> m_het[4];
	uint32_t *m_pdwOffsetTable[4];
	uint8_t *m_pEncodedBits[4];

	FRAMEINFO m_fi;
	HUFFMAN_DECODE_TABLE<10> m_hdt[4];

	STREAMINFO m_si;

protected:
	CUQ00Codec(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CUQ00Codec(void) {}

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
	void SetDefaultState();
	int CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	void CalcStripeMetric(void);

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
	void DecodeProc(uint32_t nBandIndex);

	class CThreadJob : public ::CThreadJob
	{
	public:
		typedef void (CUQ00Codec::*JobProcType)(uint32_t nBandIndex);

	private:
		CUQ00Codec *m_pCodec;
		JobProcType m_pfnJobProc;
		uint32_t m_nBandIndex;

	public:
		CThreadJob(CUQ00Codec *pCodec, JobProcType pfnJobProc, uint32_t nBandIndex)
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
