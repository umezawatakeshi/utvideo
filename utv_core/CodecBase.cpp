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

int CCodecBase::EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	LOGPRINTF("%p CCodecBase::EncodeBegin(infmt=%08X, width=%u, height=%u, cbGrossWidth[0]=%" PRIdSZT ")", this, infmt, width, height, cbGrossWidth[0]);

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

int CCodecBase::DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	if (IsLogWriterInitializedOrDebugBuild())
	{
		char buf[256];
		FormatBinary(buf, pExtraData, cbExtraData, EncodeGetExtraDataSize());
		LOGPRINTF("%p CCodecBase::DecodeBegin(outfmt=%08X, width=%u, height=%u, cbGrossWidth[0]=%" PRIdSZT ", pExtraData=%s, cbExtraData=%" PRIuSZT ")", this, outfmt, width, height, cbGrossWidth[0], buf, cbExtraData);
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

size_t CCodecBase::DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	int ret;
	FRAME_METRIC fm;

	ret = ::CalcRawFrameMetric(&fm, outfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return 0;

	return fm.cbTotalSize;
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

int CalcRawFrameMetric(FRAME_METRIC* pfm, utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	bool bBottomUp;

	switch (rawfmt)
	{
	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRA_BU:
		bBottomUp = true;
		break;

	default:
		bBottomUp = false;
	}

	switch (rawfmt)
	{
	case UTVF_YV24:
		pfm->nPlanes = 3;
		pfm->nMacroPixelWidth = 1;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width;
		pfm->cbLineWidth[1] = width;
		pfm->cbLineWidth[2] = width;
		pfm->nStripeLines[0] = 1;
		pfm->nStripeLines[1] = 1;
		pfm->nStripeLines[2] = 1;
		break;
	case UTVF_YV16:
		pfm->nPlanes = 3;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width;
		pfm->cbLineWidth[1] = width / 2;
		pfm->cbLineWidth[2] = width / 2;
		pfm->nStripeLines[0] = 1;
		pfm->nStripeLines[1] = 1;
		pfm->nStripeLines[2] = 1;
		break;
	case UTVF_YV12:
		pfm->nPlanes = 3;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 2;
		pfm->cbLineWidth[0] = width;
		pfm->cbLineWidth[1] = width / 2;
		pfm->cbLineWidth[2] = width / 2;
		pfm->nStripeLines[0] = 2;
		pfm->nStripeLines[1] = 1;
		pfm->nStripeLines[2] = 1;
		break;

	case UTVF_YUV444P10LE:
	case UTVF_YUV444P16LE:
		pfm->nPlanes = 3;
		pfm->nMacroPixelWidth = 1;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 2;
		pfm->cbLineWidth[1] = width * 2;
		pfm->cbLineWidth[2] = width * 2;
		pfm->nStripeLines[0] = 1;
		pfm->nStripeLines[1] = 1;
		pfm->nStripeLines[2] = 1;
		break;
	case UTVF_YUV422P10LE:
	case UTVF_YUV422P16LE:
		pfm->nPlanes = 3;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 2;
		pfm->cbLineWidth[1] = width;
		pfm->cbLineWidth[2] = width;
		pfm->nStripeLines[0] = 1;
		pfm->nStripeLines[1] = 1;
		pfm->nStripeLines[2] = 1;
		break;
	case UTVF_YUV420P10LE:
	case UTVF_YUV420P16LE:
		pfm->nPlanes = 3;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 2;
		pfm->cbLineWidth[0] = width * 2;
		pfm->cbLineWidth[1] = width;
		pfm->cbLineWidth[2] = width;
		pfm->nStripeLines[0] = 2;
		pfm->nStripeLines[1] = 1;
		pfm->nStripeLines[2] = 1;
		break;

	case UTVF_P210:
	case UTVF_P216:
		pfm->nPlanes = 2;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 2;
		pfm->cbLineWidth[1] = width * 2;
		pfm->nStripeLines[0] = 1;
		pfm->nStripeLines[1] = 1;
		break;
	case UTVF_P010:
	case UTVF_P016:
		pfm->nPlanes = 2;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 2;
		pfm->cbLineWidth[0] = width * 2;
		pfm->cbLineWidth[1] = width * 2;
		pfm->nStripeLines[0] = 2;
		pfm->nStripeLines[1] = 1;
		break;

	case UTVF_NFCC_BGR_BU:
	case UTVF_NFCC_BGR_TD:
	case UTVF_NFCC_RGB_TD:
		pfm->nPlanes = 1;
		pfm->nMacroPixelWidth = 1;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 3;
		pfm->nStripeLines[0] = 1;
		break;
	case UTVF_NFCC_BGRX_BU:
	case UTVF_NFCC_BGRA_BU:
	case UTVF_NFCC_BGRX_TD:
	case UTVF_NFCC_BGRA_TD:
	case UTVF_NFCC_ARGB_TD:
		pfm->nPlanes = 1;
		pfm->nMacroPixelWidth = 1;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 4;
		pfm->nStripeLines[0] = 1;
		break;

	case UTVF_YUY2:
	case UTVF_YUYV:
	case UTVF_YUNV:
	case UTVF_yuvs:
	case UTVF_UYVY:
	case UTVF_UYNV:
	case UTVF_2vuy:
	case UTVF_HDYC:
		pfm->nPlanes = 1;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 2;
		pfm->nStripeLines[0] = 1;
		break;

	case UTVF_v210:
		pfm->nPlanes = 1;
		pfm->nMacroPixelWidth = 2;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = ((width + 47) / 48) * 128;
		pfm->nStripeLines[0] = 1;
		break;

	case UTVF_b64a:
		pfm->nPlanes = 1;
		pfm->nMacroPixelWidth = 1;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 8;
		pfm->nStripeLines[0] = 1;
		break;
	case UTVF_b48r:
		pfm->nPlanes = 1;
		pfm->nMacroPixelWidth = 1;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = width * 6;
		pfm->nStripeLines[0] = 1;
		break;

	case UTVF_r210:
		pfm->nPlanes = 1;
		pfm->nMacroPixelWidth = 1;
		pfm->nMacroPixelHeight = 1;
		pfm->cbLineWidth[0] = ((width + 63) / 64) * 256;
		pfm->nStripeLines[0] = 1;
		break;

	default:
		return -1;
	}

	for (int i = 0; i < pfm->nPlanes; ++i)
	{
		pfm->nLines[i] = height / pfm->nMacroPixelHeight * pfm->nStripeLines[i];
	}

	pfm->nStripes = height / pfm->nMacroPixelHeight;

	size_t cbPlaneOffset = 0;
	for (int i = 0; i < pfm->nPlanes; ++i)
	{
		switch (cbGrossWidth[i])
		{
		case CBGROSSWIDTH_WINDOWS:
			switch (rawfmt)
			{
			case UTVF_NFCC_BGR_BU:
			case UTVF_NFCC_BGR_TD:
			case UTVF_NFCC_RGB_TD:
			case UTVF_NFCC_BGRX_BU:
			case UTVF_NFCC_BGRA_BU:
			case UTVF_NFCC_BGRX_TD:
			case UTVF_NFCC_BGRA_TD:
			case UTVF_NFCC_ARGB_TD:
				// BI_RGB の場合は4バイトアライメントである
				pfm->scbLineStride[i] = ROUNDUP(pfm->cbLineWidth[i], 4);
				break;
			default:
				pfm->scbLineStride[i] = pfm->cbLineWidth[i];
			}
			break;
		case CBGROSSWIDTH_NATURAL:
			pfm->scbLineStride[i] = pfm->cbLineWidth[i];
			break;
		default:
			pfm->scbLineStride[i] = cbGrossWidth[i];
		}

		pfm->cbPlaneOffset[i] = cbPlaneOffset;
		pfm->cbFirstLineOffset[i] = pfm->cbPlaneOffset[i];
		cbPlaneOffset += pfm->scbLineStride[i] * pfm->nLines[i];
		// plane と plane の間に隙間のあるフォーマットの場合は、後で調整すること。
	}
	pfm->cbTotalSize = cbPlaneOffset;

	// ボトムアップフォーマットのための調整
	if (bBottomUp)
	{
		for (int i = 0; i < pfm->nPlanes; ++i)
		{
			pfm->cbFirstLineOffset[i] += pfm->scbLineStride[i] * (pfm->nLines[i] - 1);
			pfm->scbLineStride[i] = -pfm->scbLineStride[i];
		}
	}

	// データ上の plane の並び順が G B R (A) あるいは Y U V (A) あるいは Y (A) と一致していない場合は、ここで調整する。
	switch (rawfmt)
	{
	case UTVF_YV24:
	case UTVF_YV16:
	case UTVF_YV12:
		std::swap(pfm->cbPlaneOffset[1], pfm->cbPlaneOffset[2]);
		std::swap(pfm->cbFirstLineOffset[1], pfm->cbFirstLineOffset[2]);
		break;
	}

	FixStripeValues(pfm);

	return 0;
}

void FixStripeValues(FRAME_METRIC* pfm)
{
	// エンコード処理の都合で必要な値の生成
	for (int i = 0; i < pfm->nPlanes; ++i)
	{
		pfm->scbStripeStride[i] = pfm->scbLineStride[i] * pfm->nStripeLines[i];
	}
}

int CCodecBase::CalcRawFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	return ::CalcRawFrameMetric(&m_fmRaw, rawfmt, width, height, cbGrossWidth);
}
