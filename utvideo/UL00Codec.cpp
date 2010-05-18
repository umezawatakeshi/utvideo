/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "UL00Codec.h"
#include "Predict.h"
#include "resource.h"

CUL00Codec::CUL00Codec(const char *pszInterfaceName)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));
	m_ec.dwFlags0 = (CThreadManager::GetNumProcessors() - 1) | EC_FLAGS0_INTRAFRAME_PREDICT_LEFT;
	m_pszInterfaceName = pszInterfaceName;
}

CUL00Codec::~CUL00Codec(void)
{
}

void CUL00Codec::GetShortFriendlyName(char *pszName, size_t cchName)
{
	char buf[16];
	DWORD fcc = GetFCC();

	sprintf(buf, "Ut Video (%c%c%c%c)", FCC4PRINTF(fcc));
	strncpy(pszName, buf, cchName);
	pszName[cchName - 1] = '\0';
}

void CUL00Codec::GetShortFriendlyName(wchar_t *pszName, size_t cchName)
{
	char buf[16];
	char *p;

	// 名前には us-ascii な文字しか入らないので、数値代入してしまう。
	GetShortFriendlyName(buf, min(cchName, _countof(buf)));
	p = buf;
	while ((*(pszName++) = *(p++)) != '\0')
		/* NOTHING */;
}

void CUL00Codec::GetLongFriendlyName(char *pszName, size_t cchName)
{
	char buf[128];
	DWORD fcc = GetFCC();

	sprintf(buf, "Ut Video Codec %s (%c%c%c%c) %s %s",
		GetColorFormatName(),
		FCC4PRINTF(fcc),
		m_pszInterfaceName,
		UTVIDEO_IMPLEMENTATION_STR);
	strncpy(pszName, buf, cchName);
	pszName[cchName - 1] = '\0';
}

void CUL00Codec::GetLongFriendlyName(wchar_t *pszName, size_t cchName)
{
	char buf[128];
	char *p;

	// 名前には us-ascii な文字しか入らないので、数値代入してしまう。
	GetLongFriendlyName(buf, min(cchName, _countof(buf)));
	p = buf;
	while ((*(pszName++) = *(p++)) != '\0')
		/* NOTHING */;
}

BOOL CUL00Codec::IsTemporalCompressionSupported(void)
{
	return FALSE;
}

LRESULT CUL00Codec::Configure(HWND hwnd)
{
	DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_CONFIG_DIALOG), hwnd, DialogProc, (LPARAM)this);
	return ICERR_OK;
}

INT_PTR CALLBACK CUL00Codec::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 一旦 LONG_PTR にキャストするのは、/Wp64 環境下で誤って 警告C4312 が発生するため。
	CUL00Codec *pThis = (CUL00Codec *)(LONG_PTR)GetWindowLongPtr(hwnd, DWLP_USER);
	char buf[256];
	int	n;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		// 一旦 __int3264 にキャストするのは、/Wp64 環境下で誤って 警告C4244 が発生するため。
		// LONG_PTR にキャストするのではダメらしい。
		SetWindowLongPtr(hwnd, DWLP_USER, (__int3264)lParam);
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
		case EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
			CheckDlgButton(hwnd, IDC_INTRAFRAME_PREDICT_MEDIAN_RADIO, BST_CHECKED);
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
			else if (IsDlgButtonChecked(hwnd, IDC_INTRAFRAME_PREDICT_MEDIAN_RADIO))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;
			if (IsDlgButtonChecked(hwnd, IDC_ASSUME_INTERLACE_CHECK))
				pThis->m_ec.dwFlags0 |= EC_FLAGS0_ASSUME_INTERLACE;
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

LRESULT CUL00Codec::GetStateSize(void)
{
	return sizeof(ENCODERCONF);
}

LRESULT CUL00Codec::GetState(void *pState, SIZE_T cb)
{
	if (cb < sizeof(ENCODERCONF))
		return ICERR_BADSIZE;

	memcpy(pState, &m_ec, sizeof(ENCODERCONF));
	return ICERR_OK;
}

