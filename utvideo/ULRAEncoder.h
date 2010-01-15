/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Encoder.h"
#include "PlanarEncoder.h"
#include "FrameBuffer.h"
#include "HuffmanCode.h"
#include "Thread.h"

class CULRAEncoder :
	public CPlanarEncoder
{
private:
	static const INPUTFORMAT m_infmts[];

public:
	CULRAEncoder(void);
	virtual ~CULRAEncoder(void);
	static CEncoder *CreateInstance(void);

protected:
	virtual DWORD GetOutputFCC(void) { return FCC('ULRA'); }
	virtual const char *GetColorFormatName(void) { return "RGBA"; }
	virtual WORD GetOutputBitCount(void) { return 32; }
	virtual WORD GetMaxBitCount(void) { return 32; }
	virtual const INPUTFORMAT *GetSupportedInputFormats(void) { return m_infmts; };
	virtual int GetNumSupportedInputFormats(void);
	virtual int GetNumPlanes(void) { return 4; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }
};
