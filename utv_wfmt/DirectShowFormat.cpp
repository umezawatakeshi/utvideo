/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Format.h"

inline bool is_fourcc(uint32_t x)
{
	return (x >= '    ' /* four SP */ && x <= 0x7e7e7e7e);
}

int UtVideoFormatToDirectShowFormat(GUID *subtype, utvf_t utvf)
{
	switch (utvf)
	{
	case UTVF_NFCC_BGR_BU:
		*subtype       = MEDIASUBTYPE_RGB24;
		return 0;
	case UTVF_NFCC_BGRX_BU:
		*subtype       = MEDIASUBTYPE_RGB32;
		return 0;
	case UTVF_NFCC_BGRA_BU:
		*subtype       = MEDIASUBTYPE_ARGB32;
		return 0;
	}

	if (!is_fourcc(utvf))
		return -1;

	*subtype = MEDIASUBTYPE_YUY2;
	subtype->Data1 = FCC(utvf);

	return 0;
}

int DirectShowFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype)
{
	utvf_t bybi, byguid;

	if (VCMFormatToUtVideoFormat(&bybi, biCompression, biBitCount) != 0)
		return -1;
	if (DirectShowFormatToUtVideoFormat(&byguid, subtype) != 0)
		return -1;

	if (bybi == byguid || (bybi == UTVF_NFCC_BGRX_BU && byguid == UTVF_NFCC_BGRA_BU))
	{
		*utvf = byguid;
		return 0;
	}
	else
		return -1;
}

int DirectShowFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype)
{
	GUID guidtmp;
	utvf_t utvftmp;

	guidtmp = subtype;
	guidtmp.Data1 = MEDIASUBTYPE_YUY2.Data1;
	if (IsEqualGUID(guidtmp, MEDIASUBTYPE_YUY2))
		utvftmp = UNFCC(subtype.Data1);
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_RGB24))
		utvftmp = UTVF_NFCC_BGR_BU;
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_RGB32))
		utvftmp = UTVF_NFCC_BGRX_BU;
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_ARGB32))
		utvftmp = UTVF_NFCC_BGRA_BU;
	else
		return -1;

	if (UtVideoFormatToDirectShowFormat(&guidtmp, utvftmp) != 0)
		return -1;

	*utvf = utvftmp;
	return 0;
}
