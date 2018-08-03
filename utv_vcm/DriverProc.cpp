/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "VCMCodec.h"

LRESULT CALLBACK DriverProc(DWORD_PTR dwDriverId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	CVCMCodec *pCodec = (CVCMCodec *)dwDriverId;

#ifdef _DEBUG
	if (uMsg != ICM_COMPRESS && uMsg != ICM_DECOMPRESS)
		DBGPRINTF("DriverProc() dwDriverId=%IX hdrvr=%p uMsg=%08X lParam1=%IX lParam2=%IX", dwDriverId, hdrvr, uMsg, lParam1, lParam2);
#endif

	switch (uMsg)
	{
	/* Driver Messages */
	case DRV_LOAD:
		return TRUE;

	case DRV_FREE:
		return TRUE;

	case DRV_OPEN:
		return (LRESULT)CVCMCodec::Open((ICOPEN *)lParam2);

	case DRV_CLOSE:
		if (pCodec != NULL)
			delete pCodec;
		return TRUE;

	/* Codec Messages */
	case ICM_ABOUT:
		if (lParam1 == -1)
			return pCodec->QueryAbout();
		else
			return pCodec->About((HWND)lParam1);

	case ICM_GETINFO:
		return pCodec->GetInfo((ICINFO *)lParam1, lParam2);

	/* Encoder Messages */
	case ICM_CONFIGURE:
		if (lParam1 == -1)
			return pCodec->QueryConfigure();
		else
			return pCodec->Configure((HWND)lParam1);

	case ICM_GETSTATE:
		if (lParam1 == 0)
			return pCodec->GetStateSize();
		else
			return pCodec->GetState((void *)lParam1, lParam2);

	case ICM_SETSTATE:
		return pCodec->SetState((void *)lParam1, lParam2);

	case ICM_COMPRESS:
		return pCodec->Compress((ICCOMPRESS *)lParam1, lParam2);

	case ICM_COMPRESS_BEGIN:
		return pCodec->CompressBegin((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_COMPRESS_END:
		return pCodec->CompressEnd();

	case ICM_COMPRESS_GET_FORMAT:
		return pCodec->CompressGetFormat((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_COMPRESS_GET_SIZE:
		return pCodec->CompressGetSize((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_COMPRESS_QUERY:
		return pCodec->CompressQuery((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	/* Decoder Messages */
	case ICM_DECOMPRESS:
		return pCodec->Decompress((ICDECOMPRESS *)lParam1, lParam2);

	case ICM_DECOMPRESS_BEGIN:
		return pCodec->DecompressBegin((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_DECOMPRESS_END:
		return pCodec->DecompressEnd();

	case ICM_DECOMPRESS_GET_FORMAT:
		return pCodec->DecompressGetFormat((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);

	case ICM_DECOMPRESS_QUERY:
		return pCodec->DecompressQuery((BITMAPINFOHEADER *)lParam1, (BITMAPINFOHEADER *)lParam2);
	}

	if (uMsg < DRV_USER)
		return DefDriverProc(dwDriverId, hdrvr, uMsg, lParam1, lParam2);
	else
		return ICERR_UNSUPPORTED;
}
