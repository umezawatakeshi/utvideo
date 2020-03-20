/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "utv_core.h"
#include "UL00Codec.h"
#include "Predict.h"
#include "TunedFunc.h"
#include "resource.h"
#include "ByteOrder.h"

CUL00Codec::CUL00Codec(const char *pszTinyName, const char *pszInterfaceName) : CBandParallelCodec(pszTinyName, pszInterfaceName)
{
	SetDefaultState();
	LoadConfig();
}

void CUL00Codec::GetLongFriendlyName(char *pszName, size_t cchName)
{
	char buf[128];

	sprintf(buf, "UtVideo %s %s",
		GetColorFormatName(),
		m_pszInterfaceName);
	strncpy(pszName, buf, cchName);
	pszName[cchName - 1] = '\0';
}

#ifdef _WIN32
INT_PTR CUL00Codec::Configure(HWND hwnd)
{
	DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_UL00_CONFIG), hwnd, DialogProc, (LPARAM)this);
	return 0;
}

INT_PTR CALLBACK CUL00Codec::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CUL00Codec *pThis = (CUL00Codec *)GetWindowLongPtr(hwnd, DWLP_USER);
	char buf[256];
	int	n;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		pThis = (CUL00Codec *)lParam;
		pThis->GetLongFriendlyName(buf, _countof(buf));
		SetWindowText(hwnd, buf);
		wsprintf(buf, "%d", (pThis->m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_MASK) + 1);
		SetDlgItemText(hwnd, IDC_DIVIDE_COUNT_EDIT, buf);
		switch (pThis->m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		default:
			_ASSERT(false);
			/* FALLTHROUGH */
		case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_LEFT_RADIO, BST_CHECKED);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_GRADIENT_RADIO, BST_CHECKED);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_WRONG_MEDIAN_RADIO, BST_CHECKED);
			break;
		}
		CheckDlgButton(hwnd, IDC_ASSUME_INTERLACE_CHECK, pThis->m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE);
		if (pThis->m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_IS_NUM_PROCESSORS)
		{
			CheckDlgButton(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS, BST_CHECKED);
			EnableDlgItem(hwnd, IDC_DIVIDE_COUNT_EDIT, FALSE);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			memset(&pThis->m_ec, 0, sizeof(ENCODERCONF));
			if (IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS))
			{
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_DIVIDE_COUNT_IS_NUM_PROCESSORS;
				pThis->m_ec.dwFlags0 |= (CThreadManager::GetNumProcessors() - 1) & EC_FLAGS0_DIVIDE_COUNT_MASK;
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
				pThis->m_ec.dwFlags0 |= (n - 1) & EC_FLAGS0_DIVIDE_COUNT_MASK;
			}
			if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_LEFT_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_LEFT;
			else if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_GRADIENT_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT;
			else if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_WRONG_MEDIAN_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN;
			if (IsDlgButtonChecked(hwnd, IDC_ASSUME_INTERLACE_CHECK))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_ASSUME_INTERLACE;
			pThis->SaveConfig();
			/* FALLTHROUGH */
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		case IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				EnableDlgItem(hwnd, IDC_DIVIDE_COUNT_EDIT, !IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS));
				if (IsDlgButtonChecked(hwnd, IDC_DIVIDE_COUNT_IS_NUM_PROCESSORS))
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

size_t CUL00Codec::GetStateSize(void)
{
	return sizeof(ENCODERCONF);
}

int CUL00Codec::GetState(void *pState, size_t cb)
{
	if (cb < sizeof(ENCODERCONF))
		return -1;

	memcpy(pState, &m_ec, sizeof(ENCODERCONF));
	return 0;
}

void CUL00Codec::SetDefaultState()
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));
	m_ec.dwFlags0 = (CThreadManager::GetNumProcessors() - 1) | EC_FLAGS0_INTRAFRAME_PREDICT_LEFT;
}

