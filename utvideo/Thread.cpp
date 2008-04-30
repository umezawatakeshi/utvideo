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
#include "Thread.h"

CThreadManager::CThreadManager(void)
{
	DWORD dwProcessAffinityMask;
	DWORD dwSystemAffinityMask;

	_RPT1(_CRT_WARN, "enter CThreadManager::CThreadManager() this=%p\n", this);
	m_nNumThreads = 0;
	m_nNumJobs = 0;
	for (int i = 0; i < MAX_THREAD; i++)
	{
		m_hThread[i] = NULL;
		m_hThreadSemaphore[i] = NULL;
	}

	GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinityMask, &dwSystemAffinityMask);
	for (DWORD dwThreadAffinityMask = 1; dwThreadAffinityMask != 0; dwThreadAffinityMask <<= 1)
	{
		if (dwProcessAffinityMask & dwThreadAffinityMask)
		{
			m_hThread[m_nNumThreads] = CreateThread(NULL, 0, StaticThreadProc, this, CREATE_SUSPENDED, &m_dwThreadId[m_nNumThreads]);
			SetThreadAffinityMask(m_hThread[m_nNumThreads], dwThreadAffinityMask);
			m_hThreadSemaphore[m_nNumThreads] = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
			m_nNumThreads++;
		}
	}

	InitializeCriticalSection(&m_csJob);

	for (int i = 0; i < m_nNumThreads; i++)
		ResumeThread(m_hThread[i]);
	_RPT1(_CRT_WARN, "leave CThreadManager::CThreadManager() this=%p\n", this);
}

CThreadManager::~CThreadManager(void)
{
	_ASSERT(m_nNumJobs == 0);
	_RPT1(_CRT_WARN, "enter CThreadManager::~CThreadManager() this=%p\n", this);
	EnterCriticalSection(&m_csJob);
	for (int i = 0; i < m_nNumThreads; i++)
	{
		m_queueJob[i].push(NULL);
		ReleaseSemaphore(m_hThreadSemaphore[i], 1, 0);
	}
	LeaveCriticalSection(&m_csJob);

	WaitForMultipleObjects(m_nNumThreads, m_hThread, TRUE, INFINITE);
	for (int i = 0; i < m_nNumThreads; i++)
	{
		CloseHandle(m_hThread[i]);
		CloseHandle(m_hThreadSemaphore[i]);
	}
	DeleteCriticalSection(&m_csJob);
	_RPT1(_CRT_WARN, "leave CThreadManager::~CThreadManager() this=%p\n", this);
}

DWORD WINAPI CThreadManager::StaticThreadProc(LPVOID lpParameter)
{
	CThreadManager *pThis = (CThreadManager *)lpParameter;
	DWORD dwThreadId;
	int nThreadIndex;

	dwThreadId = GetCurrentThreadId();
	for (nThreadIndex = 0; nThreadIndex < pThis->m_nNumThreads; nThreadIndex++)
		if (pThis->m_dwThreadId[nThreadIndex] == dwThreadId)
			break;
	_ASSERT(nThreadIndex < pThis->m_nNumThreads);
	return pThis->ThreadProc(nThreadIndex);
}

DWORD CThreadManager::ThreadProc(int nThreadIndex)
{
	_RPT3(_CRT_WARN, "enter CThreadManager::ThreadProc() this=%p index=%d ID=%08X\n", this, nThreadIndex, GetCurrentThreadId());
	for (;;)
	{
		CThreadJob *pJob;

		WaitForSingleObject(m_hThreadSemaphore[nThreadIndex], INFINITE);
		EnterCriticalSection(&m_csJob);
		pJob = m_queueJob[nThreadIndex].front();
		m_queueJob[nThreadIndex].pop();
		LeaveCriticalSection(&m_csJob);
		if (pJob == NULL)
			break;
		pJob->JobProc(this);
		SetEvent(pJob->m_hCompletionEvent);
		delete pJob;
	}
	_RPT3(_CRT_WARN, "leave CThreadManager::ThreadProc() this=%p index=%d ID=%08X\n", this, nThreadIndex, GetCurrentThreadId());
	return 0;
}

void CThreadManager::SubmitJob(CThreadJob *pJob, DWORD dwAffinityHint)
{
	HANDLE hCompletionEvent;
	int nThreadIndex;

	_ASSERT(pJob != NULL);
	_ASSERT(m_nNumJobs < MAX_JOB);

	hCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	pJob->m_hCompletionEvent = hCompletionEvent;
	nThreadIndex = dwAffinityHint % m_nNumThreads;
	m_hCompletionEvent[m_nNumJobs++] = hCompletionEvent;
	EnterCriticalSection(&m_csJob);
	m_queueJob[nThreadIndex].push(pJob);
	LeaveCriticalSection(&m_csJob);
	ReleaseSemaphore(m_hThreadSemaphore[nThreadIndex], 1, NULL);
}

void CThreadManager::WaitForJobCompletion(void)
{
	// 待機中にジョブが追加されることは考慮していない。
	WaitForMultipleObjects(m_nNumJobs, m_hCompletionEvent, TRUE, INFINITE);
	for (int i = 0; i < m_nNumJobs; i++)
		CloseHandle(m_hCompletionEvent[i]);
	m_nNumJobs = 0;
}

CThreadJob::CThreadJob(void)
{
}

CThreadJob::~CThreadJob(void)
{
}
