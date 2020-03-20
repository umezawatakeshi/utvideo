/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "CodecBase.h"
#include "BandParallelCodec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"
#include "Predict.h"


class CBandParallelCodec :
	public CCodecBase
{
protected:
	const void *m_pInput;
	void *m_pOutput;
	uint32_t m_dwNumStripes;
	uint32_t m_dwDivideCount;
	size_t m_cbPlaneStripeSize[4];

	uint32_t m_dwStripeBegin[256];
	uint32_t m_dwStripeEnd[256];

protected:
	CBandParallelCodec(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CBandParallelCodec(void) = default;

protected:
	int CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
	void CalcBandMetric();

	virtual int GetNumPlanes(void) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

	template<bool Encoding> struct BAND_POSITION
	{
		using raw_t   = std::conditional_t<Encoding, const uint8_t*, uint8_t*>;
		using plane_t = std::conditional_t<Encoding, uint8_t*, const uint8_t*>;

		raw_t pRawBegin[4];
		raw_t pRawEnd[4];
		plane_t pPlaneBegin[4];
	};
	template<bool Encoding> BAND_POSITION<Encoding> CalcBandPosition(uint32_t nBandIndex);
	template<bool Encoding> BAND_POSITION<Encoding> CalcBandPosition(CFrameBuffer* pFrameBuffer, uint32_t nBandIndex);

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height) = 0;
};
