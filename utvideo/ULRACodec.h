/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

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
