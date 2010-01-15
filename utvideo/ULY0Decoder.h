/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Decoder.h"
#include "PlanarDecoder.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

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
