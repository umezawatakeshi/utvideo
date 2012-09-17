/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Format.h"

inline bool is_fourcc(uint32_t x)
{
	return (x >= '    ' /* four SP */ && x <= 0x7e7e7e7e);
}

int UtVideoFormatToWindowsFormat(GUID *subtype, utvf_t utvf)
{
	switch (utvf)
	{
	case UTVF_RGB24_WIN:
		*subtype       = MEDIASUBTYPE_RGB24;
		return 0;
	case UTVF_RGB32_WIN:
		*subtype       = MEDIASUBTYPE_RGB32;
		return 0;
	case UTVF_ARGB32_WIN:
		*subtype       = MEDIASUBTYPE_ARGB32;
		return 0;
	}

	if (!is_fourcc(utvf))
		return -1;

	*subtype = MEDIASUBTYPE_YUY2;
	subtype->Data1 = FCC(utvf);

	return 0;
}

int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype)
{
	utvf_t bybi, byguid;

	if (VCMFormatToUtVideoFormat(&bybi, biCompression, biBitCount) != 0)
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

int WindowsFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype)
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

	if (UtVideoFormatToWindowsFormat(&guidtmp, utvftmp) != 0)
		return -1;

	*utvf = utvftmp;
	return 0;
}
