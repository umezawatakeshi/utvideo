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

#pragma once
#include "Decoder.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CPlanarDecoder :
	public CDecoder
{
protected:
	BOOL m_bBottomUpFrame;
	DWORD m_dwNumStripes;
	DWORD m_dwDivideCount;
	BOOL m_bInterlace;
	DWORD m_dwRawSize;
	DWORD m_dwRawGrossWidth;
	DWORD m_dwRawNetWidth;
	DWORD m_dwPlaneSize[4];
	DWORD m_dwPlaneWidth[4];
	DWORD m_dwPlaneStripeSize[4];
	DWORD m_dwPlanePredictStride[4];
	DWORD m_dwPlaneStripeBegin[256];
	DWORD m_dwPlaneStripeEnd[256];
	DWORD m_dwRawStripeBegin[256];
	DWORD m_dwRawStripeEnd[256];
	DWORD m_dwRawStripeSize;

	CThreadManager *m_ptm;
	CFrameBuffer *m_pCurFrame;
	CFrameBuffer *m_pRestoredFrame;
	CFrameBuffer *m_pDecodedFrame;
	FRAMEINFO m_fi;
	HUFFMAN_DECODE_TABLE m_hdt[4];
	const BYTE *m_pCodeLengthTable[4];
	const ICDECOMPRESS *m_icd;

public:
	struct OUTPUTFORMAT
	{
		DWORD fcc;
		WORD nBitCount;
	};

public:
	CPlanarDecoder(void);
	virtual ~CPlanarDecoder(void);

public:
	virtual DWORD Decompress(const ICDECOMPRESS *icd, SIZE_T cb);
	virtual DWORD DecompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual DWORD DecompressEnd(void);
	virtual DWORD DecompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual DWORD DecompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

protected:
	virtual DWORD GetInputFCC(void) = 0;
	virtual WORD GetInputBitCount(void) = 0;
	virtual const OUTPUTFORMAT *GetSupportedOutputFormats(void) = 0;
	virtual int GetNumSupportedOutputFormats(void) = 0;
	virtual int GetNumPlanes(void) = 0;
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn) = 0;
	virtual void ConvertFromPlanar(DWORD nBandIndex) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

private:
	void DecodeProc(DWORD nBandIndex);
	class CDecodeJob : public CThreadJob
	{
	private:
		DWORD m_nBandIndex;
		CPlanarDecoder *m_pDecoder;
	public:
		CDecodeJob(CPlanarDecoder *pDecoder, DWORD nBandIndex)
		{
			m_nBandIndex = nBandIndex;
			m_pDecoder = pDecoder;
		}
		void JobProc(CThreadManager *)
		{
			m_pDecoder->DecodeProc(m_nBandIndex);
		}
	};
};
