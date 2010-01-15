/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

class CEncoder
{
public:
	CEncoder(void);
	virtual ~CEncoder(void);

public:
	virtual LRESULT Configure(HWND hwnd) = 0;
	virtual LRESULT GetState(void *pState, SIZE_T cb) = 0;
	virtual LRESULT SetState(const void *pState, SIZE_T cb) = 0;
	virtual LRESULT Compress(const ICCOMPRESS *icc, SIZE_T cb) = 0;
	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT CompressEnd(void) = 0;
	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
};

class CDummyEncoder :
	public CEncoder
{
public:
	CDummyEncoder(void);
	~CDummyEncoder(void);
	static CEncoder *CreateInstance(void);

public:
	virtual LRESULT Configure(HWND hwnd);
	virtual LRESULT GetState(void *pState, SIZE_T cb);
	virtual LRESULT SetState(const void *pState, SIZE_T cb);
	virtual LRESULT Compress(const ICCOMPRESS *icc, SIZE_T cb);
	virtual LRESULT CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressEnd(void);
	virtual LRESULT CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
