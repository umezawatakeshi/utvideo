/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

class CDecoder
{
public:
	CDecoder(void);
	virtual ~CDecoder(void);

public:
	virtual LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb) = 0;
	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT DecompressEnd(void) = 0;
	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut) = 0;
	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut) = 0;
};

class CDummyDecoder :
	public CDecoder
{
public:
	CDummyDecoder(void);
	~CDummyDecoder(void);
	static CDecoder *CreateInstance(void);

public:
	virtual LRESULT Decompress(const ICDECOMPRESS *icd, SIZE_T cb);
	virtual LRESULT DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressEnd(void);
	virtual LRESULT DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual LRESULT DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
};