LRESULT CUL00Codec::SetState(const void *pState, SIZE_T cb)
{
	memset(&m_ec, 0, sizeof(ENCODERCONF));

	memcpy(&m_ec, pState, min(sizeof(ENCODERCONF), cb));
	m_ec.dwFlags0 &= ~EC_FLAGS0_RESERVED;
	if ((m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK) == EC_FLAGS0_INTRAFRAME_PREDICT_RESERVED)
		m_ec.dwFlags0 |= EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;
	if (m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_IS_NUM_PROCESSORS)
	{
		m_ec.dwFlags0 &= ~EC_FLAGS0_DIVIDE_COUNT_MASK;
		m_ec.dwFlags0 |= (CThreadManager::GetNumProcessors() - 1) & EC_FLAGS0_DIVIDE_COUNT_MASK;
	}
	return min(sizeof(ENCODERCONF), cb);
}

LRESULT CUL00Codec::Compress(const ICCOMPRESS *icc, SIZE_T cb)
{
	FRAMEINFO fi;
	BYTE *p;
	DWORD count[256];

	m_icc = icc;

	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');

	memset(&fi, 0, sizeof(FRAMEINFO));

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CPredictJob(this, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
	{
	case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
		fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_LEFT;
		break;
	case EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
		fi.dwFlags0 |= FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;
		break;
	default:
		_ASSERT(false);
	}

	p = (BYTE *)icc->lpOutput;

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		DWORD dwCurrentOffset;
		for (int i = 0; i < 256; i++)
			count[i] = 0;
		for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
			for (int i = 0; i < 256; i++)
				count[i] += m_counts[nBandIndex].dwCount[nPlaneIndex][i];
		m_pCodeLengthTable[nPlaneIndex] = p;
		GenerateHuffmanCodeLengthTable(m_pCodeLengthTable[nPlaneIndex], count);
		GenerateHuffmanEncodeTable(&m_het[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		p += 256;
		dwCurrentOffset = 0;
		for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		{
			DWORD dwBits;
			dwBits = 0;
			for (int i = 0; i < 256; i++)
				dwBits += m_pCodeLengthTable[nPlaneIndex][i] * m_counts[nBandIndex].dwCount[nPlaneIndex][i];
			dwCurrentOffset += ROUNDUP(dwBits, 32) / 8;
			*(DWORD *)p = dwCurrentOffset;
			p += 4;
		}
		p += dwCurrentOffset;
	}

	memcpy(p, &fi, sizeof(FRAMEINFO));
	p += sizeof(FRAMEINFO);

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CEncodeJob(this, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	// 念のため、サイズ値が変数に収まるかどうかのチェック。
	// 2GB を超えることなどまずあり得ないとは思うが…
	SIZE_T cbSizeImage = p - ((BYTE *)icc->lpOutput);
	if (cbSizeImage > 0x7fffffff) // BITMAPINFOHEADER::biSizeImage は DWORD なので最大値は 2^32-1 であるが、ここでは 2^31-1 で制限しておく。
	{
		return ICERR_MEMORY; // 返すエラーはこれでいいのだろうか？
	}

	icc->lpbiOutput->biSizeImage = (DWORD)cbSizeImage;
	*icc->lpdwFlags = AVIIF_KEYFRAME;

	return ICERR_OK;
}

LRESULT CUL00Codec::CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	LRESULT ret;
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;

	ret = CompressQuery(pbihIn, pbihOut);
	if (ret != ICERR_OK)
		return ret;

	m_dwDivideCount = ((pbieOut->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;
	m_bInterlace = pbieOut->dwFlags0 & BIE_FLAGS0_ASSUME_INTERLACE;
	m_dwNumStripes = pbihIn->biHeight / (GetMacroPixelHeight() * (m_bInterlace ? 2 : 1));

	_ASSERT(m_dwDivideCount >= 1 && m_dwDivideCount <= 256);

	m_bBottomUpFrame = FALSE;
	switch (pbihIn->biCompression)
	{
	case FCC('YV12'):
		//m_dwRawSize = (pbihIn->biWidth * pbihIn->biHeight * 12) / 8; // XXX 幅や高さが奇数の場合は考慮していない
		m_dwRawSize = (pbihIn->biWidth * pbihIn->biHeight * 3) / 2; // XXX 幅や高さが奇数の場合は考慮していない
		break;
	default:
		switch (pbihIn->biCompression)
		{
		case BI_RGB:
			switch (pbihIn->biBitCount)
			{
			case 24:
				m_dwRawNetWidth = pbihIn->biWidth * 3;
				m_dwRawGrossWidth = ROUNDUP(m_dwRawNetWidth, 4);
				break;
			case 32:
				m_dwRawNetWidth = pbihIn->biWidth * 4;
				m_dwRawGrossWidth = m_dwRawNetWidth;
				break;
			}
			if (pbihIn->biHeight > 0)
				m_bBottomUpFrame = TRUE;
			break;
		case FCC('YUY2'):
		case FCC('YUYV'):
		case FCC('YUNV'):
		case FCC('UYVY'):
		case FCC('UYNV'):
		case FCC('YVYU'):
		case FCC('VYUY'):
			m_dwRawNetWidth = ROUNDUP(pbihIn->biWidth, 2) * 2;
			m_dwRawGrossWidth = m_dwRawNetWidth;
			break;
		default:
			return ICERR_BADFORMAT;
		}
		m_dwRawSize = m_dwRawGrossWidth * pbihIn->biHeight;
		if (m_bInterlace)
			m_dwRawStripeSize = m_dwRawGrossWidth * GetMacroPixelHeight() * 2;
		else
			m_dwRawStripeSize = m_dwRawGrossWidth * GetMacroPixelHeight();
	}

	CalcPlaneSizes(pbihIn);

	if (m_bInterlace)
	{
		for (int i = 0; i < _countof(m_dwPlaneWidth); i++)
		{
			m_dwPlaneStripeSize[i]    *= 2;
			m_dwPlanePredictStride[i] *= 2;
		}
	}

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
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

	m_pCurFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pCurFrame->AddPlane(m_dwPlaneSize[i], m_dwPlaneWidth[i]);

	m_pMedianPredicted = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pMedianPredicted->AddPlane(m_dwPlaneSize[i], m_dwPlaneWidth[i]);

	m_counts = (COUNTS *)VirtualAlloc(NULL, sizeof(COUNTS) * m_dwDivideCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	m_ptm = new CThreadManager();

	return ICERR_OK;
}

LRESULT CUL00Codec::CompressEnd(void)
{
	delete m_pCurFrame;
	delete m_pMedianPredicted;

	VirtualFree(m_counts, 0, MEM_RELEASE);

	delete m_ptm;

	return ICERR_OK;
}

LRESULT CUL00Codec::CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;
	DWORD dwDivideCount;

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOEXT);

	memset(pbihOut, 0, sizeof(BITMAPINFOEXT));

	dwDivideCount = min(ROUNDUP(pbihIn->biHeight, 2) / 2, (m_ec.dwFlags0 & EC_FLAGS0_DIVIDE_COUNT_MASK) + 1);

	_ASSERT(dwDivideCount >= 1 && dwDivideCount <= 256);

	pbieOut->bih.biSize          = sizeof(BITMAPINFOEXT);
	pbieOut->bih.biWidth         = pbihIn->biWidth;
	pbieOut->bih.biHeight        = pbihIn->biHeight;
	pbieOut->bih.biPlanes        = 1;
	pbieOut->bih.biBitCount      = min(pbihIn->biBitCount, GetFalseBitCount());
	pbieOut->bih.biCompression   = GetFCC();
	pbieOut->bih.biSizeImage     = pbihIn->biSizeImage;
	//pbieOut->bih.biXPelsPerMeter
	//pbieOut->bih.biYPelsPerMeter
	//pbieOut->bih.biClrUsed
	//pbieOut->bih.biClrImportant
	pbieOut->dwEncoderVersionAndImplementation  = UTVIDEO_VERSION_AND_IMPLEMENTATION;
	pbieOut->fccOriginalFormat = pbihIn->biCompression;
	pbieOut->dwFrameInfoSize   = sizeof(FRAMEINFO);
	pbieOut->dwFlags0          = BIE_FLAGS0_COMPRESS_HUFFMAN_CODE | ((dwDivideCount - 1) << BIE_FLAGS0_DIVIDE_COUNT_SHIFT) | (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE ? BIE_FLAGS0_ASSUME_INTERLACE : 0);

	return ICERR_OK;
}

LRESULT CUL00Codec::CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	return ROUNDUP(pbihIn->biWidth, 4) * ROUNDUP(pbihIn->biHeight, 2) * GetRealBitCount() / 8 + 4096; // +4096 はどんぶり勘定。
}

LRESULT CUL00Codec::CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	if (pbihIn->biWidth % GetMacroPixelWidth() != 0 || pbihIn->biHeight % GetMacroPixelHeight() != 0)
		return ICERR_BADFORMAT;

	if (m_ec.dwFlags0 & EC_FLAGS0_ASSUME_INTERLACE && pbihIn->biHeight % (GetMacroPixelHeight() * 2) != 0)
		return ICERR_BADFORMAT;

	for (const FORMATINFO *pfi = GetEncoderInputFormat(); !IS_FORMATINFO_END(pfi); pfi++)
	{
		if (pbihIn->biCompression == pfi->fcc && pbihIn->biBitCount == pfi->nBitCount && pbihIn->biHeight > 0)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}

void CUL00Codec::PredictProc(DWORD nBandIndex)
{
	ConvertToPlanar(nBandIndex);

	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		DWORD dwPlaneBegin = m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[nPlaneIndex];
		DWORD dwPlaneEnd   = m_dwPlaneStripeEnd[nBandIndex]   * m_dwPlaneStripeSize[nPlaneIndex];

		for (int i = 0; i < 256; i++)
			m_counts[nBandIndex].dwCount[nPlaneIndex][i] = 0;

		switch (m_ec.dwFlags0 & EC_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		case EC_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			PredictLeftAndCount(m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_counts[nBandIndex].dwCount[nPlaneIndex]);
			break;
		case EC_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
			PredictMedianAndCount(m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pCurFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_dwPlanePredictStride[nPlaneIndex], m_counts[nBandIndex].dwCount[nPlaneIndex]);
			break;
		default:
			_ASSERT(false);
		}
	}
}

void CUL00Codec::EncodeProc(DWORD nBandIndex)
{
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		DWORD dwPlaneBegin = m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[nPlaneIndex];
		DWORD dwPlaneEnd   = m_dwPlaneStripeEnd[nBandIndex]   * m_dwPlaneStripeSize[nPlaneIndex];

		DWORD dwDstOffset;
#ifdef _DEBUG
		DWORD dwDstEnd;
		DWORD dwEncodedSize;
#endif

		if (nBandIndex == 0)
			dwDstOffset = 0;
		else
			dwDstOffset = ((DWORD *)(m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex - 1];
#ifdef _DEBUG
		dwDstEnd = ((DWORD *)(m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex];
		dwEncodedSize =
#endif
		HuffmanEncode(m_pCodeLengthTable[nPlaneIndex] + 256 + sizeof(DWORD) * m_dwDivideCount + dwDstOffset, m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pMedianPredicted->GetPlane(nPlaneIndex) + dwPlaneEnd, &m_het[nPlaneIndex]);
		_ASSERT(dwEncodedSize == dwDstEnd - dwDstOffset);
	}
}

LRESULT CUL00Codec::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)icd->lpbiInput;
	/* const */ BYTE *p;

	m_icd = icd;

	memset(&m_fi, 0, sizeof(FRAMEINFO));
	memcpy(&m_fi, ((BYTE *)icd->lpInput) + pbieIn->bih.biSizeImage - pbieIn->dwFrameInfoSize, pbieIn->dwFrameInfoSize);

	p = (BYTE *)icd->lpInput;
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		m_pCodeLengthTable[nPlaneIndex] = p;
		GenerateHuffmanDecodeTable(&m_hdt[nPlaneIndex], m_pCodeLengthTable[nPlaneIndex]);
		p += 256 + sizeof(DWORD) * m_dwDivideCount;
		p += ((const DWORD *)p)[-1];
	}

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
		m_ptm->SubmitJob(new CDecodeJob(this, nBandIndex), nBandIndex);
	m_ptm->WaitForJobCompletion();

	icd->lpbiOutput->biSizeImage = m_dwRawSize;

	return ICERR_OK;
}

