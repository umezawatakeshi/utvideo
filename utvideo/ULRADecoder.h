/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Decoder.h"
#include "PlanarDecoder.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULRADecoder :
	public CPlanarDecoder
{
private:
	static const OUTPUTFORMAT m_outfmts[];

public:
	CULRADecoder(void);
	virtual ~CULRADecoder(void);
	static CDecoder *CreateInstance(void);

protected:
	virtual DWORD GetInputFCC(void) { return FCC('ULRA'); }
	virtual WORD GetInputBitCount(void) { return 32; }
	virtual const OUTPUTFORMAT *GetSupportedOutputFormats(void) { return m_outfmts; };
	virtual int GetNumSupportedOutputFormats(void);
	virtual int GetNumPlanes(void) { return 4; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihOut);
	virtual void ConvertFromPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }
};
