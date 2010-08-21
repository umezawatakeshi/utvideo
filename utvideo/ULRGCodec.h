/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULRGCodec :
	public CUL00Codec
{
private:
	static const FORMATINFO m_fiEncoderInput[];
	static const FORMATINFO m_fiDecoderOutput[];
	static const FORMATINFO m_fiCompressed[];

public:
	CULRGCodec(const char *pszInterfaceName);
	virtual ~CULRGCodec(void);
	static CCodec *CreateInstance(const char *pszInterfaceName);

public:
	virtual const FORMATINFO *GetEncoderInputFormat(void) { return m_fiEncoderInput; }
	virtual const FORMATINFO *GetDecoderOutputFormat(void) { return m_fiDecoderOutput; }
	virtual const FORMATINFO *GetCompressedFormat(void) { return m_fiCompressed; }

protected:
	virtual const char *GetColorFormatName(void) { return "RGB"; }
	virtual WORD GetRealBitCount(void) { return 24; }
	virtual WORD GetFalseBitCount(void) { return 24; }
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }

	virtual void ConvertFromPlanar(DWORD nBandIndex);
};
