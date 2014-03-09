/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "CodecBase.h"

CCodecBase::CCodecBase(const char *pszTinyName, const char *pszInterfaceName) : m_pszTinyName(pszTinyName), m_pszInterfaceName(pszInterfaceName)
{
}

CCodecBase::~CCodecBase(void)
{
}

const char *CCodecBase::GetTinyName(void)
{
	return m_pszTinyName;
}

void CCodecBase::GetShortFriendlyName(wchar_t *pszName, size_t cchName)
{
	char buf[16];

	GetShortFriendlyName(buf, min(cchName, _countof(buf)));
	mbstowcs(pszName, buf, cchName);
	pszName[cchName - 1] = L'\0';
}

void CCodecBase::GetLongFriendlyName(wchar_t *pszName, size_t cchName)
{
	char buf[128];

	GetLongFriendlyName(buf, min(cchName, _countof(buf)));
	mbstowcs(pszName, buf, cchName);
	pszName[cchName - 1] = L'\0';
}
