/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Encoder.h"
#include "PlanarEncoder.h"
#include "FrameBuffer.h"
#include "HuffmanCode.h"
#include "Thread.h"

class CULRGEncoder :
	public CPlanarEncoder
{
private:
	static const INPUTFORMAT m_infmts[];

public:
	CULRGEncoder(void);
	virtual ~CULRGEncoder(void);
	static CEncoder *CreateInstance(void);

protected:
	virtual DWORD GetOutputFCC(void) { return FCC('ULRG'); }
	virtual const char *GetColorFormatName(void) { return "RGB"; }
	virtual WORD GetOutputBitCount(void) { return 24; }
	virtual WORD GetMaxBitCount(void) { return 24; }
	virtual const INPUTFORMAT *GetSupportedInputFormats(void) { return m_infmts; };
	virtual int GetNumSupportedInputFormats(void);
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }
};
