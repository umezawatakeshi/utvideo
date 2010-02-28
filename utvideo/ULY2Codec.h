/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULY2Codec :
	public CUL00Codec
{
private:
	static const INPUTFORMAT m_infmts[];
	static const OUTPUTFORMAT m_outfmts[];

public:
	CULY2Codec(void);
	virtual ~CULY2Codec(void);
	static CCodec *CreateInstance(void);

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

	virtual DWORD GetInputFCC(void) { return FCC('ULY2'); }
	virtual WORD GetInputBitCount(void) { return 16; }
	virtual const OUTPUTFORMAT *GetSupportedOutputFormats(void) { return m_outfmts; };
	virtual int GetNumSupportedOutputFormats(void);
	virtual void ConvertFromPlanar(DWORD nBandIndex);
};
