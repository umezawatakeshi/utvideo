/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULY0Encoder :
	public CPlanarEncoder
{
private:
	static const INPUTFORMAT m_infmts[];

public:
	CULY0Encoder(void);
	virtual ~CULY0Encoder(void);
	static CEncoder *CreateInstance(void);

private:
	void ConvertBottomupRGBToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD bypp);
	void ConvertYUV422ToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD nYOffset);

protected:
	virtual DWORD GetOutputFCC(void) { return FCC('ULY0'); }
	virtual const char *GetColorFormatName(void) { return "YUV420"; }
	virtual WORD GetOutputBitCount(void) { return 12; }
	virtual WORD GetMaxBitCount(void) { return 24; }
	virtual const INPUTFORMAT *GetSupportedInputFormats(void) { return m_infmts; }
	virtual int GetNumSupportedInputFormats(void);
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 2; }
};

class CULY0Decoder :
	public CPlanarDecoder
{
private:
	static const OUTPUTFORMAT m_outfmts[];

public:
	CULY0Decoder(void);
	virtual ~CULY0Decoder(void);
	static CDecoder *CreateInstance(void);

private:
	void ConvertULY0ToBottomupRGB(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD bypp);
	void ConvertULY0ToYUV422(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD nYOffset);

protected:
	virtual DWORD GetInputFCC(void) { return FCC('ULY0'); }
	virtual WORD GetInputBitCount(void) { return 12; }
	virtual const OUTPUTFORMAT *GetSupportedOutputFormats(void) { return m_outfmts; }
	virtual int GetNumSupportedOutputFormats(void);
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertFromPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 2; }
};
