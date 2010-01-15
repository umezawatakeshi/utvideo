/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Encoder.h"
#include "PlanarEncoder.h"
#include "FrameBuffer.h"
#include "HuffmanCode.h"
#include "Thread.h"

class CULY2Encoder :
	public CPlanarEncoder
{
private:
	static const INPUTFORMAT m_infmts[];

public:
	CULY2Encoder(void);
	virtual ~CULY2Encoder(void);
	static CEncoder *CreateInstance(void);

protected:
	virtual DWORD GetOutputFCC(void) { return FCC('ULY2'); }
	virtual const char *GetColorFormatName(void) { return "YUV422"; }
	virtual WORD GetOutputBitCount(void) { return 16; }
	virtual WORD GetMaxBitCount(void) { return 24; }
	virtual const INPUTFORMAT *GetSupportedInputFormats(void) { return m_infmts; };
	virtual int GetNumSupportedInputFormats(void);
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 1; }
};
