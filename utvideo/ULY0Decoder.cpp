/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008-2009  UMEZAWA Takeshi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * 
 * Ut Video Codec Suite
 * Copyright (C) 2008-2009  梅澤 威志
 * 
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフ
 * トウェア財団によって発行された GNU 一般公衆利用許諾契約書(バージョ
 * ン2か、希望によってはそれ以降のバージョンのうちどれか)の定める条件
 * の下で再頒布または改変することができます。
 * 
 * このプログラムは有用であることを願って頒布されますが、*全くの無保
 * 証* です。商業可能性の保証や特定の目的への適合性は、言外に示された
 * ものも含め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご
 * 覧ください。
 * 
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を
 * 一部受け取ったはずです。もし受け取っていなければ、フリーソフトウェ
 * ア財団まで請求してください(宛先は the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)。
 */

#include "StdAfx.h"
#include "utvideo.h"
#include "ULY0Decoder.h"
#include "Predict.h"

const CULY0Decoder::OUTPUTFORMAT CULY0Decoder::m_outfmts[] = {
	{ FCC('YV12'), 12 },
	{ FCC('YUY2'), 16 }, { FCC('YUYV'), 16 }, { FCC('YUNV'), 16 },
	{ FCC('UYVY'), 16 }, { FCC('UYNV'), 16 },
	{ FCC('YVYU'), 16 },
	{ FCC('VYUY'), 16 },
	{ BI_RGB, 32 },
	{ BI_RGB, 24 },
};

CULY0Decoder::CULY0Decoder(void)
{
}

CULY0Decoder::~CULY0Decoder(void)
{
}

int CULY0Decoder::GetNumSupportedOutputFormats(void)
{
	return _countof(m_outfmts);
}

CDecoder *CULY0Decoder::CreateInstance(void)
{
	return new CULY0Decoder();
}

void CULY0Decoder::CalcPlaneSizes(const BITMAPINFOHEADER *pbih)
{
	m_dwPlaneSize[0]          = pbih->biWidth * pbih->biHeight;
	m_dwPlaneSize[1]          = pbih->biWidth * pbih->biHeight / 4;
	m_dwPlaneSize[2]          = pbih->biWidth * pbih->biHeight / 4;

	m_dwPlaneWidth[0]         = pbih->biWidth;
	m_dwPlaneWidth[1]         = pbih->biWidth / 2;
	m_dwPlaneWidth[2]         = pbih->biWidth / 2;

	m_dwPlaneStripeSize[0]    = pbih->biWidth * 2;
	m_dwPlaneStripeSize[1]    = pbih->biWidth / 2;
	m_dwPlaneStripeSize[2]    = pbih->biWidth / 2;

	m_dwPlanePredictStride[0] = pbih->biWidth;
	m_dwPlanePredictStride[1] = pbih->biWidth / 2;
	m_dwPlanePredictStride[2] = pbih->biWidth / 2;
}

