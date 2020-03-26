/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"

class CDummyCodec :
	public CCodec
{
public:
	static const utvf_t m_utvfCodec = UTVF_INVALID;

public:
	CDummyCodec(void);
	~CDummyCodec(void);
	static CCodec *CreateInstance(const char *pszInterfaceName);

public:
	virtual const char *GetTinyName(void);
	virtual void GetShortFriendlyName(char *pszName, size_t cchName);
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName);
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
	virtual const utvf_t *GetEncoderInputFormat(void);
	virtual const utvf_t *GetDecoderOutputFormat(void);
	virtual const utvf_t *GetCompressedFormat(void);
	virtual bool IsTemporalCompressionSupported(void) { return false; }

#ifdef _WIN32
	virtual INT_PTR Configure(HWND hwnd);
#endif

	virtual size_t GetStateSize(void);
	virtual int GetState(void *pState, size_t cb);
	virtual int SetState(const void *pState, size_t cb);

	virtual int EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
	virtual size_t EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput);
	virtual int EncodeEnd(void);
	virtual size_t EncodeGetExtraDataSize(void);
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height);
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height);
	virtual int EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height);

	virtual int DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual size_t DecodeFrame(void *pOutput, const void *pInput);
	virtual int DecodeGetFrameType(bool *pbKeyFrame, const void *pInput);
	virtual int DecodeEnd(void);
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
	virtual int DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData);
};
