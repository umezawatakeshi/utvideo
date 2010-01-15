/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once
#include "Decoder.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CPlanarDecoder :
	public CDecoder
{
protected:
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
	CFrameBuffer *m_pCurFrame;
	CFrameBuffer *m_pRestoredFrame;
	CFrameBuffer *m_pDecodedFrame;
	FRAMEINFO m_fi;
	HUFFMAN_DECODE_TABLE m_hdt[4];
	const BYTE *m_pCodeLengthTable[4];
	const ICDECOMPRESS *m_icd;

public:
	struct OUTPUTFORMAT
	{
		DWORD fcc;
		WORD nBitCount;
	};

public:
	CPlanarDecoder(void);
	virtual ~CPlanarDecoder(void);

public:
	virtual LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb);
	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressEnd(void);
	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

protected:
	virtual DWORD GetInputFCC(void) = 0;
	virtual WORD GetInputBitCount(void) = 0;
	virtual const OUTPUTFORMAT *GetSupportedOutputFormats(void) = 0;
	virtual int GetNumSupportedOutputFormats(void) = 0;
	virtual int GetNumPlanes(void) = 0;
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn) = 0;
	virtual void ConvertFromPlanar(DWORD nBandIndex) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

private:
	void DecodeProc(DWORD nBandIndex);
	class CDecodeJob : public CThreadJob
	{
	private:
		DWORD m_nBandIndex;
		CPlanarDecoder *m_pDecoder;
	public:
		CDecodeJob(CPlanarDecoder *pDecoder, DWORD nBandIndex)
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
