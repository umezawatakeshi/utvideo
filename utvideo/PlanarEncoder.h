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
#include "Encoder.h"
#include "FrameBuffer.h"
#include "HuffmanCode.h"
#include "Thread.h"

class CPlanarEncoder :
	public CEncoder
{
protected:
	ENCODERCONF m_ec;
	BOOL m_bBottomUpFrame;
	DWORD m_dwNumStripes;
	DWORD m_dwDivideCount;
	BOOL m_bInterlace;
	DWORD m_dwFrameSize;
	DWORD m_dwFrameStride;
	DWORD m_dwPlaneSize[4];
	DWORD m_dwPlaneWidth[4];
	DWORD m_dwPlanePredictStride[4];

	CThreadManager *m_ptm;
	const ICCOMPRESS *m_icc;
	CFrameBuffer *m_pCurFrame;
	CFrameBuffer *m_pMedianPredicted;
	struct COUNTS
	{
		DWORD dwCount[4][256];
	} *m_counts;
	BYTE *m_pCodeLengthTable[4];
	HUFFMAN_ENCODE_TABLE m_het[4];

public:
	struct INPUTFORMAT
	{
		DWORD fcc;
		WORD nBitCount;
	};

public:
	CPlanarEncoder(void);
	virtual ~CPlanarEncoder(void);

public:
	virtual DWORD Configure(HWND hwnd);
	static int CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DWORD GetState(void *pState, DWORD dwSize);
	virtual DWORD SetState(const void *pState, DWORD dwSize);
	virtual DWORD Compress(const ICCOMPRESS *icc, DWORD dwSize);
	virtual DWORD CompressBegin(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual DWORD CompressEnd(void);
	virtual DWORD CompressGetFormat(const BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut);
	virtual DWORD CompressGetSize(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);
	virtual DWORD CompressQuery(const BITMAPINFOHEADER *pbihIn, const BITMAPINFOHEADER *pbihOut);

protected:
	virtual DWORD GetOutputFCC(void) = 0;
	virtual const char *GetColorFormatName(void) = 0;
	virtual WORD GetOutputBitCount(void) = 0;
	virtual WORD GetMaxBitCount(void) = 0;
	virtual const INPUTFORMAT *GetSupportedInputFormats(void) = 0;
	virtual int GetNumSupportedInputFormats(void) = 0;
	virtual int GetNumPlanes(void) = 0;
	virtual void CalcPlaneSizes(const BITMAPINFOHEADER *pbihIn) = 0;
	virtual void ConvertToPlanar(DWORD nBandIndex) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

private:
	void PredictProc(DWORD nBandIndex);
	class CPredictJob : public CThreadJob
	{
	private:
		DWORD m_nBandIndex;
		CPlanarEncoder *m_pEncoder;
	public:
		CPredictJob(CPlanarEncoder *pEncoder, DWORD nBandIndex)
		{
			m_nBandIndex = nBandIndex;
			m_pEncoder = pEncoder;
		}
		void JobProc(CThreadManager *)
		{
			m_pEncoder->PredictProc(m_nBandIndex);
		}
	};

	void EncodeProc(DWORD nBandIndex);
	class CEncodeJob : public CThreadJob
	{
	private:
		DWORD m_nBandIndex;
		CPlanarEncoder *m_pEncoder;
	public:
		CEncodeJob(CPlanarEncoder *pEncoder, DWORD nBandIndex)
		{
			m_nBandIndex = nBandIndex;
			m_pEncoder = pEncoder;
		}
		void JobProc(CThreadManager *)
		{
			m_pEncoder->EncodeProc(m_nBandIndex);
		}
	};
};
