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
#include "FrameBuffer.h"

CFrameBuffer::CFrameBuffer(void)
{
	for (int i = 0; i < MAX_PLANE; i++)
	{
		m_pAllocatedAddr[i] = NULL;
		m_pBufferAddr[i] = NULL;
	}
	m_nPlanes = 0;
}

CFrameBuffer::~CFrameBuffer(void)
{
	for (int i = 0; i < MAX_PLANE; i++)
	{
		if (m_pAllocatedAddr[i] != NULL)
			VirtualFree(m_pAllocatedAddr[i], 0, MEM_RELEASE);
	}
}

void CFrameBuffer::AddPlane(DWORD dwSize, DWORD dwMarginSize)
{
	SYSTEM_INFO si;
	DWORD dwPageSize;
	DWORD dwPrecedingMarginSize;
	DWORD dwAllocateSize;
	BYTE *pAllocatedAddr;

	GetSystemInfo(&si);
	dwPageSize = si.dwPageSize;

	dwPrecedingMarginSize = ROUNDUP(dwMarginSize, dwPageSize);
	dwAllocateSize = dwPrecedingMarginSize + ROUNDUP(dwSize + max(dwMarginSize, dwPageSize), dwPageSize);

	pAllocatedAddr = (BYTE *)VirtualAlloc(NULL, dwAllocateSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAllocatedAddr == NULL)
		return;

	m_pAllocatedAddr[m_nPlanes] = pAllocatedAddr;
	m_pBufferAddr[m_nPlanes] = pAllocatedAddr + dwPrecedingMarginSize;
	m_nPlanes++;

	return;
}
