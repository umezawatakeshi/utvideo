/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#define CBGROSSWIDTH_NATURAL ((size_t)0)
#define CBGROSSWIDTH_WINDOWS ((size_t)-1)

class CCodec
{
public:
	DLLEXPORT static CCodec *CreateInstance(utvf_t utvf, const char *pszInterfaceName);
	DLLEXPORT static void DeleteInstance(CCodec *pCodec);

protected:
	CCodec(void);
	virtual ~CCodec(void);

public:
	virtual const char *GetTinyName(void) = 0;
	virtual void GetShortFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName) = 0;
	virtual const utvf_t *GetEncoderInputFormat(void) = 0;
	virtual const utvf_t *GetDecoderOutputFormat(void) = 0;
	virtual const utvf_t *GetCompressedFormat(void) = 0;
	virtual bool IsTemporalCompressionSupported(void) = 0;

#ifdef _WIN32
	virtual INT_PTR About(HWND hwnd);
	virtual INT_PTR Configure(HWND hwnd) = 0;
#endif

	virtual size_t GetStateSize(void) = 0;
	virtual int GetState(void *pState, size_t cb) = 0;
	virtual int SetState(const void *pState, size_t cb) = 0;

	virtual int EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t* cbGrossWidth) = 0;
	virtual size_t EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput) = 0;
	virtual int EncodeEnd(void) = 0;
	virtual size_t EncodeGetExtraDataSize(void) = 0;
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height) = 0;
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height) = 0;
	virtual int EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height) = 0;

	virtual int DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth, const void *pExtraData, size_t cbExtraData) = 0;
	virtual size_t DecodeFrame(void *pOutput, const void *pInput) = 0;
	virtual int DecodeGetFrameType(bool *pbKeyFrame, const void *pInput) = 0;
	virtual int DecodeEnd(void) = 0;
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth) = 0;
	virtual int DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData) = 0;
};
