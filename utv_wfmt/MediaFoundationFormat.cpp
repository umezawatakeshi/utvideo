/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Format.h"

inline bool is_fourcc(uint32_t x)
{
	return (x >= '    ' /* four SP */ && x <= 0x7e7e7e7e);
}

int UtVideoFormatToMediaFoundationFormat(GUID *subtype, utvf_t utvf)
{
	switch (utvf)
	{
	case UTVF_NFCC_BGR_TD:
		*subtype       = MFVideoFormat_RGB24;
		return 0;
	case UTVF_NFCC_BGRX_TD:
		*subtype       = MFVideoFormat_RGB32;
		return 0;
	case UTVF_NFCC_BGRA_TD:
		*subtype       = MFVideoFormat_ARGB32;
		return 0;
	}

	if (!is_fourcc(utvf))
		return -1;

	*subtype = MFVideoFormat_YUY2;
	subtype->Data1 = FCC(utvf);

	return 0;
}

int MediaFoundationFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype)
{
	GUID guidtmp;
	utvf_t utvftmp;

	guidtmp = subtype;
	guidtmp.Data1 = MFVideoFormat_YUY2.Data1;
	if (IsEqualGUID(guidtmp, MFVideoFormat_YUY2) && is_fourcc(subtype.Data1))
		utvftmp = UNFCC(subtype.Data1);
	else if (IsEqualGUID(subtype, MFVideoFormat_RGB24))
		utvftmp = UTVF_NFCC_BGR_TD;
	else if (IsEqualGUID(subtype, MFVideoFormat_RGB32))
		utvftmp = UTVF_NFCC_BGRX_TD;
	else if (IsEqualGUID(subtype, MFVideoFormat_ARGB32))
		utvftmp = UTVF_NFCC_BGRA_TD;
	else
		return -1;

	if (UtVideoFormatToMediaFoundationFormat(&guidtmp, utvftmp) != 0)
		return -1;

	*utvf = utvftmp;
	return 0;
}
