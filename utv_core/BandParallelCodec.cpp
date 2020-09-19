/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "utv_core.h"
#include "BandParallelCodec.h"

CBandParallelCodec::CBandParallelCodec(const char *pszTinyName, const char *pszInterfaceName) : CCodecBase(pszTinyName, pszInterfaceName)
{
}

template<bool Encoding>
CBandParallelCodec::BAND_POSITION<Encoding> CBandParallelCodec::CalcBandPosition(uint32_t nBandIndex)
{
	BAND_POSITION<Encoding> result;

	auto pRaw = (BAND_POSITION<Encoding>::raw_t)(Encoding ? m_pInput : m_pOutput);
	for (int i = 0; i < m_fmRaw.nPlanes; ++i)
	{
		result.pRawBegin[i] = pRaw + m_fmRaw.cbFirstLineOffset[i] + m_dwStripeBegin[nBandIndex] * m_fmRaw.scbStripeStride[i];
		result.pRawEnd[i] = pRaw + m_fmRaw.cbFirstLineOffset[i] + m_dwStripeEnd[nBandIndex] * m_fmRaw.scbStripeStride[i];
	}

	return result;
}

template<bool Encoding>
CBandParallelCodec::BAND_POSITION<Encoding> CBandParallelCodec::CalcBandPosition(CFrameBuffer* pFrameBuffer, uint32_t nBandIndex)
{
	BAND_POSITION<Encoding> result = CalcBandPosition<Encoding>(nBandIndex);

	for (int i = 0; i < GetNumPlanes(); ++i)
	{
		result.pPlaneBegin[i] = pFrameBuffer->GetPlane(i) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[i];
	}

	return result;
}

template CBandParallelCodec::BAND_POSITION<true> CBandParallelCodec::CalcBandPosition<true>(CFrameBuffer* pFrameBuffer, uint32_t nBandIndex);
template CBandParallelCodec::BAND_POSITION<false> CBandParallelCodec::CalcBandPosition<false>(CFrameBuffer* pFrameBuffer, uint32_t nBandIndex);

template<bool Encoding>
CBandParallelCodec::PLANE_POSITION<Encoding> CBandParallelCodec::CalcPlanePosition()
{
	PLANE_POSITION<Encoding> result;

	auto pRaw = (BAND_POSITION<Encoding>::raw_t)(Encoding ? m_pInput : m_pOutput);
	for (int i = 0; i < m_fmRaw.nPlanes; ++i)
	{
		result.pRawBegin[i] = pRaw + m_fmRaw.cbPlaneOffset[i];
	}

	return result;
}

template CBandParallelCodec::PLANE_POSITION<true> CBandParallelCodec::CalcPlanePosition<true>();
template CBandParallelCodec::PLANE_POSITION<false> CBandParallelCodec::CalcPlanePosition<false>();

int CBandParallelCodec::CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	int ret;

	ret = CalcRawFrameMetric(rawfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return ret;

	CalcPlaneSizes(width, height);

	m_dwNumStripes = height / GetMacroPixelHeight();

	return 0;
}

void CBandParallelCodec::CalcBandMetric()
{
	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
	{
		m_dwStripeBegin[nBandIndex] = m_dwNumStripes *  nBandIndex      / m_dwDivideCount;
		m_dwStripeEnd[nBandIndex]   = m_dwNumStripes * (nBandIndex + 1) / m_dwDivideCount;
	}
}
