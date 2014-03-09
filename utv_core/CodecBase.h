/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once
#include "Codec.h"

class CCodecBase :
	public CCodec
{
protected:
	const char *const m_pszTinyName;
	const char *const m_pszInterfaceName;

protected:
	CCodecBase(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CCodecBase(void);

public:
	virtual const char *GetTinyName(void);
	virtual void GetShortFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
};
