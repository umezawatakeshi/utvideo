/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "StdAfx.h"
#include "Codec.h"

CCodec::CCodec(void)
{
}

CCodec::~CCodec(void)
{
}

CDummyCodec::CDummyCodec(void)
{
}

CDummyCodec::~CDummyCodec(void)
{
}

CCodec *CDummyCodec::CreateInstance(void)
{
	return new CDummyCodec();
}

LRESULT CDummyCodec::Configure(HWND hwnd)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::GetState(void *pState, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::SetState(const void *pState, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::Compress(const ICCOMPRESS *icc, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressEnd(void)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressEnd(void)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}

LRESULT CDummyCodec::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ICERR_UNSUPPORTED;
}