LRESULT CUL00Codec::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	LRESULT ret;
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	ret = DecompressQuery(pbihIn, pbihOut);
	if (ret != ICERR_OK)
		return ret;

	m_dwDivideCount = ((pbieIn->dwFlags0 & BIE_FLAGS0_DIVIDE_COUNT_MASK) >> BIE_FLAGS0_DIVIDE_COUNT_SHIFT) + 1;
	m_bInterlace = pbieIn->dwFlags0 & BIE_FLAGS0_ASSUME_INTERLACE;
	m_dwNumStripes = pbihIn->biHeight / (GetMacroPixelHeight() * (m_bInterlace ? 2 : 1));

	_ASSERT(m_dwDivideCount >= 1 && m_dwDivideCount <= 256);
	_RPT1(_CRT_WARN, "divide count = %d\n", m_dwDivideCount);

	m_bBottomUpFrame = FALSE;
	switch (pbihOut->biCompression)
	{
	case FCC('YV12'):
		//m_dwRawSize = (pbihOut->biWidth * pbihOut->biHeight * 12) / 8; // XXX 幅や高さが奇数の場合は考慮していない
		m_dwRawSize = (pbihOut->biWidth * pbihOut->biHeight * 3) / 2; // XXX 幅や高さが奇数の場合は考慮していない
		break;
	default:
		switch (pbihOut->biCompression)
		{
		case BI_RGB:
			switch (pbihOut->biBitCount)
			{
			case 24:
				m_dwRawNetWidth = pbihOut->biWidth * 3;
				m_dwRawGrossWidth = ROUNDUP(m_dwRawNetWidth, 4);
				break;
			case 32:
				m_dwRawNetWidth = pbihOut->biWidth * 4;
				m_dwRawGrossWidth = m_dwRawNetWidth;
				break;
			}
			if (pbihOut->biHeight > 0)
				m_bBottomUpFrame = TRUE;
			break;
		case FCC('YUY2'):
		case FCC('YUYV'):
		case FCC('YUNV'):
		case FCC('UYVY'):
		case FCC('UYNV'):
		case FCC('YVYU'):
		case FCC('VYUY'):
			m_dwRawNetWidth = ROUNDUP(pbihOut->biWidth, 2) * 2;
			m_dwRawGrossWidth = m_dwRawNetWidth;
			break;
		default:
			return ICERR_BADFORMAT;
		}
		m_dwRawSize = m_dwRawGrossWidth * pbihOut->biHeight;
		if (m_bInterlace)
			m_dwRawStripeSize = m_dwRawGrossWidth * GetMacroPixelHeight() * 2;
		else
			m_dwRawStripeSize = m_dwRawGrossWidth * GetMacroPixelHeight();
	}

	CalcPlaneSizes(pbihOut);

	if (m_bInterlace)
	{
		for (int i = 0; i < _countof(m_dwPlaneWidth); i++)
		{
			m_dwPlaneStripeSize[i]    *= 2;
			m_dwPlanePredictStride[i] *= 2;
		}
	}

	for (DWORD nBandIndex = 0; nBandIndex < m_dwDivideCount; nBandIndex++)
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

	m_pRestoredFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pRestoredFrame->AddPlane(m_dwPlaneSize[i], m_dwPlaneWidth[i]);

	m_pDecodedFrame = new CFrameBuffer();
	for (int i = 0; i < GetNumPlanes(); i++)
		m_pDecodedFrame->AddPlane(m_dwPlaneSize[i], m_dwPlaneWidth[i]);

	m_ptm = new CThreadManager();

	return ICERR_OK;
}

