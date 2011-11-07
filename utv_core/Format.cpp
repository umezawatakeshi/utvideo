/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Format.h"

inline bool is_fourcc(uint32_t x)
{
	return (x >= '    ' /* four SP */ && x <= 0x7e7e7e7e);
}

#ifdef _WIN32

DLLEXPORT int UtVideoFormatToWindowsFormat(DWORD *biCompression, WORD *biBitCount, GUID *subtype, utvf_t utvf)
{
	DWORD dwtmp;
	WORD wtmp;
	GUID guidtmp;

	if (biCompression == NULL)
		biCompression = &dwtmp;
	if (biBitCount == NULL)
		biBitCount = &wtmp;
	if (subtype == NULL)
		subtype = &guidtmp;

	switch (utvf)
	{
	case UTVF_RGB24_WIN:
		*biCompression = BI_RGB;
		*biBitCount    = 24;
		*subtype       = MEDIASUBTYPE_RGB24;
		return 0;
	case UTVF_RGB32_WIN:
		*biCompression = BI_RGB;
		*biBitCount    = 32;
		*subtype       = MEDIASUBTYPE_RGB32;
		return 0;
	case UTVF_ARGB32_WIN:
		*biCompression = BI_RGB;
		*biBitCount    = 32;
		*subtype       = MEDIASUBTYPE_ARGB32;
		return 0;
	}

	if (!is_fourcc(utvf))
		return -1;

	switch (utvf)
	{
	case UTVF_ULRA:
		*biBitCount = 32;
		break;
	case UTVF_ULRG:
	case UTVF_ULY2:
	case UTVF_ULY0:
		*biBitCount = 24;
		break;

	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_UYVY:
	case UTVF_UYNV:
		*biBitCount = 16;
		break;
	case UTVF_YV12:
		*biBitCount = 12;
		break;

	default:
		return -1;
	}

	*biCompression = FCC(utvf);
	*subtype = MEDIASUBTYPE_YUY2;
	subtype->Data1 = FCC(utvf);

	return 0;
}

DLLEXPORT int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype)
{
	utvf_t bybi, byguid;

	if (WindowsFormatToUtVideoFormat(&bybi, biCompression, biBitCount) != 0)
		return -1;
	if (WindowsFormatToUtVideoFormat(&byguid, subtype) != 0)
		return -1;

	if (bybi == byguid || (bybi == UTVF_RGB32_WIN && byguid == UTVF_ARGB32_WIN))
	{
		*utvf = byguid;
		return 0;
	}
	else
		return -1;
}

DLLEXPORT int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount)
{
	DWORD dwtmp;

	if (biCompression == BI_RGB)
	{
		switch (biBitCount)
		{
		case 24:
			*utvf = UTVF_RGB24_WIN;
			return 0;
		case 32:
			*utvf = UTVF_RGB32_WIN;
			return 0;
		default:
			return -1;
		}
	}

	if (UtVideoFormatToWindowsFormat(&dwtmp, NULL, NULL, (utvf_t)UNFCC(biCompression)) != 0)
		return -1;

	*utvf = UNFCC(biCompression);
	return 0;
}

DLLEXPORT int WindowsFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype)
{
	GUID guidtmp;
	utvf_t utvftmp;

	guidtmp = subtype;
	guidtmp.Data1 = MEDIASUBTYPE_YUY2.Data1;
	if (IsEqualGUID(guidtmp, MEDIASUBTYPE_YUY2))
		utvftmp = UNFCC(subtype.Data1);
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_RGB24))
		utvftmp = UTVF_RGB24_WIN;
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_RGB32))
		utvftmp = UTVF_RGB32_WIN;
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_ARGB32))
		utvftmp = UTVF_ARGB32_WIN;
	else
		return -1;

	if (UtVideoFormatToWindowsFormat(NULL, NULL, NULL, utvftmp) != 0)
		return -1;

	*utvf = utvftmp;
	return 0;
}

#endif

#ifdef __QUICKTIME__

DLLEXPORT int UtVideoFormatToQuickTimeFormat(OSType *pixelFormat, utvf_t utvf)
{
	switch (utvf)
	{
	case UTVF_RGB24_QT:
		*pixelFormat = k24RGBPixelFormat;
		return 0;
	case UTVF_ARGB32_QT:
		*pixelFormat = k32ARGBPixelFormat;
		return 0;
	}

	if (!is_fourcc(utvf))
		return -1;

	*pixelFormat = utvf;

	return 0;
}

DLLEXPORT int QuickTimeFormatToUtVideoFormat(utvf_t *utvf, OSType pixelFormat)
{
	OSType ostypetmp;

	switch (pixelFormat)
	{
	case k24RGBPixelFormat:
		*utvf = UTVF_RGB24_QT;
		return 0;
	case k32ARGBPixelFormat:
		*utvf = UTVF_ARGB32_QT;
		return 0;
	}

	if (UtVideoFormatToQuickTimeFormat(&ostypetmp, pixelFormat) != 0)
		return -1;

	*utvf = pixelFormat;
	return 0;
}

#endif
