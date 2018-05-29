/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "utv_core.h"
#include "UM00Codec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "ByteOrder.h"
#include "SymPack.h"
#include "resource.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <lz4.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

CUM00Codec::CUM00Codec(const char *pszTinyName, const char *pszInterfaceName) : CCodecBase(pszTinyName, pszInterfaceName)
{
	SetDefaultState();
	LoadConfig();
}

void CUM00Codec::GetLongFriendlyName(char *pszName, size_t cchName)
{
	char buf[128];

	sprintf(buf, "UtVideo T2 %s %s",
		GetColorFormatName(),
		m_pszInterfaceName);
	strncpy(pszName, buf, cchName);
	pszName[cchName - 1] = '\0';
}

#ifdef _WIN32
INT_PTR CUM00Codec::Configure(HWND hwnd)
{
	DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_UM00_CONFIG), hwnd, DialogProc, (LPARAM)this);
	return 0;
}

INT_PTR CALLBACK CUM00Codec::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CUM00Codec *pThis = (CUM00Codec *)GetWindowLongPtr(hwnd, DWLP_USER);
	char buf[256];
	int	n;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		pThis = (CUM00Codec *)lParam;
		pThis->GetLongFriendlyName(buf, _countof(buf));
		SetWindowText(hwnd, buf);
		wsprintf(buf, "%d", pThis->m_ec.ecDivideCountMinusOne + 1);
		SetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf);
		if (pThis->m_ec.ecFlags & EC_FLAGS_DIVIDE_COUNT_AUTO)
		{
			CheckDlgButton(hwnd, IDC_DIVIDE_COUNT_AUTO, BST_CHECKED);
			EnableDlgItem(hwnd, IDC_DIVIDE_COUNT_EDIT, FALSE);
		}
		if (pThis->m_ec.ecFlags & EC_FLAGS_USE_TEMPORAL_COMPRESSION)
			CheckDlgButton(hwnd, IDC_USE_TEMPORAL_COMPRESSION_CHECK, BST_CHECKED);
		else
			EnableDlgItem(hwnd, IDC_KEY_FRAME_INTERVAL_EDIT, FALSE);
		wsprintf(buf, "%d", pThis->m_ec.ecKeyFrameIntervalMinusOne + 1);
		SetDlgItemText(hwnd, IDC_KEY_FRAME_INTERVAL_EDIT, buf);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			ENCODERCONF ec;
			memset(&ec, 0, sizeof(ENCODERCONF));
			if (IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_AUTO))
			{
				ec.ecFlags |= EC_FLAGS_DIVIDE_COUNT_AUTO;
				ec.ecDivideCountMinusOne = CThreadManager::GetNumProcessors() - 1;
			}
			else
			{
				GetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf, sizeof(buf));
				n = atoi(buf);
				if (n < 1 || n > 256)
				{
					MessageBox(hwnd, "1 <= DIVIDE_COUNT <= 256", "ERR", MB_ICONERROR);
					return TRUE;
				}
				ec.ecDivideCountMinusOne = n - 1;
			}

			if (IsDlgButtonChecked(hwnd, IDC_USE_TEMPORAL_COMPRESSION_CHECK))
				ec.ecFlags |= EC_FLAGS_USE_TEMPORAL_COMPRESSION;

			GetDlgItemText(hwnd, IDC_KEY_FRAME_INTERVAL_EDIT, buf, sizeof(buf));
			n = atoi(buf);
			if (n < 1 || n > EC_KEY_FRAME_INTERVAL_MAX)
			{
				MessageBox(hwnd, "1 <= KEY_FRAME_INTERVAL <= 60000", "ERR", MB_ICONERROR);
				return TRUE;
			}
			ec.ecKeyFrameIntervalMinusOne = n - 1;

			pThis->m_ec = ec;
			pThis->SaveConfig();
			/* FALLTHROUGH */
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		case IDC_DIVIDE_COUNT_AUTO:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				EnableDlgItem(hwnd, IDC_DIVIDE_COUNT_EDIT, !IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_AUTO));
				if (IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_AUTO))
				{
					wsprintf(buf, "%d", CThreadManager::GetNumProcessors());
					SetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf);
				}
			}
			break;
		case IDC_USE_TEMPORAL_COMPRESSION_CHECK:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				EnableDlgItem(hwnd, IDC_KEY_FRAME_INTERVAL_EDIT, IsDlgButtonChecked(hwnd, IDC_USE_TEMPORAL_COMPRESSION_CHECK));
			}
			break;
		}
		break;
	}

	return FALSE;
}
#endif

