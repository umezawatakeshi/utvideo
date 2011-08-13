/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULRACodec :
	public CUL00Codec
{
private:
	static const FORMATINFO m_fiEncoderInput[];
	static const FORMATINFO m_fiDecoderOutput[];
	static const FORMATINFO m_fiCompressed[];

public:
	CULRACodec(const char *pszInterfaceName);
	virtual ~CULRACodec(void);
	static CCodec *CreateInstance(const char *pszInterfaceName);

public:
	virtual const FORMATINFO *GetEncoderInputFormat(void) { return m_fiEncoderInput; }
	virtual const FORMATINFO *GetDecoderOutputFormat(void) { return m_fiDecoderOutput; }
	virtual const FORMATINFO *GetCompressedFormat(void) { return m_fiCompressed; }

protected:
	virtual const char *GetColorFormatName(void) { return "RGBA"; }
	virtual WORD GetRealBitCount(void) { return 32; }
	virtual WORD GetFalseBitCount(void) { return 32; }
	virtual int GetNumPlanes(void) { return 4; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }

	virtual void ConvertFromPlanar(DWORD nBandIndex);
	BOOL DecodeDirect(DWORD nBandIndex);
};
