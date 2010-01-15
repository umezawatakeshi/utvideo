/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "PlanarDecoder.h"
#include "Predict.h"

CPlanarDecoder::CPlanarDecoder(void)
{
}

CPlanarDecoder::~CPlanarDecoder(void)
{
}

LRESULT CPlanarDecoder::Decompress(const ICDECOMPRESS *icd, SIZE_T cb)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)icd->lpbiInput;
	const BYTE *p;

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

LRESULT CPlanarDecoder::DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
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
		//m_dwRawSize = (pbihOut->biWidth * pbihOut->biHeight * 12) / 8; // XXX •‚â‚‚³‚ªŠï”‚Ìê‡‚Íl—¶‚µ‚Ä‚¢‚È‚¢
		m_dwRawSize = (pbihOut->biWidth * pbihOut->biHeight * 3) / 2; // XXX •‚â‚‚³‚ªŠï”‚Ìê‡‚Íl—¶‚µ‚Ä‚¢‚È‚¢
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

LRESULT CPlanarDecoder::DecompressEnd(void)
{
	delete m_pRestoredFrame;
	delete m_pDecodedFrame;

	delete m_ptm;

	return ICERR_OK;
}

LRESULT CPlanarDecoder::DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	if (pbihOut == NULL)
		return sizeof(BITMAPINFOHEADER);

	memset(pbihOut, 0, sizeof(BITMAPINFOHEADER));

	pbihOut->biSize          = sizeof(BITMAPINFOHEADER);
	pbihOut->biWidth         = pbihIn->biWidth;
	pbihOut->biHeight        = pbihIn->biHeight;
	pbihOut->biPlanes        = 1;
	pbihOut->biBitCount      = GetSupportedOutputFormats()[0].nBitCount;
	pbihOut->biCompression   = GetSupportedOutputFormats()[0].fcc;
	pbihOut->biSizeImage     = pbihIn->biSizeImage;
	//pbihOut->biXPelsPerMeter
	//pbihOut->biYPelsPerMeter
	//pbihOut->biClrUsed
	//pbihOut->biClrImportant

	return ICERR_OK;
}

LRESULT CPlanarDecoder::DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieIn = (BITMAPINFOEXT *)pbihIn;

	const OUTPUTFORMAT *pfmts;

	if (pbihIn->biCompression != GetInputFCC())
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

	pfmts = GetSupportedOutputFormats();
	for (int i = 0; i < GetNumSupportedOutputFormats(); i++)
	{
		if (pbihOut->biCompression == pfmts[i].fcc && pbihOut->biBitCount == pfmts[i].nBitCount && pbihOut->biHeight > 0 && pbihOut->biHeight == pbihIn->biHeight && pbihOut->biWidth == pbihIn->biWidth)
			return ICERR_OK;
	}

	return ICERR_BADFORMAT;
}

void CPlanarDecoder::DecodeProc(DWORD nBandIndex)
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