int CUL00Codec::InternalSetState(const void *pState, size_t cb)
{
	if (pState == NULL)
	{
		SetDefaultState();
		return sizeof(ENCODERCONF);
	}

	ENCODERCONF ec;

	memset(&ec, 0, sizeof(ENCODERCONF));
	memcpy(&ec, pState, min(sizeof(ENCODERCONF), cb));

	if ((ec.dwFlags0 & EC_FLAGS0_RESERVED) != 0)
		return -1;

	memcpy(&m_ec, &ec, sizeof(ENCODERCONF));

	if ((m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK) == EC_FLAGS0_INTRAFRAME_PREDICT_RESERVED)
		m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN;
	if (m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_IS_NUM_PROCESSORS)
	{
		m_ec.dwFlags0 &= ~EC_FLAGS0_DIVIDE_COUNT_MASK;
		m_ec.dwFlags0 |= (CThreadManager::GetNumProcessors() - 1) & EC_FLAGS0_DIVIDE_COUNT_MASK;
	}

	return 0;
}

size_t CUL00Codec::EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput)
{
	FRAMEINFO fi;
	uint8_t *p;
	uint32_t count[256];

	m_pInput = pInput;
	m_pOutput = pOutput;

	memset(&fi, 0, sizeof(FRAMEINFO));

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUL00Codec::PredictProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_LEFT;
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
		fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT;
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
		fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN;
		break;
	default:
		_ASSERT(false);
	}

	p = (uint8_t *)pOutput;

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		uint32_t dwCurrentOffset;
		for (int i = 0; i < 256; i++)
			count[i] = 0;
		for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
			for (int i = 0; i < 256; i++)
				count[i] += m_counts[nBandIndex].dwCount[nPlaneIndex][0][i];
		m_pCodeLengthTable[nPlaneIndex] = (HUFFMAN_CODELEN_TABLE<8> *)p;
		GenerateHuffmanCodeLengthTable<8>(m_pCodeLengthTable[nPlaneIndex], count);
		GenerateHuffmanEncodeTable(&m_het[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		p += 256;
		dwCurrentOffset = 0;
		for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		{
			uint32_t dwBits;
			dwBits = 0;
			for (int i = 0; i < 256; i++)
				dwBits += m_pCodeLengthTable[nPlaneIndex]->codelen[i] * m_counts[nBandIndex].dwCount[nPlaneIndex][0][i];
			dwCurrentOffset += ROUNDUP(dwBits, 32) / 8;
			*(uint32_t *)p = dwCurrentOffset;
			p += 4;
		}
		p += dwCurrentOffset;
	}

	memcpy(p, &fi, sizeof(FRAMEINFO));
	p += sizeof(FRAMEINFO);

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUL00Codec::EncodeProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	*pbKeyFrame = true;

	return p - ((uint8_t *)pOutput);
}