size_t CUM00Codec::GetStateSize(void)
{
	return sizeof(ENCODERCONF);
}

int CUM00Codec::GetState(void *pState, size_t cb)
{
	if (cb < sizeof(ENCODERCONF))
		return -1;

	memcpy(pState, &m_ec, sizeof(ENCODERCONF));
	return 0;
}

void CUM00Codec::SetDefaultState()
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));
	m_ec.ecFlags = EC_FLAGS_DIVIDE_COUNT_AUTO;
	m_ec.ecDivideCountMinusOne = CThreadManager::GetNumProcessors() - 1;
	m_ec.ecKeyFrameIntervalMinusOne = EC_KEY_FRAME_INTERVAL_DEFAULT - 1;
}

int CUM00Codec::InternalSetState(const void *pState, size_t cb)
{
	if (pState == NULL)
	{
		SetDefaultState();
		return sizeof(ENCODERCONF);
	}

	ENCODERCONF ec;

	memset(&ec, 0, sizeof(ENCODERCONF));
	memcpy(&ec, pState, min(sizeof(ENCODERCONF), cb));

	if ((ec.ecFlags & EC_FLAGS_RESERVED) != 0)
		return -1;
	if (ec.ecKeyFrameIntervalMinusOne > EC_KEY_FRAME_INTERVAL_MAX - 1)
		return -1;

	if (sizeof(ENCODERCONF) < cb && is_not_all_zero((uint8_t*)pState + sizeof(ENCODERCONF), (uint8_t*)pState + cb))
		return -1;

	memcpy(&m_ec, &ec, sizeof(ENCODERCONF));

	if (m_ec.ecFlags & EC_FLAGS_DIVIDE_COUNT_AUTO)
	{
		m_ec.ecDivideCountMinusOne = CThreadManager::GetNumProcessors() - 1;
	}

	return 0;
}

size_t CUM00Codec::EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput)
{
	m_pInput = pInput;
	m_pOutput = pOutput;

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUM00Codec::EncodeProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();


	FRAMEINFO *fi = (FRAMEINFO*)pOutput;

	uint8_t *pPackedStreamsBegin = (uint8_t *)pOutput + sizeof(FRAMEINFO);
	size_t cbPackedStreams = 0;
	for (int j = 0; j < GetNumPlanes(); ++j)
		for (unsigned int i = 0; i < m_dwDivideCount; ++i)
			cbPackedStreams += m_cbPackedStream[j][i];
	cbPackedStreams = ROUNDUP(cbPackedStreams, (size_t)8);

	uint8_t *pControlStreamsBegin = pPackedStreamsBegin + cbPackedStreams;
	size_t cbControlStreams = 0;
	for (int j = 0; j < GetNumPlanes(); ++j)
		for (unsigned int i = 0; i < m_dwDivideCount; ++i)
			cbControlStreams += m_cbControlStream[j][i];
	cbControlStreams = ROUNDUP(cbControlStreams, (size_t)8);

	uint32_t *pcbControlStreams = (uint32_t*)(pControlStreamsBegin + cbControlStreams);
	uint32_t *pPackedStreamSizeArray = pcbControlStreams + 1;
	uint32_t *pControlStreamSizeArray = pPackedStreamSizeArray + GetNumPlanes() * m_dwDivideCount;
	uint8_t *pOutputEnd = (uint8_t*)(pControlStreamSizeArray + GetNumPlanes() * m_dwDivideCount);
	if (pOutputEnd - pControlStreamsBegin <= 256)
		pOutputEnd = pControlStreamsBegin + 256;

	memset(fi, 0, sizeof(FRAMEINFO));
	fi->fiFrameType = (m_nFrameNumber == 0) ? FI_FRAME_TYPE_INTRA : FI_FRAME_TYPE_DELTA;
	fi->fiFlags = (m_nFrameNumber == 0) ? 0 : FI_FLAGS_CONTROL_COMPRESSED;
	fi->fiSizeArrayOffset = (uint32_t)(cbPackedStreams + cbControlStreams);

	uint8_t *p = pPackedStreamsBegin;
	for (int j = 0; j < GetNumPlanes(); ++j)
	{
		for (unsigned int i = 0; i < m_dwDivideCount; ++i)
		{
			memcpy(p, m_pPackedStream[j][i], m_cbPackedStream[j][i]);
			p += m_cbPackedStream[j][i];
		}
	}
	memset(p, 0, pControlStreamsBegin - p);

	p = pControlStreamsBegin;
	for (int j = 0; j < GetNumPlanes(); ++j)
	{
		for (unsigned int i = 0; i < m_dwDivideCount; ++i)
		{
			memcpy(p, m_pControlStream[j][i], m_cbControlStream[j][i]);
			p += m_cbControlStream[j][i];
		}
	}
	memset(p, 0, (uint8_t *)pcbControlStreams - p);

	*pcbControlStreams = (uint32_t)cbControlStreams;
	for (int j = 0; j < GetNumPlanes(); ++j)
		for (unsigned int i = 0; i < m_dwDivideCount; ++i)
			*pPackedStreamSizeArray++ = (uint32_t)m_cbPackedStream[j][i];
	for (int j = 0; j < GetNumPlanes(); ++j)
		for (unsigned int i = 0; i < m_dwDivideCount; ++i)
			*pControlStreamSizeArray++ = (uint32_t)m_cbControlStream[j][i];

	memset(pControlStreamSizeArray, 0, pOutputEnd - (uint8_t*)pControlStreamSizeArray);

	*pbKeyFrame = (m_nFrameNumber == 0);
	(++m_nFrameNumber) %= m_nKeyFrameInterval;
	if (m_nKeyFrameInterval > 1)
		std::swap(m_pCurFrame, m_pPrevFrame);
	return pOutputEnd - ((uint8_t *)pOutput);
}

