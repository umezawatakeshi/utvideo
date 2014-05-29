/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"

class CCodecBase :
	public CCodec
{
protected:
	const char *const m_pszTinyName;
	const char *const m_pszInterfaceName;

	bool m_bBottomUpFrame;
	size_t m_cbRawSize;
	size_t m_cbRawGrossWidth;
	size_t m_cbRawNetWidth;

protected:
	CCodecBase(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CCodecBase(void);

public:
	virtual const char *GetTinyName(void);
	virtual void GetShortFriendlyName(char *pszName, size_t cchName);
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
	virtual int SetState(const void *pState, size_t cb);

protected:
	virtual int InternalSetState(const void *pState, size_t cb) = 0;
	int CalcRawFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
};
