/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "DummyCodec.h"

CDummyCodec::CDummyCodec(void)
{
}

CDummyCodec::~CDummyCodec(void)
{
}

CCodec *CDummyCodec::CreateInstance(const char *pszInterfaceName)
{
	return new CDummyCodec();
}

const char *CDummyCodec::GetTinyName(void)
{
	return "Dummy";
}

void CDummyCodec::GetShortFriendlyName(char *pszName, size_t cchName)
{
	pszName[0] = '\0';
}

void CDummyCodec::GetShortFriendlyName(wchar_t *pszName, size_t cchName)
{
	pszName[0] = L'\0';
}

void CDummyCodec::GetLongFriendlyName(char *pszName, size_t cchName)
{
	pszName[0] = '\0';
}

void CDummyCodec::GetLongFriendlyName(wchar_t *pszName, size_t cchName)
{
	pszName[0] = L'\0';
}

const utvf_t *CDummyCodec::GetEncoderInputFormat(void)
{
	return NULL;
}

const utvf_t *CDummyCodec::GetDecoderOutputFormat(void)
{
	return NULL;
}

const utvf_t *CDummyCodec::GetCompressedFormat(void)
{
	return NULL;
}

#ifdef _WIN32
INT_PTR CDummyCodec::Configure(HWND hwnd)
{
	return 0;
}
#endif

size_t CDummyCodec::GetStateSize(void)
{
	return 0;
}

int CDummyCodec::GetState(void *pState, size_t cb)
{
	return 0;
}

int CDummyCodec::SetState(const void *pState, size_t cb)
{
	return 0;
}


int CDummyCodec::EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	return -1;
}

size_t CDummyCodec::EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput)
{
	return 0;
}

int CDummyCodec::EncodeEnd(void)
{
	return 0;
}

size_t CDummyCodec::EncodeGetExtraDataSize(void)
{
	return 0;
}

int CDummyCodec::EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height)
{
	return -1;
}

size_t CDummyCodec::EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	return 0;
}

int CDummyCodec::EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height)
{
	return -1;
}


int CDummyCodec::DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	return -1;
}

size_t CDummyCodec::DecodeFrame(void *pOutput, const void *pInput)
{
	return 0;
}

int CDummyCodec::DecodeGetFrameType(bool *pbKeyFrame, const void *pInput)
{
	*pbKeyFrame = true;
	return 0;
}

int CDummyCodec::DecodeEnd(void)
{
	return 0;
}

size_t CDummyCodec::DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	return 0;
}

int CDummyCodec::DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData)
{
	return -1;
}
