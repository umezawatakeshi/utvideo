/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Encoder.h"
#include "Decoder.h"

class CVCMCodec
{
private:
	struct CODECLIST
	{
		DWORD fcc;
		const char *pszColorFormatName;
		CEncoder *(*pfnCreateEncoder)(void);
		CDecoder *(*pfnCreateDecoder)(void);
	};

	static const CODECLIST m_codeclist[];

public:
	static void ICInstallAll(void);
	static void ICRemoveAll(void);

private:
	DWORD m_fccHandler;
	const char *m_pszColorFormatName;
	CEncoder *m_pEncoder;
	CDecoder *m_pDecoder;

private:
	CVCMCodec(DWORD fccHandler);
public:
	~CVCMCodec(void);

public:
	static CVCMCodec *Open(ICOPEN *icopen);

	LRESULT QueryAbout(void);
	LRESULT About(HWND hwnd);
	LRESULT GetInfo(ICINFO *icinfo, SIZE_T cb);

	LRESULT QueryConfigure(void);
	LRESULT Configure(HWND hwnd);
	LRESULT GetStateSize(void);
	LRESULT GetState(void *pState, SIZE_T cb);
	LRESULT SetState(const void *pState, SIZE_T cb);
	LRESULT Compress(const ICCOMPRESS *icc, SIZE_T cb);
	LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	LRESULT CompressEnd(void);
	LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

	LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb);
	LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	LRESULT DecompressEnd(void);
	LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