LRESULT CUL00Codec::DecompressEnd(void)
{
	delete m_pRestoredFrame;
	delete m_pDecodedFrame;

	delete m_ptm;

	return ICERR_OK;
}

LRESULT CUL00Codec::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER);

	memset(pbihOut, 0, sizeof(BITMAPINFOHEADER));

	pbihOut->biSize          = sizeof(BITMAPINFOHEADER);
	pbihOut->biWidth         = pbihIn->biWidth;
	pbihOut->biHeight        = pbihIn->biHeight;
	pbihOut->biPlanes        = 1;
	pbihOut->biBitCount      = GetDecoderOutputFormat()->nBitCount;
	pbihOut->biCompression   = GetDecoderOutputFormat()->fcc;
	pbihOut->biSizeImage     = pbihIn->biSizeImage;
	//pbihOut->biXPelsPerMeter
	//pbihOut->biYPelsPerMeter
	//pbihOut->biClrUsed
	//pbihOut->biClrImportant

	return ICERR_OK;
}

LRESULT CUL00Codec::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	if (pbihIn->biCompression != GetFCC())
		return ICERR_BADFORMAT;

	if (pbihIn->biWidth % GetMacroPixelWidth() != 0 || pbihIn->biHeight % GetMacroPixelHeight() != 0)
		return ICERR_BADFORMAT;

	if (pbihIn->biSize > sizeof(BITMAPINFOEXT))
		return ICERR_BADFORMAT;

	if (pbieIn->dwFlags0 & BIE_FLAGS0_ASSUME_INTERLACE && pbihIn->biHeight % (GetMacroPixelHeight() * 2) != 0)
		return ICERR_BADFORMAT;

	if (pbieIn->dwFrameInfoSize > sizeof(FRAMEINFO))
		return ICERR_BADFORMAT;
	if (pbieIn->dwFlags0 & BIE_FLAGS0_RESERVED)
		return ICERR_BADFORMAT;

	if (pbihOut == NULL)
		return ICERR_OK;

	for (const FORMATINFO *pfi = GetDecoderOutputFormat(); !IS_FORMATINFO_END(pfi); pfi++)
	{
		if (pbihOut->biCompression == pfi->fcc && pbihOut->biBitCount == pfi->nBitCount && pbihOut->biHeight > 0 && pbihOut->biHeight == pbihIn->biHeight && pbihOut->biWidth == pbihIn->biWidth)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}

