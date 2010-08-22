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
	DWORD const m_fcc;
	const char *const m_pszInterfaceName;

	ENCODERCONF m_ec;
	BOOL m_bBottomUpFrame;
	DWORD m_dwNumStripes;
	DWORD m_dwDivideCount;
	BOOL m_bInterlace;
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
	const ICCOMPRESS *m_icc;
	CFrameBuffer *m_pCurFrame;
	CFrameBuffer *m_pMedianPredicted;
	struct COUNTS
	{
		DWORD dwCount[4][256];
	} *m_counts;
	/* const */ BYTE *m_pCodeLengthTable[4];
	HUFFMAN_ENCODE_TABLE m_het[4];

	CFrameBuffer *m_pRestoredFrame;
	CFrameBuffer *m_pDecodedFrame;
	FRAMEINFO m_fi;
	HUFFMAN_DECODE_TABLE m_hdt[4];
	const ICDECOMPRESS *m_icd;

public:
	CUL00Codec(DWORD fcc, const char *pszInterfaceName);
	virtual ~CUL00Codec(void);

public:
	virtual void GetShortFriendlyName(char *pszName, size_t cchName);
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName);
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
	virtual DWORD GetFCC(void);
	virtual BOOL IsTemporalCompressionSupported(void);
	virtual const FORMATINFO *GetEncoderInputFormat(void) = 0;
	virtual const FORMATINFO *GetDecoderOutputFormat(void) = 0;
	virtual const FORMATINFO *GetCompressedFormat(void) = 0;

	virtual LRESULT Configure(HWND hwnd);
	static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT GetStateSize(void);
	virtual LRESULT GetState(void *pState, SIZE_T cb);
	virtual LRESULT SetState(const void *pState, SIZE_T cb);
	virtual LRESULT Compress(const ICCOMPRESS *icc, SIZE_T cb);
	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressEnd(void);
	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb);
	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressEnd(void);
	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

protected:
	LRESULT LoadConfig(void);
	LRESULT SaveConfig(void);
	LRESULT InternalSetState(const void *pState, SIZE_T cb);
	virtual const char *GetColorFormatName(void) = 0;
	virtual WORD GetRealBitCount(void) = 0;
	virtual WORD GetFalseBitCount(void) = 0;
	virtual int GetNumPlanes(void) = 0;
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn) = 0;
	virtual void ConvertToPlanar(DWORD nBandIndex) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

	virtual void ConvertFromPlanar(DWORD nBandIndex) = 0;

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
