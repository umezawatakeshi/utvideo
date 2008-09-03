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
#include "utvideo.h"
#include "UAY2Encoder.h"
#include "Predict.h"
#include "Convert.h"
#include "resource.h"

const CPackedEncoder::INPUTFORMAT CUAY2Encoder::m_infmts[] = {
	{ FCC('YUY2'), 16, 16, TRUE, TRUE }, { FCC('YUYV'), 16, 16, TRUE, TRUE }, { FCC('YUNV'), 16, 16, TRUE, TRUE },
	{ FCC('UYVY'), 16, 16, TRUE, TRUE }, { FCC('UYNV'), 16, 16, TRUE, TRUE },
	{ FCC('YVYU'), 16, 16, TRUE, TRUE },
	{ FCC('VYUY'), 16, 16, TRUE, TRUE },
};

CUAY2Encoder::CUAY2Encoder(void)
{
}

CUAY2Encoder::~CUAY2Encoder(void)
{
}

int CUAY2Encoder::GetNumSupportedInputFormats(void)
{
	return _countof(m_infmts);
}

CEncoder *CUAY2Encoder::CreateInstance(void)
{
	return new CUAY2Encoder();
}

void CUAY2Encoder::SetCompressionProperty(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut)
{
	m_dwStrideSize = pbihIn->biWidth*2;
	m_dwNumStrides = abs(pbihIn->biHeight);

	switch(pbihIn->biCompression)
	{
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
		m_pfnHuffmanEncodeFirstRawWithDiff = HuffmanEncodeFirstRawWithDiffYUY2;
		break;
	case FCC('UYVY'):
	case FCC('UYNV'):
		m_pfnHuffmanEncodeFirstRawWithDiff = HuffmanEncodeFirstRawWithDiffUYVY;
		break;
	case FCC('YVYU'):
		m_pfnHuffmanEncodeFirstRawWithDiff = HuffmanEncodeFirstRawWithDiffYVYU;
		break;
	case FCC('VYUY'):
		m_pfnHuffmanEncodeFirstRawWithDiff = HuffmanEncodeFirstRawWithDiffVYUY;
		break;
	default:
		_ASSERT(false);
	}
}