void CUL00Codec::ExtractFromExtraData(const void* pExtraData, size_t cbExtraData)
{
	const EXTRADATA* p = (const EXTRADATA*)pExtraData;

	m_dwDivideCount = ((p->flags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;
	m_bInterlace = (p->flags0 & BIE_FLAGS0_ASSUME_INTERLACE) != 0;
}

int CUL00Codec::CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	int ret;
	ret = CBandParallelCodec::CalcFrameMetric(rawfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return ret;

	int nStripeScale = GetMacroPixelHeight() / m_fmRaw.nMacroPixelHeight;
	for (int i = 0; i < m_fmRaw.nPlanes; ++i)
		m_fmRaw.nStripeLines[i] *= nStripeScale;

	if (m_bInterlace)
	{
		m_dwNumStripes /= 2;
		for (int i = 0; i < m_fmRaw.nPlanes; ++i)
			m_fmRaw.nStripeLines[i] *= 2;

		for (int i = 0; i < _countof(m_cbPlaneWidth); i++)
		{
			m_cbPlaneStripeSize[i]    *= 2;
			m_cbPlanePredictStride[i] *= 2;
		}
	}

	::FixStripeValues(&m_fmRaw);

	return 0;
}

int CUL00Codec::InternalEncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	int ret;
	EXTRADATA ed;

	ret = EncodeQuery(infmt, width, height);
	if (ret != 0)
		return ret;

	m_utvfRaw = infmt;
	m_nWidth = width;
	m_nHeight = height;

	EncodeGetExtraData(&ed, sizeof(ed), infmt, width, height);
	ExtractFromExtraData(&ed, sizeof(ed));
	ret = CalcFrameMetric(infmt, width, height, cbGrossWidth);
	if (ret != 0)
		return ret;
	CalcBandMetric();

	m_pCurFrame = std::make_unique<CFrameBuffer>();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_pPredicted = std::make_unique<CFrameBuffer>();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pPredicted->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

#ifdef _WIN32
	m_counts = (COUNTS *)VirtualAlloc(NULL, sizeof(COUNTS) * m_dwDivideCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif
#if defined(__APPLE__) || defined(__unix__)
	m_counts = (COUNTS *)mmap(NULL, sizeof(COUNTS) * m_dwDivideCount, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
#endif

	m_ptm = std::make_unique<CThreadManager>();

	return 0;
}

int CUL00Codec::InternalEncodeEnd(void)
{
	m_pCurFrame.reset();
	m_pPredicted.reset();

#ifdef _WIN32
	VirtualFree(m_counts, 0, MEM_RELEASE);
#endif
#if defined(__APPLE__) || defined(__unix__)
	munmap(m_counts, sizeof(COUNTS) * m_dwDivideCount);
#endif

	m_ptm.reset();

	return 0;
}

size_t CUL00Codec::EncodeGetExtraDataSize(void)
{
	return sizeof(EXTRADATA);
}

int CUL00Codec::EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height)
{
	EXTRADATA *p = (EXTRADATA *)pExtraData;
	unsigned int nDivideCount;

	if (cb < sizeof(EXTRADATA))
		return -1;

	memset(p, 0, cb);

	nDivideCount = min(ROUNDUP(height, 2) / 2, (m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_MASK) + 1);

	p->EncoderVersionAndImplementation = UTVIDEO_VERSION_AND_IMPLEMENTATION;
	p->fccOriginalFormat               = htob32(infmt);
	p->cbFrameInfo                     = sizeof(FRAMEINFO);
	p->flags0                          = BIE_FLAGS0_COMPRESS_HUFFMAN_CODE | ((nDivideCount - 1) << BIE_FLAGS0_DIVIDE_COUNT_SHIFT) | (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE ? BIE_FLAGS0_ASSUME_INTERLACE : 0);

	return 0;
}

size_t CUL00Codec::EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height)
{
	return ROUNDUP(width, 4) * ROUNDUP(height, 2) * GetRealBitCount() / 8 + 4096; // +4096 はどんぶり勘定。
}

int CUL00Codec::InternalEncodeQuery(utvf_t infmt, unsigned int width, unsigned int height)
{
	if (width % GetMacroPixelWidth() != 0 || height % GetMacroPixelHeight() != 0)
		return -1;

	if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE && height % (GetMacroPixelHeight() * 2) != 0)
		return -1;

	for (const utvf_t *utvf = GetEncoderInputFormat(); *utvf; utvf++)
	{
		if (infmt == *utvf)
			return 0;
	}

	return -1;
}

void CUL00Codec::PredictProc(uint32_t nBandIndex)
{
	memset(&m_counts[nBandIndex], 0, sizeof(m_counts[nBandIndex]));

	if (!PredictDirect(nBandIndex))
	{
		ConvertToPlanar(nBandIndex);

		const uint8_t* pSrcBegin[4];
		for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
			pSrcBegin[nPlaneIndex] = m_pCurFrame->GetPlane(nPlaneIndex);
		PredictFromPlanar(nBandIndex, pSrcBegin);
	}

	// 理想的にはここベクトル化してほしいんだけどどうなるんだろう
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		for (int j = 1; j < NUM_COUNT_TABLES_PER_CHANNEL<8>; ++j)
			for (int i = 0; i < 256; i++)
				m_counts[nBandIndex].dwCount[nPlaneIndex][0][i] += m_counts[nBandIndex].dwCount[nPlaneIndex][j][i];
	}
}

void CUL00Codec::PredictFromPlanar(uint32_t nBandIndex, const uint8_t* const* pSrcBegin)
{
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			PredictCylindricalLeftAndCount8(m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneEnd, m_counts[nBandIndex].dwCount[nPlaneIndex]);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
			PredictPlanarGradientAndCount8(m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneEnd, m_cbPlanePredictStride[nPlaneIndex], m_counts[nBandIndex].dwCount[nPlaneIndex]);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
			PredictCylindricalWrongMedianAndCount8(m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneBegin, pSrcBegin[nPlaneIndex] + cbPlaneEnd, m_cbPlanePredictStride[nPlaneIndex], m_counts[nBandIndex].dwCount[nPlaneIndex]);
			break;
		default:
			_ASSERT(false);
		}
	}
}