void CULY0Decoder::ConvertFromPlanar(DWORD nBandIndex)
{
	const BYTE *pSrcYBegin = m_pCurFrame->GetPlane(0) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
	const BYTE *pSrcUBegin = m_pCurFrame->GetPlane(1) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
	const BYTE *pSrcVBegin = m_pCurFrame->GetPlane(2) + m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

	switch (m_icd->lpbiOutput->biCompression)
	{
	case FCC('YV12'):
		{
			BYTE *pDstYBegin, *pDstVBegin, *pDstUBegin;

			pDstYBegin = ((BYTE *)m_icd->lpOutput);
			pDstVBegin = pDstYBegin + m_icd->lpbiOutput->biWidth * m_icd->lpbiOutput->biHeight;
			pDstUBegin = pDstVBegin + m_icd->lpbiOutput->biWidth * m_icd->lpbiOutput->biHeight / 4;

			pDstYBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[0];
			pDstVBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[1];
			pDstUBegin += m_dwPlaneStripeBegin[nBandIndex] * m_dwPlaneStripeSize[2];

			memcpy(pDstYBegin, pSrcYBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[0]);
			memcpy(pDstUBegin, pSrcUBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[1]);
			memcpy(pDstVBegin, pSrcVBegin, (m_dwPlaneStripeEnd[nBandIndex] - m_dwPlaneStripeBegin[nBandIndex]) * m_dwPlaneStripeSize[2]);
		}
		break;
	default:
		{
			const BYTE *y, *u, *v;
			BYTE *pDstBegin, *pDstEnd;

			y = pSrcYBegin;
			u = pSrcUBegin;
			v = pSrcVBegin;

			switch (m_icd->lpbiOutput->biCompression)
			{
			case FCC('YVYU'):
				swap(pSrcUBegin, pSrcVBegin);
				swap(u, v);
				/* FALLTHROUGH */
			case FCC('YUY2'):
			case FCC('YUYV'):
			case FCC('YUNV'):
				{
					DWORD dwDstStride;
					DWORD dwYStride;

					dwDstStride = m_icd->lpbiOutput->biWidth * 2;
					dwYStride = m_icd->lpbiOutput->biWidth;

					pDstBegin = ((BYTE *)m_icd->lpOutput) + m_dwPlaneStripeBegin[nBandIndex] * dwDstStride * 2;
					pDstEnd   = ((BYTE *)m_icd->lpOutput) + m_dwPlaneStripeEnd[nBandIndex]   * dwDstStride * 2;

					for (BYTE *pStrideBegin = pDstBegin; pStrideBegin < pDstEnd; pStrideBegin += dwDstStride * 2)
					{
						BYTE *pStrideEnd = pStrideBegin + dwDstStride;
						for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
						{
							BYTE *q = p + dwDstStride;
							*p = *(y+0);
							*(p+2) = *(y+1);
							*q = *(y+dwYStride+0);
							*(q+2) = *(y+dwYStride+1);
							*(p+1) = *u;
							*(q+1) = *u;
							*(p+3) = *v;
							*(q+3) = *v;

							y+=2; u++; v++;
						}
						y += dwYStride;
					}
				}
				break;
			case FCC('VYUY'):
				swap(pSrcUBegin, pSrcVBegin);
				swap(u, v);
				/* FALLTHROUGH */
			case FCC('UYVY'):
			case FCC('UYNV'):
				{
					DWORD dwDstStride;
					DWORD dwYStride;

					dwDstStride = m_icd->lpbiOutput->biWidth * 2;
					dwYStride = m_icd->lpbiOutput->biWidth;

					pDstBegin = ((BYTE *)m_icd->lpOutput) + m_dwPlaneStripeBegin[nBandIndex] * dwDstStride * 2;
					pDstEnd   = ((BYTE *)m_icd->lpOutput) + m_dwPlaneStripeEnd[nBandIndex]   * dwDstStride * 2;

					for (BYTE *pStrideBegin = pDstBegin; pStrideBegin < pDstEnd; pStrideBegin += dwDstStride * 2)
					{
						BYTE *pStrideEnd = pStrideBegin + dwDstStride;
						for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 4)
						{
							BYTE *q = p + dwDstStride;
							*(p+1) = *(y+0);
							*(p+3) = *(y+1);
							*(q+1) = *(y+dwYStride+0);
							*(q+3) = *(y+dwYStride+1);
							*(p+0) = *u;
							*(q+0) = *u;
							*(p+2) = *v;
							*(q+2) = *v;

							y+=2; u++; v++;
						}
						y += dwYStride;
					}
				}
				break;
			case BI_RGB:
				switch (m_icd->lpbiOutput->biBitCount)
				{
				case 24:
					{
						DWORD dwDstStride;
						DWORD dwYStride;
						DWORD dwDataStride;

						dwDataStride = m_icd->lpbiOutput->biWidth * 3;
						dwDstStride = ROUNDUP(dwDataStride, 4);
						dwYStride = m_icd->lpbiOutput->biWidth;

						pDstBegin = ((BYTE *)m_icd->lpOutput) + (m_dwNumStripes - m_dwPlaneStripeEnd[nBandIndex]  ) * dwDstStride * 2;
						pDstEnd   = ((BYTE *)m_icd->lpOutput) + (m_dwNumStripes - m_dwPlaneStripeBegin[nBandIndex]) * dwDstStride * 2;

						for (BYTE *pStrideBegin = pDstEnd - dwDstStride * 2; pStrideBegin >= pDstBegin; pStrideBegin -= dwDstStride * 2)
						{
							BYTE *pStrideEnd = pStrideBegin + dwDataStride;
							for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 6)
							{
								BYTE *q = p + dwDstStride;
								*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+1) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+0) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+2) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								y++;
								*(q+4) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+3) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+5) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+4) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+3) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+5) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								y++; u++; v++;
							}
							y += dwYStride;
						}
					}
					break;
				case 32:
					{
						DWORD dwDstStride;
						DWORD dwYStride;
						DWORD dwDataStride;

						dwDataStride = m_icd->lpbiOutput->biWidth * 4;
						dwDstStride = ROUNDUP(dwDataStride, 4);
						dwYStride = m_icd->lpbiOutput->biWidth;

						pDstBegin = ((BYTE *)m_icd->lpOutput) + (m_dwNumStripes - m_dwPlaneStripeEnd[nBandIndex]  ) * dwDstStride * 2;
						pDstEnd   = ((BYTE *)m_icd->lpOutput) + (m_dwNumStripes - m_dwPlaneStripeBegin[nBandIndex]) * dwDstStride * 2;

						for (BYTE *pStrideBegin = pDstEnd - dwDstStride * 2; pStrideBegin >= pDstBegin; pStrideBegin -= dwDstStride * 2)
						{
							BYTE *pStrideEnd = pStrideBegin + dwDataStride;
							for (BYTE *p = pStrideBegin; p < pStrideEnd; p += 8)
							{
								BYTE *q = p + dwDstStride;
								*(q+1) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+0) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+2) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(q+3) = 0xff;
								*(p+1) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+0) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+2) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+3) = 0xff;
								y++;
								*(q+5) = min(max(int((*y-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(q+4) = min(max(int((*y-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(q+6) = min(max(int((*y-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(q+7) = 0xff;
								*(p+5) = min(max(int((*(y+dwYStride)-16)*1.164 - (*u-128)*0.391 - (*v-128)*0.813), 0), 255);
								*(p+4) = min(max(int((*(y+dwYStride)-16)*1.164 + (*u-128)*2.018                 ), 0), 255);
								*(p+6) = min(max(int((*(y+dwYStride)-16)*1.164                  + (*v-128)*1.596), 0), 255);
								*(p+7) = 0xff;
								y++; u++; v++;
							}
							y += dwYStride;
						}
					}
					break;
				}
			}
			break;
		}
	}
}
