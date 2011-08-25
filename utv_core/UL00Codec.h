/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

struct BITMAPINFOEXT
{
	BITMAPINFOHEADER bih;
	DWORD dwEncoderVersionAndImplementation;
	DWORD fccOriginalFormat;
	DWORD dwFrameInfoSize;
	DWORD dwFlags0;
};

struct EXTRADATA
{
	uint32_t EncoderVersionAndImplementation;
	uint32_t fccOriginalFormat;
	uint32_t cbFrameInfo;
	uint32_t flags0;
};

#define BIE_FLAGS0_DIVIDE_COUNT_MASK             0xff000000
#define BIE_FLAGS0_DIVIDE_COUNT_SHIFT            24

#define BIE_FLAGS0_COMPRESS_MASK                 0x00000001
#define BIE_FLAGS0_COMPRESS_NONE                 0x00000000
#define BIE_FLAGS0_COMPRESS_HUFFMAN_CODE         0x00000001

#define BIE_FLAGS0_ASSUME_INTERLACE              0x00000800

#define BIE_FLAGS0_RESERVED                      0x00fff7fe


struct FRAMEINFO
{
	DWORD dwFlags0;
};

#define FI_FLAGS0_INTRAFRAME_PREDICT_MASK        0x00000300
#define FI_FLAGS0_INTRAFRAME_PREDICT_NONE        0x00000000
#define FI_FLAGS0_INTRAFRAME_PREDICT_LEFT        0x00000100
#define FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT    0x00000200
#define FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN      0x00000300

#define FI_FLAGS0_RESERVED                       0xfffffcff


struct ENCODERCONF
{
	DWORD dwFlags0;
};

#define EC_FLAGS0_DIVIDE_COUNT_MASK              0x000000ff

#define EC_FLAGS0_INTRAFRAME_PREDICT_MASK        0x00000300
#define EC_FLAGS0_INTRAFRAME_PREDICT_RESERVED    0x00000000
#define EC_FLAGS0_INTRAFRAME_PREDICT_LEFT        0x00000100
#define EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN      0x00000300

#define EC_FLAGS0_ASSUME_INTERLACE               0x00000800
#define EC_FLAGS0_DIVIDE_COUNT_IS_NUM_PROCESSORS 0x00001000

#define EC_FLAGS0_RESERVED                       0xffffe400


class CUL00Codec :
	public CCodec
{
protected:
	const char *const m_pszInterfaceName;

	ENCODERCONF m_ec;

	utvf_t m_utvfRaw;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	const void *m_pInput;
	void *m_pOutput;
	bool m_bBottomUpFrame;
	DWORD m_dwNumStripes;
	DWORD m_dwDivideCount;
	bool m_bInterlace;
	DWORD m_dwRawSize;
	DWORD m_dwRawGrossWidth;
	DWORD m_dwRawNetWidth;
	DWORD m_dwPlaneSize[4];
	DWORD m_dwPlaneWidth[4];
	DWORD m_dwPlaneStripeSize[4];
	DWORD m_dwPlanePredictStride[4];
	DWORD m_dwPlaneStripeBegin[256];
	DWORD m_dwPlaneStripeEnd[256];
	DWORD m_dwRawStripeBegin[256];
	DWORD m_dwRawStripeEnd[256];
	DWORD m_dwRawStripeSize;

	CThreadManager *m_ptm;
	CFrameBuffer *m_pCurFrame;
	CFrameBuffer *m_pMedianPredicted;
	struct COUNTS
	{
		DWORD dwCount[4][256];
	} *m_counts;
	/* const */ uint8_t *m_pCodeLengthTable[4];
	HUFFMAN_ENCODE_TABLE m_het[4];

	CFrameBuffer *m_pRestoredFrame;
	CFrameBuffer *m_pDecodedFrame;
	FRAMEINFO m_fi;
	HUFFMAN_DECODE_TABLE m_hdt[4];
	const uint8_t *m_pDecodeCode[4][256];

	EXTRADATA m_ed;

public:
	CUL00Codec(const char *pszInterfaceName);
	virtual ~CUL00Codec(void);

public:
	virtual const char *GetTinyName(void) = 0;
	virtual void GetShortFriendlyName(char *pszName, size_t cchName);
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName);
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
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
	virtual int SetState(const void *pState, size_t cb);

	virtual int EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual size_t EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput);
	virtual int EncodeEnd(void);
	virtual size_t EncodeGetExtraDataSize(void);
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual int EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);

	virtual int DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual size_t DecodeFrame(void *pOutput, const void *pInput, bool bKeyFrame);
	virtual int DecodeEnd(void);
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual int DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);

protected:
	int LoadConfig(void);
	int SaveConfig(void);
	int InternalSetState(const void *pState, size_t cb);
	int CalcRawFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	int CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual const char *GetColorFormatName(void) = 0;
	virtual int GetRealBitCount(void) = 0;
	virtual int GetNumPlanes(void) = 0;
	virtual void CalcPlaneSizes(unsigned int width, unsigned int height) = 0;
	virtual void ConvertToPlanar(DWORD nBandIndex) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

	virtual void ConvertFromPlanar(DWORD nBandIndex) = 0;
	virtual bool DecodeDirect(DWORD nBandIndex);

private:
	void PredictProc(DWORD nBandIndex);
	class CPredictJob : public CThreadJob
	{
	private:
		DWORD m_nBandIndex;
		CUL00Codec *m_pEncoder;
	public:
		CPredictJob(CUL00Codec *pEncoder, DWORD nBandIndex)
		{
			m_nBandIndex = nBandIndex;
			m_pEncoder = pEncoder;
		}
		void JobProc(CThreadManager *)
		{
			m_pEncoder->PredictProc(m_nBandIndex);
		}
	};

	void EncodeProc(DWORD nBandIndex);
	class CEncodeJob : public CThreadJob
	{
	private:
		DWORD m_nBandIndex;
		CUL00Codec *m_pEncoder;
	public:
		CEncodeJob(CUL00Codec *pEncoder, DWORD nBandIndex)
		{
			m_nBandIndex = nBandIndex;
			m_pEncoder = pEncoder;
		}
		void JobProc(CThreadManager *)
		{
			m_pEncoder->EncodeProc(m_nBandIndex);
		}
	};

	void DecodeProc(DWORD nBandIndex);
	class CDecodeJob : public CThreadJob
	{
	private:
		DWORD m_nBandIndex;
		CUL00Codec *m_pDecoder;
	public:
		CDecodeJob(CUL00Codec *pDecoder, DWORD nBandIndex)
		{
			m_nBandIndex = nBandIndex;
			m_pDecoder = pDecoder;
		}
		void JobProc(CThreadManager *)
		{
			m_pDecoder->DecodeProc(m_nBandIndex);
		}
	};
};
