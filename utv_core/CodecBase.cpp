/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "CodecBase.h"

CCodecBase::CCodecBase(const char *pszTinyName, const char *pszInterfaceName) : m_pszTinyName(pszTinyName), m_pszInterfaceName(pszInterfaceName)
{
	LOGPRINTF("%p CCodecBase::CCodecBase(pszTinyName=\"%s\", pszInterfaceName=\"%s\")", this, pszTinyName, pszInterfaceName);
}

CCodecBase::~CCodecBase(void)
{
	LOGPRINTF("%p CCodecBase::~CCodecBase()", this);
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

int CCodecBase::LoadConfig(void)
{
#ifdef _WIN32
	HKEY hkUtVideo;
	DWORD dwSaveConfig;
	DWORD cb;
	DWORD dwType;
	char buf[16];
	char szValueName[16];

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Ut Video Codec Suite", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkUtVideo, NULL) != ERROR_SUCCESS)
		return -1;

	cb = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "SaveConfig", NULL, &dwType, (uint8_t *)&dwSaveConfig, &cb) != ERROR_SUCCESS)
		goto notloaded;
	if (!dwSaveConfig)
		goto notloaded;

	wsprintf(szValueName, "Config%s", GetTinyName());
	cb = (DWORD)GetStateSize();
	_ASSERT(cb <= sizeof(buf));
	if (RegQueryValueEx(hkUtVideo, szValueName, NULL, &dwType, (BYTE *)buf, &cb) != ERROR_SUCCESS)
		goto notloaded;
	InternalSetState(buf, cb);

	RegCloseKey(hkUtVideo);
	return 0;

notloaded:
	RegCloseKey(hkUtVideo);
	return -1;
#endif
#if defined(__APPLE__) || defined(__unix__)
	return 0;
#endif
}

int CCodecBase::SaveConfig(void)
{
#ifdef _WIN32
	HKEY hkUtVideo;
	DWORD dwSaveConfig;
	DWORD cb;
	DWORD dwType;
	char buf[16];
	char szValueName[16];

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Ut Video Codec Suite", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkUtVideo, NULL) != ERROR_SUCCESS)
		return -1;

	cb = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "SaveConfig", NULL, &dwType, (uint8_t *)&dwSaveConfig, &cb) != ERROR_SUCCESS)
		goto notsaved;
	if (!dwSaveConfig)
		goto notsaved;

	wsprintf(szValueName, "Config%s", GetTinyName());
	cb = (DWORD)GetStateSize();
	_ASSERT(cb <= sizeof(buf));
	GetState(buf, cb);
	if (RegSetValueEx(hkUtVideo, szValueName, 0, REG_BINARY, (const BYTE *)buf, cb) != ERROR_SUCCESS)
		goto notsaved;

	RegCloseKey(hkUtVideo);
	return 0;

notsaved:
	RegCloseKey(hkUtVideo);
	return -1;
#endif
#if defined(__APPLE__) || defined (__unix__)
	return 0;
#endif
}

int CCodecBase::SetState(const void *pState, size_t cb)
{
#ifdef _WIN32
	HKEY hkUtVideo;
	DWORD dwIgnoreSetConfig;
	DWORD cbRegData;
	DWORD dwType;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Ut Video Codec Suite", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkUtVideo, NULL) != ERROR_SUCCESS)
		goto doset_noclose;

	cbRegData = sizeof(DWORD);
	if (RegQueryValueEx(hkUtVideo, "IgnoreSetConfig", NULL, &dwType, (uint8_t *)&dwIgnoreSetConfig, &cbRegData) != ERROR_SUCCESS)
		goto doset;
	if (!dwIgnoreSetConfig)
		goto doset;

	RegCloseKey(hkUtVideo);
	return 0;

doset:
	RegCloseKey(hkUtVideo);
doset_noclose:
	return InternalSetState(pState, cb);
#endif
#if defined(__APPLE__) || defined (__unix__)
	return InternalSetState(pState, cb);
#endif
}

int CCodecBase::InternalSetStateWrapper(const void *pState, size_t cb)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		char buf[256];
		FormatBinary(buf, pState, cb, GetStateSize());
		LOGPRINTF("%p CCodecBase::InternalSetStateWrapper(pState=%s, cb=%" PRIuSZT ")", this, buf, cb);
	}

	int ret = InternalSetState(pState, cb);
	LOGPRINTF("%p CCodecBase::InternalSetStateWrapper return %d", this, ret);
	return ret;
}

int CCodecBase::EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	LOGPRINTF("%p CCodecBase::EncodeBegin(infmt=%08X, width=%u, height=%u, cbGrossWidth=%" PRIdSZT ")", this, infmt, width, height, cbGrossWidth);

	int ret = InternalEncodeBegin(infmt, width, height, cbGrossWidth);
	LOGPRINTF("%p CCodecBase::EncodeBegin return %d", this, ret);
	return ret;
}

int CCodecBase::EncodeEnd(void)
{
	LOGPRINTF("%p CCodecBase::EncodeEnd()", this);

	return InternalEncodeEnd();
}

int CCodecBase::EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height)
{
	LOGPRINTF("%p CCodecBase::EncodeQuery(infmt=%08X, width=%u, height=%u)", this, infmt, width, height);

	int ret = InternalEncodeQuery(infmt, width, height);
	LOGPRINTF("%p CCodecBase::EncodeQuery return %d", this, ret);
	return ret;
}

int CCodecBase::DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		char buf[256];
		FormatBinary(buf, pExtraData, cbExtraData, EncodeGetExtraDataSize());
		LOGPRINTF("%p CCodecBase::DecodeBegin(outfmt=%08X, width=%u, height=%u, cbGrossWidth=%" PRIdSZT ", pExtraData=%s, cbExtraData=%" PRIuSZT ")", this, outfmt, width, height, cbGrossWidth, buf, cbExtraData);
	}

	int ret = InternalDecodeBegin(outfmt, width, height, cbGrossWidth, pExtraData, cbExtraData);
	LOGPRINTF("%p CCodecBase::DecodeBegin return %d", this, ret);
	return ret;
}

int CCodecBase::DecodeEnd(void)
{
	LOGPRINTF("%p CCodecBase::DecodeEnd()", this);

	return InternalDecodeEnd();
}

int CCodecBase::DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		char buf[256];
		FormatBinary(buf, pExtraData, cbExtraData, EncodeGetExtraDataSize());
		LOGPRINTF("%p CCodecBase::DecodeQuery(outfmt=%08X, width=%u, height=%u, pExtraData=%s, cbExtraData=%" PRIuSZT ")", this, outfmt, width, height, buf, cbExtraData);
	}

	int ret = InternalDecodeQuery(outfmt, width, height, pExtraData, cbExtraData);
	LOGPRINTF("%p CCodecBase::DecodeQuery return %d", this, ret);
	return ret;
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
		case UTVF_yuvs:
		case UTVF_UYVY:
		case UTVF_UYNV:
		case UTVF_2vuy:
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
