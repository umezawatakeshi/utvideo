/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Format.h"

static inline bool is_fourcc(uint32_t x)
{
	return (x >= '    ' /* four SP */ && x <= 0x7e7e7e7e);
}

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

	if (!is_fourcc(pixelFormat))
		return -1;

	*utvf = pixelFormat;

	return 0;
}
