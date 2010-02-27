/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "StdAfx.h"
#include "Codec.h"

CEncoder::CEncoder(void)
{
}

CEncoder::~CEncoder(void)
{
}

CDecoder::CDecoder(void)
{
}

CDecoder::~CDecoder(void)
{
}

CDummyEncoder::CDummyEncoder(void)
{
}

CDummyEncoder::~CDummyEncoder(void)
{
}

CEncoder *CDummyEncoder::CreateInstance(void)
{
	return new CDummyEncoder();
}

LRESULT CDummyEncoder::Configure(HWND hwnd)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::GetState(void *pState, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::SetState(const void *pState, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::Compress(const ICCOMPRESS *icc, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::CompressEnd(void)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyEncoder::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
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