bool CUL00Codec::PredictDirect(uint32_t nBandIndex)
{
	return false;
}

void CUL00Codec::EncodeProc(uint32_t nBandIndex)
{
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

		uint32_t dwDstOffset;
#ifdef _DEBUG
		uint32_t dwDstEnd;
		size_t dwEncodedSize;
#endif

		if (nBandIndex == 0)
			dwDstOffset = 0;
		else
			dwDstOffset = ((uint32_t *)((uint8_t *)m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex - 1];
#ifdef _DEBUG
		dwDstEnd = ((uint32_t *)((uint8_t *)m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex];
		dwEncodedSize =
#endif
		HuffmanEncode8((uint8_t *)m_pCodeLengthTable[nPlaneIndex] + 256 + sizeof(uint32_t) * m_dwDivideCount + dwDstOffset, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneEnd, &m_het[nPlaneIndex]);
		_ASSERT(dwEncodedSize == dwDstEnd - dwDstOffset);
	}
}

size_t CUL00Codec::DecodeFrame(void *pOutput, const void *pInput)
{
	/* const */ uint8_t *p;

	m_pInput = pInput;
	m_pOutput = pOutput;

	p = (uint8_t *)pInput;
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		p += 256 + sizeof(uint32_t) * m_dwDivideCount;
		p += ((const uint32_t *)p)[-1];
	}
	memset(&m_fi, 0, sizeof(FRAMEINFO));
	memcpy(&m_fi, p, m_ed.cbFrameInfo);

	p = (uint8_t *)pInput;
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		m_pCodeLengthTable[nPlaneIndex] = (HUFFMAN_CODELEN_TABLE<8> *)p;
		m_ptm->SubmitJob(new CThreadJob(this, &CUL00Codec::GenerateDecodeTableProc, nPlaneIndex), nPlaneIndex);
		p += 256 + sizeof(uint32_t) * m_dwDivideCount;
		for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		{
			uint32_t dwCodeOffset;

			if (nBandIndex == 0)
				dwCodeOffset = 0;
			else
				dwCodeOffset = ((const uint32_t *)((uint8_t *)m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex - 1];

			m_pDecodeCode[nPlaneIndex][nBandIndex] = p + dwCodeOffset;
		}
		p += ((const uint32_t *)p)[-1];
	}
	m_ptm->WaitForJobCompletion();

	for (uint32_t nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CThreadJob(this, &CUL00Codec::DecodeProc, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	return m_fmRaw.cbTotalSize;
}

void CUL00Codec::GenerateDecodeTableProc(uint32_t nPlaneIndex)
{
	GenerateHuffmanDecodeTable(&m_hdt[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
}

int CUL00Codec::DecodeGetFrameType(bool *pbKeyFrame, const void *pInput)
{
	*pbKeyFrame = true;
	return 0;
}

int CUL00Codec::InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth, const void *pExtraData, size_t cbExtraData)
{
	int ret;

	ret = DecodeQuery(outfmt, width, height, pExtraData, cbExtraData);
	if (ret != 0)
		return ret;

	ExtractFromExtraData(pExtraData, cbExtraData);
	ret = CalcFrameMetric(outfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return ret;
	CalcBandMetric();

	memset(&m_ed, 0, sizeof(m_ed));
	memcpy(&m_ed, pExtraData, min(sizeof(m_ed), cbExtraData));

	m_utvfRaw = outfmt;
	m_nWidth = width;
	m_nHeight = height;

	m_pCurFrame = std::make_unique<CFrameBuffer>();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_pPredicted = std::make_unique<CFrameBuffer>();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pPredicted->AddPlane(m_cbPlaneSize[i], m_cbPlaneWidth[i]);

	m_ptm = std::make_unique<CThreadManager>();

	return 0;
}

int CUL00Codec::InternalDecodeEnd(void)
{
	m_pCurFrame.reset();
	m_pPredicted.reset();

	m_ptm.reset();

	return 0;
}

size_t CUL00Codec::DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth)
{
	int ret;

	ret = CalcRawFrameMetric(outfmt, width, height, cbGrossWidth);
	if (ret != 0)
		return 0;

	return m_fmRaw.cbTotalSize;
}

int CUL00Codec::InternalDecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData)
{
	const EXTRADATA *p = (const EXTRADATA *)pExtraData;

	if (width % GetMacroPixelWidth() != 0 || height % GetMacroPixelHeight() != 0)
		return -1;

	if (cbExtraData > sizeof(EXTRADATA))
		return -1;

	if (p->flags0 & BIE_FLAGS0_ASSUME_INTERLACE && height % (GetMacroPixelHeight() * 2) != 0)
		return -1;

	if (p->cbFrameInfo > sizeof(FRAMEINFO))
		return -1;
	if (p->flags0 & BIE_FLAGS0_RESERVED)
		return -1;

	if (!outfmt)
		return 0;

	for (const utvf_t *utvf = GetDecoderOutputFormat(); *utvf; utvf++)
	{
		if (outfmt == *utvf)
			return 0;
	}

	return -1;
}

void CUL00Codec::DecodeProc(uint32_t nBandIndex)
{
	if (DecodeDirect(nBandIndex))
		return;

	if (IsDirectRestorable())
	{
		DecodeToPlanar(nBandIndex);

		if (RestoreDirect(nBandIndex))
			return;

		_ASSERT(false);
	}
	else
	{
		uint8_t* pDstBegin[4];
		for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
			pDstBegin[nPlaneIndex] = m_pCurFrame->GetPlane(nPlaneIndex);
		DecodeAndRestoreToPlanar(nBandIndex, pDstBegin);

		ConvertFromPlanar(nBandIndex);
	}
}

template<bool DoRestore>
void CUL00Codec::DecodeAndRestoreToPlanarImpl(uint32_t nBandIndex, uint8_t* const* pDstBegin)
{
	/*
	 * decode と restore をお仕着せの処理で行う場合、
	 * 全部の plane を decode してから全部の plane を restore するよりも、
	 * plane ごとに decode して restore した方が、メインメモリへのアクセスが最適化されて速い。
	 * （ULY0->YV12 4k マルチスレッドで 5% ぐらい違う）
	 */

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		size_t cbPlaneBegin = m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[nPlaneIndex];
		size_t cbPlaneEnd   = m_dwStripeEnd[nBandIndex]   * m_cbPlaneStripeSize[nPlaneIndex];

#ifdef _DEBUG
		uint8_t *pRetExpected = m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneEnd;
		uint8_t *pRetActual =
#endif
		HuffmanDecode<8>(m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneEnd, m_pDecodeCode[nPlaneIndex][nBandIndex], &m_hdt[nPlaneIndex]);
		_ASSERT(pRetActual == pRetExpected);

		if (DoRestore)
		{
			switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
			{
			case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
			case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
				RestoreCylindricalLeft8(pDstBegin[nPlaneIndex] + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneEnd);
				break;
			case FI_FLAGS0_INTRAFRAME_PREDICT_GRADIENT:
				RestorePlanarGradient8(pDstBegin[nPlaneIndex] + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneEnd, m_cbPlanePredictStride[nPlaneIndex]);
				break;
			case FI_FLAGS0_INTRAFRAME_PREDICT_WRONG_MEDIAN:
				RestoreCylindricalWrongMedian8(pDstBegin[nPlaneIndex] + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneBegin, m_pPredicted->GetPlane(nPlaneIndex) + cbPlaneEnd, m_cbPlanePredictStride[nPlaneIndex]);
				break;
			}
		}
	}
}

void CUL00Codec::DecodeToPlanar(uint32_t nBandIndex)
{
	DecodeAndRestoreToPlanarImpl<false>(nBandIndex, NULL);
}

void CUL00Codec::DecodeAndRestoreToPlanar(uint32_t nBandIndex, uint8_t* const* pDstBegin)
{
	DecodeAndRestoreToPlanarImpl<true>(nBandIndex, pDstBegin);
}

bool CUL00Codec::DecodeDirect(uint32_t nBandIndex)
{
	return false;
}

bool CUL00Codec::RestoreDirect(uint32_t nBandIndex)
{
	return false;
}

bool CUL00Codec::IsDirectRestorable()
{
	return false;
}
