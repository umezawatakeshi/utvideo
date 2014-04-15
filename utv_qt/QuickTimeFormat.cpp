/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "QuickTimeFormat.h"

static inline bool is_fourcc(uint32_t x)
{
	return (x >= '    ' /* four SP */ && x <= 0x7e7e7e7e);
}

DLLEXPORT int UtVideoFormatToQuickTimeFormat(OSType *pixelFormat, utvf_t utvf)
{
	switch (utvf)
	{
	case UTVF_NFCC_RGB_TD:
		*pixelFormat = k24RGBPixelFormat;
		return 0;
	case UTVF_NFCC_ARGB_TD:
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
		*utvf = UTVF_NFCC_RGB_TD;
		return 0;
	case k32ARGBPixelFormat:
		*utvf = UTVF_NFCC_ARGB_TD;
		return 0;
	}

	if (!is_fourcc(pixelFormat))
		return -1;

	*utvf = pixelFormat;

	return 0;
}
