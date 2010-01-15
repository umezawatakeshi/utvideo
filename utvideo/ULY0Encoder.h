/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Encoder.h"
#include "PlanarEncoder.h"
#include "FrameBuffer.h"
#include "HuffmanCode.h"
#include "Thread.h"

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