void CUL00Codec::DecodeProc(DWORD nBandIndex)
{
	for (int nPlaneIndex = 0; nPlaneIndex < GetNumPlanes(); nPlaneIndex++)
	{
		DWORD dwPlaneBegin = m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[nPlaneIndex];
		DWORD dwPlaneEnd   = m_dwPlaneStripeEnd[nBandIndex]   * m_dwPlaneStripeSize[nPlaneIndex];

		DWORD dwDstOffset;

		if (nBandIndex == 0)
			dwDstOffset = 0;
		else
			dwDstOffset = ((const DWORD *)(m_pCodeLengthTable[nPlaneIndex] + 256))[nBandIndex - 1];

		if ((m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK) == FI_FLAGS0_INTRAFRAME_PREDICT_LEFT)
			HuffmanDecodeAndAccum(m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_pCodeLengthTable[nPlaneIndex] + 256 + sizeof(DWORD) * m_dwDivideCount + dwDstOffset, &m_hdt[nPlaneIndex]);
		else
			HuffmanDecode(m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_pCodeLengthTable[nPlaneIndex] + 256 + sizeof(DWORD) * m_dwDivideCount + dwDstOffset, &m_hdt[nPlaneIndex]);

		switch (m_fi.dwFlags0 & FI_FLAGS0_INTRAFRAME_PREDICT_MASK)
		{
		case FI_FLAGS0_INTRAFRAME_PREDICT_NONE:
		case FI_FLAGS0_INTRAFRAME_PREDICT_LEFT:
			m_pCurFrame = m_pDecodedFrame;
			break;
		case FI_FLAGS0_INTRAFRAME_PREDICT_MEDIAN:
			RestoreMedian(m_pRestoredFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneBegin, m_pDecodedFrame->GetPlane(nPlaneIndex) + dwPlaneEnd, m_dwPlanePredictStride[nPlaneIndex]);
			m_pCurFrame = m_pRestoredFrame;
			break;
		}
	}

	ConvertFromPlanar(nBandIndex);
}
