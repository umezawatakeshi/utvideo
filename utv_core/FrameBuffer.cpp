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
		{
#ifdef _WIN32
			VirtualFree(m_pAllocatedAddr[i], 0, MEM_RELEASE);
#endif
#if defined(__APPLE__) || defined(__unix__)
			munmap(m_pAllocatedAddr[i], m_cbAllocated[i]);
#endif
		}
	}
}

void CFrameBuffer::AddPlane(size_t cbBuffer, size_t cbMargin)
{
#ifdef _WIN32
	SYSTEM_INFO si;
#endif
	size_t cbAllocateUnit;
	size_t cbAllocated;
	uint8_t *pAllocatedAddr;
	/*
	 * プレーンごとに開始アドレスを少しずつずらし、キャッシュのスラッシングを回避する。
	 * 256 はマジックナンバーであるが、
	 *   - キャッシュラインサイズ（最近の多くのプロセッサでは 64 バイト）の整数倍
	 *   - キャッシュサイズより十分小さい
	 * を満たす必要がある。
	 */
	size_t cbOffset = m_nPlanes * 256;

#ifdef _WIN32
	GetSystemInfo(&si);
	cbAllocateUnit = si.dwPageSize;
#endif
#if defined(__APPLE__) || defined(__unix__)
	cbAllocateUnit = getpagesize();
#endif
	cbMargin = ROUNDUP(cbMargin, cbAllocateUnit);
	cbAllocated = cbMargin + ROUNDUP(cbBuffer + cbMargin + cbOffset, cbAllocateUnit);

#ifdef _WIN32
	pAllocatedAddr = (uint8_t *)VirtualAlloc(NULL, cbAllocated, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAllocatedAddr == NULL)
		return;
#endif
#if defined(__APPLE__) || defined(__unix__)
	pAllocatedAddr = (uint8_t *)mmap(NULL, cbAllocated, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (pAllocatedAddr == MAP_FAILED)
		return;
#endif

	m_pAllocatedAddr[m_nPlanes] = pAllocatedAddr;
	m_cbAllocated[m_nPlanes] = cbAllocated;
	m_pBufferAddr[m_nPlanes] = pAllocatedAddr + cbMargin + cbOffset;

	m_nPlanes++;

	return;
}
