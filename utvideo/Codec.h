/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

struct FORMATINFO
{
	DWORD fcc;
	WORD nBitCount;
	REFGUID guidMediaSubType;
};

#define FORMATINFO_END { DWORD(-1), 0, GUID_NULL }
#define IS_FORMATINFO_END(pfi) ((pfi)->fcc == DWORD(-1))

class CCodec
{
public:
	CCodec(void);
	virtual ~CCodec(void);
	__declspec(dllexport) static CCodec *CreateInstance(DWORD fcc, const char *pszInterfaceName);

public:
	virtual void GetShortFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName) = 0;
	virtual DWORD GetFCC(void) = 0;
	virtual const FORMATINFO *GetEncoderInputFormat(void) = 0;
	virtual const FORMATINFO *GetDecoderOutputFormat(void) = 0;
	virtual const FORMATINFO *GetCompressedFormat(void) = 0;

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
