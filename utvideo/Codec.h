/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

class CCodec
{
public:
	CCodec(void);
	virtual ~CCodec(void);

public:
	virtual LRESULT Configure(HWND hwnd) = 0;
	virtual LRESULT GetStateSize(void) = 0;
	virtual LRESULT GetState(void *pState, SIZE_T cb) = 0;
	virtual LRESULT SetState(const void *pState, SIZE_T cb) = 0;
	virtual LRESULT Compress(const ICCOMPRESS *icc, SIZE_T cb) = 0;
	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT CompressEnd(void) = 0;
	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb) = 0;
	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT DecompressEnd(void) = 0;
	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
};

class CDummyCodec :
	public CCodec
{
public:
	CDummyCodec(void);
	~CDummyCodec(void);
	static CCodec *CreateInstance(void);

public:
	virtual LRESULT Configure(HWND hwnd);
	virtual LRESULT GetStateSize(void);
	virtual LRESULT GetState(void *pState, SIZE_T cb);
	virtual LRESULT SetState(const void *pState, SIZE_T cb);
	virtual LRESULT Compress(const ICCOMPRESS *icc, SIZE_T cb);
	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressEnd(void);
	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb);
	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressEnd(void);
	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
