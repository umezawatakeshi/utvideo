/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
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

void CCodecBase::GetShortFriendlyName(char *pszName, size_t cchName)
{
	char buf[16];

	sprintf(buf, "UtVideo (%s)", m_pszTinyName);
	strncpy(pszName, buf, cchName);
	pszName[cchName - 1] = '\0';
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

int CCodecBase::CalcRawFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	m_bBottomUpFrame = false;
	switch (rawfmt)
	{
	case UTVF_YV12:
		m_cbRawSize = (width * height * 3) / 2; // XXX 幅や高さが奇数の場合は考慮していない
		break;
	default:
		switch (rawfmt)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRA_BU:
			m_bBottomUpFrame = true;
		}

		switch (rawfmt)
		{
		case UTVF_NFCC_BGR_BU:
		case UTVF_NFCC_BGR_TD:
		case UTVF_NFCC_RGB_TD:
			m_cbRawNetWidth = width * 3;
			break;
		case UTVF_NFCC_BGRX_BU:
		case UTVF_NFCC_BGRA_BU:
		case UTVF_NFCC_BGRX_TD:
		case UTVF_NFCC_BGRA_TD:
		case UTVF_NFCC_ARGB_TD:
			m_cbRawNetWidth = width * 4;
			break;
		case UTVF_YUY2:
		case UTVF_YUYV:
		case UTVF_YUNV:
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_HDYC:
			m_cbRawNetWidth = width * 2;
			break;
		case UTVF_v210:
			m_cbRawNetWidth = ((width + 47) / 48) * 128;
			break;
		default:
			return -1;
		}

		switch (cbGrossWidth)
		{
		case CBGROSSWIDTH_NATURAL:
			m_cbRawGrossWidth = m_cbRawNetWidth;
			break;
		case CBGROSSWIDTH_WINDOWS:
			/*
			 * BI_RGB の場合は4バイトアライメントであるが、
			 * その他の場合に4バイトアライメントにすべきかどうかは実は不明確である。
			 */
			m_cbRawGrossWidth = ROUNDUP(m_cbRawNetWidth, 4);
			break;
		default:
			m_cbRawGrossWidth = cbGrossWidth;
		}

		m_cbRawSize = m_cbRawGrossWidth * height;
	}

	return 0;
}
