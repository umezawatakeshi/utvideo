/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULY0Codec :
	public CUL00Codec
{
private:
	static const INPUTFORMAT m_infmts[];
	static const OUTPUTFORMAT m_outfmts[];

public:
	CULY0Codec(void);
	virtual ~CULY0Codec(void);
	static CCodec *CreateInstance(void);

private:
	void ConvertBottomupRGBToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD bypp);
	void ConvertYUV422ToULY0(BYTE *pDstYBegin, BYTE *pDstUBegin, BYTE *pDstVBegin, DWORD nBandIndex, DWORD nYOffset);
	void ConvertULY0ToBottomupRGB(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD bypp);
	void ConvertULY0ToYUV422(const BYTE *pSrcYBegin, const BYTE *pSrcUBegin, const BYTE *pSrcVBegin, DWORD nBandIndex, DWORD nYOffset);

protected:
	virtual DWORD GetFCC(void) { return FCC('ULY0'); }
	virtual const char *GetColorFormatName(void) { return "YUV420"; }
	virtual WORD GetRealBitCount(void) { return 12; }
	virtual WORD GetFalseBitCount(void) { return 24; }
	virtual const INPUTFORMAT *GetSupportedInputFormats(void) { return m_infmts; }
	virtual int GetNumSupportedInputFormats(void);
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 2; }

	virtual const OUTPUTFORMAT *GetSupportedOutputFormats(void) { return m_outfmts; }
	virtual int GetNumSupportedOutputFormats(void);
	virtual void ConvertFromPlanar(DWORD nBandIndex);
};
