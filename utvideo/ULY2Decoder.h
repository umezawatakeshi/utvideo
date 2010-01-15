/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Decoder.h"
#include "PlanarDecoder.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULY2Decoder :
	public CPlanarDecoder
{
private:
	static const OUTPUTFORMAT m_outfmts[];

public:
	CULY2Decoder(void);
	virtual ~CULY2Decoder(void);
	static CDecoder *CreateInstance(void);

protected:
	virtual DWORD GetInputFCC(void) { return FCC('ULY2'); }
	virtual WORD GetInputBitCount(void) { return 16; }
	virtual const OUTPUTFORMAT *GetSupportedOutputFormats(void) { return m_outfmts; };
	virtual int GetNumSupportedOutputFormats(void);
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihOut);
	virtual void ConvertFromPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 1; }
};
