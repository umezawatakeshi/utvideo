/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"

class CDummyCodec :
	public CCodec
{
public:
	CDummyCodec(void);
	~CDummyCodec(void);
	static CCodec *CreateInstance(const char *pszInterfaceName);

public:
	virtual void GetShortFriendlyName(char *pszName, size_t cchName);
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName);
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
	virtual const utvf_t *GetEncoderInputFormat(void);
	virtual const utvf_t *GetDecoderOutputFormat(void);
	virtual const utvf_t *GetCompressedFormat(void);

#ifdef _WIN32
	virtual INT_PTR Configure(HWND hwnd);
#endif

	virtual size_t GetStateSize(void);
	virtual int GetState(void *pState, size_t cb);
	virtual int SetState(const void *pState, size_t cb);

	virtual int EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual size_t EncodeFrame(void *pOutput, const void *pInput);
	virtual int EncodeEnd(void);
	virtual size_t EncodeGetExtraDataSize(void);
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb);
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual int EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);

//	virtual LRESULT Compress(const ICCOMPRESS *icc, size_t cb);
//	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
//	virtual LRESULT CompressEnd(void);
//	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
//	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
//	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

	virtual int DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual size_t DecodeFrame(void *pOutput, const void *pInput);
	virtual int DecodeEnd(void);
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual int DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);

//	virtual LRESULT Decompress(const ICDECOMPRESS *icd, size_t cb);
//	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
//	virtual LRESULT DecompressEnd(void);
//	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut, const FORMATINFO *pfiOut);
//	virtual LRESULT DecompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
//	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
