/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

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
