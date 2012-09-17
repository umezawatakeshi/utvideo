/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Format.h"

inline bool is_fourcc(uint32_t x)
{
	return (x >= '    ' /* four SP */ && x <= 0x7e7e7e7e);
}

int UtVideoFormatToWindowsFormat(DWORD *biCompression, WORD *biBitCount, utvf_t utvf)
{
	switch (utvf)
	{
	case UTVF_RGB24_WIN:
		*biCompression = BI_RGB;
		*biBitCount    = 24;
		return 0;
	case UTVF_RGB32_WIN:
		*biCompression = BI_RGB;
		*biBitCount    = 32;
		return 0;
	case UTVF_ARGB32_WIN:
		*biCompression = BI_RGB;
		*biBitCount    = 32;
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

	return 0;
}

int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount)
{
	DWORD dwtmp;
	WORD wtmp;

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

	if (UtVideoFormatToWindowsFormat(&dwtmp, &wtmp, (utvf_t)UNFCC(biCompression)) != 0)
		return -1;

	*utvf = UNFCC(biCompression);
	return 0;
}
