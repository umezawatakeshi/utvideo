/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008  UMEZAWA Takeshi
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
 * Copyright (C) 2008  梅澤 威志
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
#include "UAY2Encoder.h"
#include "utvideo.h"

CUAY2Encoder::CUAY2Encoder(void)
{
	m_pPrevFrame = NULL;
}

CUAY2Encoder::~CUAY2Encoder(void)
{
}

DWORD CUAY2Encoder::Compress(ICCOMPRESS *icc, DWORD dwSize)
{
	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');

	if (!(icc->dwFlags & ICCOMPRESS_KEYFRAME) && memcmp(m_pPrevFrame->GetBuffer(), icc->lpInput, icc->lpbiInput->biSizeImage) == 0)
	{
		icc->lpbiOutput->biSizeImage = 0;
		*icc->lpdwFlags = 0;
		return ICERR_OK;
	}

	memcpy(icc->lpOutput, icc->lpInput, icc->lpbiInput->biSizeImage);
	memcpy(m_pPrevFrame->GetBuffer(), icc->lpInput, icc->lpbiInput->biSizeImage);
	icc->lpbiOutput->biSizeImage = icc->lpbiInput->biSizeImage;
	*icc->lpdwFlags = AVIIF_KEYFRAME;
	return ICERR_OK;
}

DWORD CUAY2Encoder::CompressBegin(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	m_pPrevFrame = CFrameBuffer::NewBuffer(pbmihIn->biWidth * pbmihIn->biHeight * 2, pbmihIn->biWidth * 2);
	return ICERR_OK;
}

DWORD CUAY2Encoder::CompressEnd(void)
{
	delete m_pPrevFrame;
	return ICERR_OK;
}

DWORD CUAY2Encoder::CompressGetFormat(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	BITMAPINFOHEADER_EXTRA *pbmiheOut;

	if (pbmihOut == NULL)
		return sizeof(BITMAPINFOHEADER) + sizeof(BITMAPINFOHEADER_EXTRA);

	pbmihOut->biSize          = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPINFOHEADER_EXTRA);
	pbmihOut->biWidth         = pbmihIn->biWidth;
	pbmihOut->biHeight        = pbmihIn->biHeight;
	pbmihOut->biPlanes        = 1;
	pbmihOut->biBitCount      = 16;
	pbmihOut->biCompression   = FCC('UAY2');
	pbmihOut->biSizeImage     = pbmihIn->biSizeImage;
	//pbmihOut->biXPelsPerMeter
	//pbmihOut->biYPelsPerMeter
	//pbmihOut->biClrUsed
	//pbmihOut->biClrImportant

	pbmiheOut = (BITMAPINFOHEADER_EXTRA *)(pbmihOut + 1);
	pbmiheOut->dwEncoderVersion  = UTVIDEO_ENCODER_VERSION;
	pbmiheOut->fccOriginalFormat = pbmihIn->biCompression;

	return ICERR_OK;
}

DWORD CUAY2Encoder::CompressGetSize(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	return pbmihIn->biSizeImage;
}

DWORD CUAY2Encoder::CompressQuery(BITMAPINFOHEADER *pbmihIn, BITMAPINFOHEADER *pbmihOut)
{
	if (pbmihIn->biCompression == FCC('YUY2'))
		return ICERR_OK;
	else
		return ICERR_BADFORMAT;
}
