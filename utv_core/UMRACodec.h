/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UM00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CUMRACodec :
	public CUM00Codec
{
public:
	static const utvf_t m_utvfCodec = UTVF_UMRA;

private:
	static const utvf_t m_utvfEncoderInput[];
	static const utvf_t m_utvfDecoderOutput[];
	static const utvf_t m_utvfCompressed[];

public:
	CUMRACodec(const char *pszInterfaceName);
	virtual ~CUMRACodec(void) {}
	static CCodec *CreateInstance(const char *pszInterfaceName) { return new CUMRACodec(pszInterfaceName); }

public:
	virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
	virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
	virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
	virtual const char *GetColorFormatName(void) { return "RGBA"; }
	virtual int GetRealBitCount(void) { return 32; }
	virtual int GetNumPlanes(void) { return 4; }
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height);
	virtual void ConvertToPlanar(uint32_t nBandIndex);
	virtual void ConvertFromPlanar(uint32_t nBandIndex);
	virtual bool EncodeDirect(uint32_t nBandIndex);
	virtual bool DecodeDirect(uint32_t nBandIndex);
};