int CUM00Codec::CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	const STREAMINFO *p = (const STREAMINFO *)pExtraData;

	CalcRawFrameMetric(rawfmt, width, height, cbGrossWidth);
	CalcPlaneSizes(width, height);

	m_dwNumStripes = height / GetMacroPixelHeight();
	m_cbRawStripeSize = m_cbRawGrossWidth * GetMacroPixelHeight();

	return 0;
}

void CUM00Codec::CalcStripeMetric(void)
{
	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
	{
		m_dwPlaneStripeBegin[nBandIndex] = m_dwNumStripes *  nBandIndex      / m_dwDivideCount;
		m_dwPlaneStripeEnd[nBandIndex]   = m_dwNumStripes * (nBandIndex + 1) / m_dwDivideCount;

		if (!m_bBottomUpFrame)
		{
			m_dwRawStripeBegin[nBandIndex] = m_dwPlaneStripeBegin[nBandIndex];
			m_dwRawStripeEnd[nBandIndex]   = m_dwPlaneStripeEnd[nBandIndex];
		}
		else
		{
			m_dwRawStripeBegin[nBandIndex] = m_dwNumStripes - m_dwPlaneStripeEnd[nBandIndex];
			m_dwRawStripeEnd[nBandIndex]   = m_dwNumStripes - m_dwPlaneStripeBegin[nBandIndex];
		}
	}
}

int CUM00Codec::InternalEncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	int ret;
	STREAMINFO si;

	ret = EncodeQuery(infmt, width, height);
	if (ret != 0)
		return ret;

	m_utvfRaw = infmt;
	m_nWidth = width;
	m_nHeight = height;

	EncodeGetExtraData(&si, sizeof(si), infmt, width, height);
	ret = CalcFrameMetric(infmt, width, height, cbGrossWidth, &si, sizeof(si));
	if (ret != 0)
		return ret;
	m_dwDivideCount = m_ec.ecDivideCountMinusOne + 1;
	m_nKeyFrameInterval = (m_ec.ecFlags & EC_FLAGS_USE_TEMPORAL_COMPRESSION) ? (m_ec.ecKeyFrameIntervalMinusOne + 1) : 1;
	CalcStripeMetric();

	m_pCurFrame = std::make_unique<CFrameBuffer>();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	if (m_nKeyFrameInterval > 1)
	{
		m_pPrevFrame = std::make_unique<CFrameBuffer>();
		for (int i = 0; i < GetNumPlanes(); i++)
			m_pPrevFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);
	}

	m_ptm = std::make_unique<CThreadManager>();

	for (int i = 0; i < GetNumPlanes(); i++)
	{
		for (unsigned int j = 0; j < m_dwDivideCount; ++j)
		{
			m_pPackedStream[i][j] = (uint8_t*)malloc(m_cbPlaneStripeSize[i] * (m_dwPlaneStripeEnd[j] - m_dwPlaneStripeBegin[j]));
			m_pControlStream[i][j] = (uint8_t*)malloc(LZ4_compressBound(m_cbPlaneStripeSize[i] * (m_dwPlaneStripeEnd[j] - m_dwPlaneStripeBegin[j]) / 64 * 4));
			if (m_nKeyFrameInterval > 1)
				m_pTmpControlStream[i][j] = (uint8_t*)malloc(m_cbPlaneStripeSize[i] * (m_dwPlaneStripeEnd[j] - m_dwPlaneStripeBegin[j]) / 64 * 4);
		}
	}

	m_nFrameNumber = 0;

	return 0;
}

