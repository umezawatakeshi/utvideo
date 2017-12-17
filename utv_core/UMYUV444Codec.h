/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UM00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"
#include "Coefficient.h"

template<class C> class CUMYUV444CodecFormat;
template<> class CUMYUV444CodecFormat<CBT601Coefficient> { public: static const utvf_t m_utvf = UTVF_UMY4; };
template<> class CUMYUV444CodecFormat<CBT709Coefficient> { public: static const utvf_t m_utvf = UTVF_UMH4; };

template<class C>
class CUMYUV444Codec :
	public CUM00Codec
{
public:
	static const utvf_t m_utvfCodec = CUMYUV444CodecFormat<C>::m_utvf;

private:
	static const utvf_t m_utvfEncoderInput[];
	static const utvf_t m_utvfDecoderOutput[];
	static const utvf_t m_utvfCompressed[];

public:
	CUMYUV444Codec(const char *pszInterfaceName);
	virtual ~CUMYUV444Codec(void) {}
	static CCodec *CreateInstance(const char *pszInterfaceName) { return new CUMYUV444Codec(pszInterfaceName); }

public:
	virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
	virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
	virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
	virtual const char *GetColorFormatName(void);
	virtual int GetRealBitCount(void) { return 24; }
	virtual int GetNumPlanes(void) { return 3; }
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height);
	virtual void ConvertToPlanar(uint32_t nBandIndex);
	virtual void ConvertFromPlanar(uint32_t nBandIndex);
	virtual bool DecodeDirect(uint32_t nBandIndex);
};
