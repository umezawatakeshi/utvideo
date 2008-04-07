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

#pragma once
#include "decoder.h"
#include "FrameBuffer.h"

class CULY2Decoder :
	public CDecoder
{
private:
	DWORD m_dwFrameSize;
	DWORD m_dwFrameStride;
	DWORD m_dwYPlaneSize;
	DWORD m_dwYPlaneStride;
	DWORD m_dwCPlaneSize;
	DWORD m_dwCPlaneStride;

public:
	CULY2Decoder(void);
	virtual ~CULY2Decoder(void);

public:
	virtual DWORD Decompress(ICDECOMPRESS *icd, DWORD dwSize);
	virtual DWORD DecompressBegin(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual DWORD DecompressEnd(void);
	virtual DWORD DecompressGetFormat(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual DWORD DecompressQuery(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);

private:
	void ConvertFromPlanarToYUY2(BYTE *pDstBegin, CFrameBuffer *pBuffer, DWORD dwFrameSize);
	DWORD DecodePlane(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pSrcBegin, DWORD dwFrameStride);
};