int CUM00Codec::InternalEncodeEnd(void)
{
	for (int i = 0; i < GetNumPlanes(); i++)
	{
		for (unsigned int j = 0; j < m_dwDivideCount; ++j)
		{
			free(m_pPackedStream[i][j]);
			free(m_pControlStream[i][j]);
			if (m_nKeyFrameInterval > 1)
				free(m_pTmpControlStream[i][j]);
		}
	}

	m_pCurFrame.reset();
	m_pPrevFrame.reset();

	m_ptm.reset();

	return 0;
}

size_t CUM00Codec::EncodeGetExtraDataSize(void)
{
	return sizeof(STREAMINFO);
}

int CUM00Codec::EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height)
{
	STREAMINFO *p = (STREAMINFO *)pExtraData;

	if (cb < sizeof(STREAMINFO))
		return -1;

	memset(p, 0, cb);

	p->siEncoderVersionAndImplementation = htol32(UTVIDEO_VERSION_AND_IMPLEMENTATION);
	p->siOriginalFormat                  = htob32(infmt);
	p->siEncodingMode                    = SI_ENCODING_MODE_8SYMPACK;
	p->siDivideCountMinusOne             = m_ec.ecDivideCountMinusOne;
	if ((m_ec.ecFlags & EC_FLAGS_USE_TEMPORAL_COMPRESSION) && m_ec.ecKeyFrameIntervalMinusOne > 0)
		p->siFlags                      |= SI_FLAGS_USE_TEMPORAL_COMPRESSION | SI_FLAGS_USE_CONTROL_COMPRESSION;

	return 0;
}

size_t CUM00Codec::EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height)
{
	return ROUNDUP(width, 128) * height * GetRealBitCount() / 64 * 68 / 8 + 4096; // +4096 ÇÕÇ«ÇÒÇ‘ÇËä®íËÅB
}

int CUM00Codec::InternalEncodeQuery(utvf_t infmt, unsigned int width, unsigned int height)
{
	if (width % GetMacroPixelWidth() != 0 || height % GetMacroPixelHeight() != 0)
		return -1;

	for (const utvf_t *utvf = GetEncoderInputFormat(); *utvf; utvf++)
	{
		if (infmt == *utvf)
			return 0;
	}

	return -1;
}

bool CUM00Codec::PredictDirect(uint32_t nBandIndex)
{
	return false;
}

void CUM00Codec::EncodeProc(uint32_t nBandIndex)
{
	if (m_nKeyFrameInterval <= 1)
	{
		if (PredictDirect(nBandIndex))
			return;
	}

	ConvertToPlanar(nBandIndex);

	const uint8_t* pSrcBegin[4];
	const uint8_t* pPrevBegin[4];
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
		pSrcBegin[nPlaneIndex] = m_pCurFrame->GetPlane(nPlaneIndex);
	if (m_nKeyFrameInterval > 1)
	{
		for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
			pPrevBegin[nPlaneIndex] = m_pPrevFrame->GetPlane(nPlaneIndex);
	}
	PredictFromPlanar(nBandIndex, pSrcBegin, pPrevBegin);
}

