/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"
#include "Coefficient.h"

template<class C> class CULYUV422CodecFormat;
template<> class CULYUV422CodecFormat<CBT601Coefficient> { public: static const utvf_t m_utvf = UTVF_ULY2; };
template<> class CULYUV422CodecFormat<CBT709Coefficient> { public: static const utvf_t m_utvf = UTVF_ULH2; };

template<class C>
class CULYUV422Codec :
	public CUL00Codec
{
public:
	static const utvf_t m_utvfCodec = CULYUV422CodecFormat<C>::m_utvf;

private:
	static const utvf_t m_utvfEncoderInput[];
	static const utvf_t m_utvfDecoderOutput[];
	static const utvf_t m_utvfCompressed[];

public:
	CULYUV422Codec(const char *pszInterfaceName);
	virtual ~CULYUV422Codec(void) {}
	static CCodec *CreateInstance(const char *pszInterfaceName) { return new CULYUV422Codec(pszInterfaceName); }

public:
	virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
	virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
	virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
	virtual const char *GetColorFormatName(void);
	virtual int GetRealBitCount(void) { return 16; }
	virtual int GetNumPlanes(void) { return 3; }
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 1; }

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height);
	virtual void ConvertToPlanar(uint32_t nBandIndex);
	virtual void ConvertFromPlanar(uint32_t nBandIndex);
	virtual bool DecodeDirect(uint32_t nBandIndex);
};
