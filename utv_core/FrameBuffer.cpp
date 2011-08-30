/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
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

void CFrameBuffer::AddPlane(size_t cbBuffer, size_t cbMargin)
{
	SYSTEM_INFO si;
	size_t cbAllocateUnit;
	size_t cbAllocated;
	uint8_t *pAllocatedAddr;

	GetSystemInfo(&si);
	cbAllocateUnit = si.dwPageSize;

	cbMargin = ROUNDUP(cbMargin, cbAllocateUnit);
	cbAllocated = cbMargin + ROUNDUP(cbBuffer + cbMargin, cbAllocateUnit);

	pAllocatedAddr = (uint8_t *)VirtualAlloc(NULL, cbAllocated, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAllocatedAddr == NULL)
		return;

	m_pAllocatedAddr[m_nPlanes] = pAllocatedAddr;
	m_cbAllocated[m_nPlanes] = cbAllocated;
	/*
	 * プレーンごとに開始アドレスを少しずつずらし、キャッシュのスラッシングを回避する。
	 * 256 はマジックナンバーであるが、
	 *   - キャッシュラインサイズ（最近の多くのプロセッサでは 64 バイト）の整数倍
	 *   - キャッシュサイズより十分小さい
	 * を満たす必要がある。
	 */
	m_pBufferAddr[m_nPlanes] = pAllocatedAddr + cbMargin + m_nPlanes * 256;

	m_nPlanes++;

	return;
}