void CUM00Codec::PredictFromPlanar(uint32_t nBandIndex, const uint8_t* const* pSrcBegin, const uint8_t* const* pPrevBegin)
{
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwPlaneStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		if (m_nFrameNumber == 0)
		{
			Pack8SymAfterPredictPlanarGradient8(
				m_pPackedStream[nPlaneIndex][nBandIndex], &m_cbPackedStream[nPlaneIndex][nBandIndex],
				m_pControlStream[nPlaneIndex][nBandIndex],
				pSrcBegin[nPlaneIndex] + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneEnd,
				m_cbPlanePredictStride[nPlaneIndex]);
			m_cbControlStream[nPlaneIndex][nBandIndex] = (cbPlaneEnd - cbPlaneBegin) / 64 * 3;
		}
		else
		{
			Pack8SymWithDiff8(
				m_pPackedStream[nPlaneIndex][nBandIndex], &m_cbPackedStream[nPlaneIndex][nBandIndex],
				m_pTmpControlStream[nPlaneIndex][nBandIndex],
				pSrcBegin[nPlaneIndex] + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneEnd,
				pPrevBegin[nPlaneIndex] + cbPlaneBegin,
				m_cbPlanePredictStride[nPlaneIndex]);
			m_cbControlStream[nPlaneIndex][nBandIndex] = LZ4_compress_default((char*)m_pTmpControlStream[nPlaneIndex][nBandIndex], (char*)m_pControlStream[nPlaneIndex][nBandIndex], (cbPlaneEnd - cbPlaneBegin) / 64 * 4, LZ4_compressBound((cbPlaneEnd - cbPlaneBegin) / 64 * 4));
		}
	}
}

size_t CUM00Codec::DecodeFrame(void *pOutput, const void *pInput)
{
	m_pInput = pInput;
	m_pOutput = pOutput;

	m_fiDecode = (const FRAMEINFO*)pInput;

	if (is_not_all_zero(m_fiDecode->fiReserved))
		return m_cbRawSize;

	if (m_fiDecode->fiFrameType != FI_FRAME_TYPE_INTRA && m_fiDecode->fiFrameType != FI_FRAME_TYPE_DELTA)
		return m_cbRawSize;

	const uint8_t *pPackedStreams = (const uint8_t *)pInput + sizeof(FRAMEINFO);
	const uint32_t *pcbControlStreams = (const uint32_t*)(pPackedStreams + m_fiDecode->fiSizeArrayOffset);
	const uint8_t *pControlStreams = (const uint8_t*)pcbControlStreams - *pcbControlStreams;
	const uint32_t *pPackedStreamSizeArray = pcbControlStreams + 1;
	const uint32_t *pControlStreamSizeArray = pPackedStreamSizeArray + GetNumPlanes() * m_dwDivideCount;

	for (int i = 0; i < GetNumPlanes(); i++)
	{
		for (unsigned int j = 0; j < m_dwDivideCount; ++j)
		{
			m_pPackedStream[i][j] = (uint8_t *)pPackedStreams;
			m_cbPackedStream[i][j] = *pPackedStreamSizeArray;
			pPackedStreams += *pPackedStreamSizeArray;
			++pPackedStreamSizeArray;

			m_pControlStream[i][j] = (uint8_t *)pControlStreams;
			m_cbControlStream[i][j] = *pControlStreamSizeArray;
			pControlStreams += *pControlStreamSizeArray;
			++pControlStreamSizeArray;
		}
	}

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUM00Codec::DecodeProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	if (m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION)
		std::swap(m_pCurFrame, m_pPrevFrame);

	return m_cbRawSize;
}

int CUM00Codec::DecodeGetFrameType(bool *pbKeyFrame, const void *pInput)
{
	*pbKeyFrame = true;
	return 0;
}

int CUM00Codec::InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	int ret;

	ret = DecodeQuery(outfmt, width, height, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	ret = CalcFrameMetric(outfmt, width, height, cbGrossWidth, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	memset(&m_siDecode, 0, sizeof(STREAMINFO));
	memcpy(&m_siDecode, pExtraData, min(sizeof(STREAMINFO), cbExtraData));

	m_dwDivideCount = m_siDecode.siDivideCountMinusOne + 1;
	CalcStripeMetric();

	m_utvfRaw = outfmt;
	m_nWidth = width;
	m_nHeight = height;

	if (m_siDecode.siFlags & SI_FLAGS_USE_CONTROL_COMPRESSION)
	{
		for (int i = 0; i < GetNumPlanes(); i++)
			for (unsigned int j = 0; j < m_dwDivideCount; ++j)
				m_pTmpControlStream[i][j] = (uint8_t*)malloc(m_cbPlaneStripeSize[i] * (m_dwPlaneStripeEnd[j] - m_dwPlaneStripeBegin[j]) / 64 * 4);
	}

	m_pCurFrame = std::make_unique<CFrameBuffer>();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	if (m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION)
	{
		m_pPrevFrame = std::make_unique<CFrameBuffer>();
		for (int i = 0; i < GetNumPlanes(); i++)
			m_pPrevFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);
	}

	m_ptm = std::make_unique<CThreadManager>();

	return 0;
}

