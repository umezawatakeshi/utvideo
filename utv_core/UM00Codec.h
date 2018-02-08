/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "CodecBase.h"
#include "FrameBuffer.h"
#include "Thread.h"


class CUM00Codec :
	public CCodecBase
{
protected:
	struct ENCODERCONF
	{
		uint8_t ecFlags;
		uint8_t ecDivideCountMinusOne;
		uint8_t ecReserved[2];
	};

	static constexpr uint8_t EC_FLAGS_DIVIDE_COUNT_AUTO = 0x01;
	static constexpr uint8_t EC_FLAGS_RESERVED          = 0xfe;

	struct STREAMINFO
	{
		union
		{
			struct
			{
				uint8_t siEncoderImplementation;
				uint8_t siEncoderImplementationSpecificData[7];
			};
			struct
			{
				uint32_t siEncoderVersionAndImplementation;
				uint32_t siOriginalFormat;
			};
		};
		uint8_t siEncodingMode;
		uint8_t siDivideCountMinusOne;
		uint8_t siReserved[6];
	};

	static constexpr uint8_t SI_ENCODING_MODE_8SYMPACK = 2;

	struct FRAMEINFO
	{
		uint8_t fiFrameType;
		uint8_t fiReserved[3];
		uint32_t fiSizeArrayOffset;
	};

	// static constexpr uint8_t FI_FRAME_TYPE_COPY  = 0;
	static constexpr uint8_t FI_FRAME_TYPE_INTRA = 1;
	// static constexpr uint8_t FI_FRAME_TYPE_DELTA = 2;

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
	uint32_t m_dwPlaneStripeBegin[256];
	uint32_t m_dwPlaneStripeEnd[256];
	uint32_t m_dwRawStripeBegin[256];
	uint32_t m_dwRawStripeEnd[256];
	uint8_t* m_pPackedStream[4][256];
	size_t m_cbPackedStream[4][256];
	uint8_t* m_pControlStream[4][256];
	size_t m_cbControlStream[4][256];

	std::unique_ptr<CThreadManager> m_ptm;
	std::unique_ptr<CFrameBuffer> m_pCurFrame;
	std::unique_ptr<CFrameBuffer> m_pPredicted;

protected:
	CUM00Codec(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CUM00Codec(void) {}

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
	void EncodeProc(uint32_t nBandIndex);
	void DecodeProc(uint32_t nBandIndex);

	class CThreadJob : public ::CThreadJob
	{
	public:
		typedef void (CUM00Codec::*JobProcType)(uint32_t nBandIndex);

	private:
		CUM00Codec *m_pCodec;
		JobProcType m_pfnJobProc;
		uint32_t m_nBandIndex;

	public:
		CThreadJob(CUM00Codec *pCodec, JobProcType pfnJobProc, uint32_t nBandIndex)
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
