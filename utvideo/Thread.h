/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

class CThreadManager;

class CThreadJob
{
	friend class CThreadManager;

private:
	HANDLE m_hCompletionEvent;

public:
	CThreadJob(void);
	virtual ~CThreadJob(void) = 0;

public:
	virtual void JobProc(CThreadManager *pManager) = 0;
};

class CThreadManager
{
private:
	static const int MAX_THREAD = 32;
	static const int MAX_JOB = 256;

private:
	int m_nNumThreads;
	int m_nNumJobs;
	HANDLE m_hThread[MAX_THREAD];
	DWORD m_dwThreadId[MAX_THREAD];
	HANDLE m_hThreadSemaphore[MAX_THREAD];
	queue<CThreadJob *> m_queueJob[MAX_THREAD];
	CRITICAL_SECTION m_csJob;
	HANDLE m_hCompletionEvent[MAX_JOB];

public:
	static int GetNumProcessors(void);

public:
	CThreadManager(void);
	~CThreadManager(void);

private:
	static DWORD WINAPI StaticThreadProc(LPVOID lpParameter);
	DWORD ThreadProc(int nThreadIndex);

public:
	void SubmitJob(CThreadJob *pJob, DWORD dwAffinityHint);
	void WaitForJobCompletion(void);
};
