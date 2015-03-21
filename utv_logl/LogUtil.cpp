/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "LogUtil.h"

void FormatBinary(char *pszDst, const void *pData, size_t cb, size_t cbLimit)
{
	if (pData == NULL)
		strcpy(pszDst, "<NULL>");
	else
	{
		char buf2[16];
		unsigned int i = 0;

		pszDst[0] = '\0';
		for (; i < cb && i < cbLimit; i++)
		{
			if (i != 0 && i % 4 == 0)
				strcat(pszDst, " ");
			sprintf(buf2, "%02X", ((const unsigned char *)pData)[i]);
			strcat(pszDst, buf2);
		}
		if (i < cb)
			strcat(pszDst, " ...");
	}
}
