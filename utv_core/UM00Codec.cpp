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

CUM00Codec::CUM00Codec(const char *pszTinyName, const char *pszInterfaceName) : CCodecBase(pszTinyName, pszInterfaceName)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));
	m_ec.ecFlags = EC_FLAGS_DIVIDE_COUNT_AUTO;
	m_ec.ecDivideCountMinusOne = CThreadManager::GetNumProcessors() - 1;

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
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			memset(&pThis->m_ec, 0, sizeof(ENCODERCONF));
			if (IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_AUTO))
			{
				pThis->m_ec.ecFlags |= EC_FLAGS_DIVIDE_COUNT_AUTO;
				pThis->m_ec.ecDivideCountMinusOne = CThreadManager::GetNumProcessors() - 1;
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
				pThis->m_ec.ecDivideCountMinusOne = n - 1;
			}
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

int CUM00Codec::InternalSetState(const void *pState, size_t cb)
{
	ENCODERCONF ec;

	memset(&ec, 0, sizeof(ENCODERCONF));
	memcpy(&ec, pState, min(sizeof(ENCODERCONF), cb));

	if ((ec.ecFlags & EC_FLAGS_RESERVED) != 0)
		return -1;
	if (ec.ecReserved[0] != 0)
		return -1;
	if (ec.ecReserved[1] != 0)
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
	fi->fiFrameType = FI_FRAME_TYPE_INTRA;
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

	*pbKeyFrame = true;
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
	CalcStripeMetric();

	m_pCurFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_pPredicted = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pPredicted->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_ptm = new CThreadManager();

	for (int i = 0; i < GetNumPlanes(); i++)
	{
		for (unsigned int j = 0; j < m_dwDivideCount; ++j)
		{
			m_pPackedStream[i][j] = (uint8_t*)malloc(m_cbPlaneStripeSize[i] * (m_dwPlaneStripeEnd[j] - m_dwPlaneStripeBegin[j]));
			m_pControlStream[i][j] = (uint8_t*)malloc(m_cbPlaneStripeSize[i] * (m_dwPlaneStripeEnd[j] - m_dwPlaneStripeBegin[j]) / 64 * 3 + 8);
		}
	}

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
		}
	}

	delete m_pCurFrame;
	delete m_pPredicted;

	delete m_ptm;

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

	return 0;
}

size_t CUM00Codec::EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height)
{
	return ROUNDUP(width, 128) * height * GetRealBitCount() / 64 * 67 / 8 + 4096; // +4096 ÇÕÇ«ÇÒÇ‘ÇËä®íËÅB
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

void CUM00Codec::EncodeProc(uint32_t nBandIndex)
{
	ConvertToPlanar(nBandIndex);

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwPlaneStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		Pack8SymAfterPredictPlanarGradient8(
			m_pPackedStream[nPlaneIndex][nBandIndex], &m_cbPackedStream[nPlaneIndex][nBandIndex],
			m_pControlStream[nPlaneIndex][nBandIndex],
			m_pCurFrame->GetPlane(nPlaneIndex) + cbPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + cbPlaneEnd,
			m_cbPlanePredictStride[nPlaneIndex]);
		m_cbControlStream[nPlaneIndex][nBandIndex] = (cbPlaneEnd - cbPlaneBegin) / 64 * 3;
	}
}

size_t CUM00Codec::DecodeFrame(void *pOutput, const void *pInput)
{
	m_pInput = pInput;
	m_pOutput = pOutput;

	const FRAMEINFO *fi = (const FRAMEINFO*)pInput;

	if (is_not_all_zero(fi->fiReserved))
		return m_cbRawSize;

	if (fi->fiFrameType != FI_FRAME_TYPE_INTRA)
		return m_cbRawSize;

	const uint8_t *pPackedStreams = (const uint8_t *)pInput + sizeof(FRAMEINFO);
	const uint32_t *pcbControlStreams = (const uint32_t*)(pPackedStreams + fi->fiSizeArrayOffset);
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

	STREAMINFO si;

	memset(&si, 0, sizeof(STREAMINFO));
	memcpy(&si, pExtraData, min(sizeof(STREAMINFO), cbExtraData));

	m_dwDivideCount = si.siDivideCountMinusOne + 1;
	CalcStripeMetric();

	m_utvfRaw = outfmt;
	m_nWidth = width;
	m_nHeight = height;

	m_pCurFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_pPredicted = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pPredicted->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_ptm = new CThreadManager();

	return 0;
}

int CUM00Codec::InternalDecodeEnd(void)
{
	delete m_pCurFrame;
	delete m_pPredicted;

	delete m_ptm;

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
	if (DecodeDirect(nBandIndex))
		return;

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwPlaneStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwPlaneStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		Unpack8SymAndRestorePlanarGradient8(
			m_pCurFrame->GetPlane(nPlaneIndex) + cbPlaneBegin,
			m_pCurFrame->GetPlane(nPlaneIndex) + cbPlaneEnd,
			m_pPackedStream[nPlaneIndex][nBandIndex], m_pControlStream[nPlaneIndex][nBandIndex],
			m_cbPlanePredictStride[nPlaneIndex]);
	}

	ConvertFromPlanar(nBandIndex);
}

bool CUM00Codec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}
