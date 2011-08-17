/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#define CBGROSSWIDTH_NATURAL ((size_t)0)
#define CBGROSSWIDTH_WINDOWS ((size_t)-1)

class CCodec
{
public:
	__declspec(dllexport) static CCodec *CreateInstance(DWORD fcc, const char *pszInterfaceName);
	__declspec(dllexport) static void DeleteInstance(CCodec *pCodec);

protected:
	CCodec(void);
	virtual ~CCodec(void);

public:
	virtual void GetShortFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName) = 0;
	virtual const utvf_t *GetEncoderInputFormat(void) = 0;
	virtual const utvf_t *GetDecoderOutputFormat(void) = 0;
	virtual const utvf_t *GetCompressedFormat(void) = 0;

#ifdef _WIN32
	virtual INT_PTR About(HWND hwnd);
	virtual INT_PTR Configure(HWND hwnd) = 0;
#endif

	virtual size_t GetStateSize(void) = 0;
	virtual int GetState(void *pState, size_t cb) = 0;
	virtual int SetState(const void *pState, size_t cb) = 0;

	virtual int EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth) = 0;
	virtual size_t EncodeFrame(void *pOutput, const void *pInput) = 0;
	virtual int EncodeEnd(void) = 0;
	virtual size_t EncodeGetExtraDataSize(void) = 0;
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb) = 0;
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth) = 0;
	virtual int EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth) = 0;

//	virtual LRESULT Compress(const ICCOMPRESS *icc, size_t cb) = 0;
//	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
//	virtual LRESULT CompressEnd(void) = 0;
//	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut) = 0;
//	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
//	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;

	virtual int DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData) = 0;
	virtual size_t DecodeFrame(void *pOutput, const void *pInput) = 0;
	virtual int DecodeEnd(void) = 0;
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData) = 0;
	virtual int DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData) = 0;
	// DecodeGetFormat ÇÕïsóv

//	virtual LRESULT Decompress(const ICDECOMPRESS *icd, size_t cb) = 0;
//	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
//	virtual LRESULT DecompressEnd(void) = 0;
//	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut, const FORMATINFO *pfiOut = NULL) = 0;
//	virtual LRESULT DecompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
//	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
};
