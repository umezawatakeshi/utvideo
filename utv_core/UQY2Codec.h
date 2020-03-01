/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UQ00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CUQY2Codec :
	public CUQ00Codec
{
public:
	static const utvf_t m_utvfCodec = UTVF_UQY2;

private:
	static const utvf_t m_utvfEncoderInput[];
	static const utvf_t m_utvfDecoderOutput[];
	static const utvf_t m_utvfCompressed[];

protected:

public:
	CUQY2Codec(const char *pszInterfaceName);
	virtual ~CUQY2Codec(void) {}
	static CCodec *CreateInstance(const char *pszInterfaceName) { return new CUQY2Codec(pszInterfaceName); }

public:
	virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
	virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
	virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
	virtual const char *GetColorFormatName(void) { return "YUV422"; }
	virtual int GetRealBitCount(void) { return 20; }
	virtual int GetNumPlanes(void) { return 3; }
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 1; }

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height);
	virtual void ConvertToPlanar(uint32_t nBandIndex);
	virtual void ConvertFromPlanar(uint32_t nBandIndex);
	virtual bool PredictDirect(uint32_t nBandIndex);
	virtual void GenerateDecodeTable(uint32_t nPlaneIndex);
	virtual bool DecodeDirect(uint32_t nBandIndex);
	virtual void RestoreCustom(uint32_t nBandIndex, int nPlaneIndex, uint8_t* const* pDstBegin, const uint16_t* pSrcBegin, const uint16_t* pSrcEnd);
};
