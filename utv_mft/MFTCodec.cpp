/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

// MFTCodec.cpp : CMFTCodec ‚ÌÀ‘•

#include "stdafx.h"
#include "MFTCodec.h"
#include <Format.h>

void FormatInfoToRegisterTypeInfo(const utvf_t *putvf, UINT32 *pcTypes, MFT_REGISTER_TYPE_INFO **ppTypes)
{
	const utvf_t *p;
	MFT_REGISTER_TYPE_INFO *prti;
	UINT32 n;
	bool b;

	p = putvf;
	n = 0;
	while (*p)
	{
		n++;
		p++;
	}

	*pcTypes = 0;
	*ppTypes = new MFT_REGISTER_TYPE_INFO[n];

	prti = *ppTypes;
	for (p = putvf; *p; p++)
	{
		if (UtVideoFormatToMediaFoundationFormat(&prti->guidSubtype, &b, *p) == 0)
		{
			prti->guidMajorType = MFMediaType_Video;
			prti++;
			(*pcTypes)++;
		}
	}
}
