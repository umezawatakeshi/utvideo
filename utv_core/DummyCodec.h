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
	virtual DWORD GetFCC(void);
	virtual const FORMATINFO *GetEncoderInputFormat(void);
	virtual const FORMATINFO *GetDecoderOutputFormat(void);
	virtual const FORMATINFO *GetCompressedFormat(void);

	virtual LRESULT Configure(HWND hwnd);
	virtual LRESULT GetStateSize(void);
	virtual LRESULT GetState(void *pState, size_t cb);
	virtual LRESULT SetState(const void *pState, size_t cb);
	virtual LRESULT Compress(const ICCOMPRESS *icc, size_t cb);
	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressEnd(void);
	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT Decompress(const ICDECOMPRESS *icd, size_t cb);
	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressEnd(void);
	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut, const FORMATINFO *pfiOut);
	virtual LRESULT DecompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