int CUM00Codec::InternalDecodeEnd(void)
{

	if (m_siDecode.siFlags & SI_FLAGS_USE_CONTROL_COMPRESSION)
	{
		for (int i = 0; i < GetNumPlanes(); i++)
			for (unsigned int j = 0; j < m_dwDivideCount; ++j)
				free(m_pTmpControlStream[i][j]);
	}

	m_pCurFrame.reset();

	m_ptm.reset();

	return 0;
}

size_t CUM00Codec::DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth)
{
	int ret;

	ret = CalcRawFrameMetric(outfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return 0;

	return m_cbRawSize;
}

int CUM00Codec::InternalDecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData)
{
	if (width % GetMacroPixelWidth() != 0 || height % GetMacroPixelHeight() != 0)
		return -1;

	if (!outfmt)
		return 0;

	STREAMINFO si;

	memset(&si, 0, sizeof(STREAMINFO));
	memcpy(&si, pExtraData, min(sizeof(STREAMINFO), cbExtraData));

	if (sizeof(STREAMINFO) > cbExtraData && is_not_all_zero((uint8_t*)pExtraData + sizeof(STREAMINFO), (uint8_t*)pExtraData + cbExtraData))
	{
		return -1;
	}

	if (si.siEncodingMode != SI_ENCODING_MODE_8SYMPACK)
		return -1;
	if ((si.siFlags & SI_FLAGS_RESERVED) != 0)
		return -1;

	if (is_not_all_zero(si.siReserved))
		return -1;

	for (const utvf_t *utvf = GetDecoderOutputFormat(); *utvf; utvf++)
	{
		if (outfmt == *utvf)
			return 0;
	}

	return -1;
}

void CUM00Codec::DecodeProc(uint32_t nBandIndex)
{
	if (!(m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION))
	{
		if (DecodeDirect(nBandIndex))
			return;
	}

	uint8_t* pDstBegin[4];
	const uint8_t* pPrevBegin[4];
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
		pDstBegin[nPlaneIndex] = m_pCurFrame->GetPlane(nPlaneIndex);
	if (m_siDecode.siFlags & SI_FLAGS_USE_TEMPORAL_COMPRESSION)
	{
		for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
			pPrevBegin[nPlaneIndex] = m_pPrevFrame->GetPlane(nPlaneIndex);
	}
	DecodeToPlanar(nBandIndex, pDstBegin, pPrevBegin);

	ConvertFromPlanar(nBandIndex);
}

void CUM00Codec::DecodeToPlanar(uint32_t nBandIndex, uint8_t* const* pDstBegin, const uint8_t* const* pPrevBegin)
{
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwPlaneStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		uint8_t *pControl;
		if (m_fiDecode->fiFlags & FI_FLAGS_CONTROL_COMPRESSED)
		{
			LZ4_decompress_safe((char*)m_pControlStream[nPlaneIndex][nBandIndex], (char*)m_pTmpControlStream[nPlaneIndex][nBandIndex], m_cbControlStream[nPlaneIndex][nBandIndex], (cbPlaneEnd - cbPlaneBegin) / 64 * 4);
			pControl = m_pTmpControlStream[nPlaneIndex][nBandIndex];
		}
		else
		{
			pControl = m_pControlStream[nPlaneIndex][nBandIndex];
		}

		if (m_fiDecode->fiFrameType == FI_FRAME_TYPE_INTRA)
		{
			Unpack8SymAndRestorePlanarGradient8(
				pDstBegin[nPlaneIndex] + cbPlaneBegin,
				pDstBegin[nPlaneIndex] + cbPlaneEnd,
				m_pPackedStream[nPlaneIndex][nBandIndex], pControl,
				m_cbPlanePredictStride[nPlaneIndex]);
		}
		else
		{
			Unpack8SymWithDiff8(
				pDstBegin[nPlaneIndex] + cbPlaneBegin,
				pDstBegin[nPlaneIndex] + cbPlaneEnd,
				m_pPackedStream[nPlaneIndex][nBandIndex], pControl,
				pPrevBegin[nPlaneIndex] + cbPlaneBegin,
				m_cbPlanePredictStride[nPlaneIndex]);
		}
	}
}

bool CUM00Codec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
