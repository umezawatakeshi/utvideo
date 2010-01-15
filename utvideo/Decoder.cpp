/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "StdAfx.h"
#include "Decoder.h"

CDecoder::CDecoder(void)
{
}

CDecoder::~CDecoder(void)
{
}

CDummyDecoder::CDummyDecoder(void)
{
}

CDummyDecoder::~CDummyDecoder(void)
{
}

CDecoder *CDummyDecoder::CreateInstance(void)
{
	return new CDummyDecoder();
}

LRESULT CDummyDecoder::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyDecoder::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyDecoder::DecompressEnd(void)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyDecoder::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyDecoder::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}
