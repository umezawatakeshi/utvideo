/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

class CThreadManager;

class CThreadJob
{
	friend class CThreadManager;

private:
#ifdef _WIN32
	HANDLE m_hCompletionEvent;
#endif

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
	queue<CThreadJob *> m_queueJob[MAX_THREAD];
#ifdef _WIN32
	HANDLE m_hThread[MAX_THREAD];
	DWORD m_dwThreadId[MAX_THREAD];
	HANDLE m_hThreadSemaphore[MAX_THREAD];
	CRITICAL_SECTION m_csJob;
	HANDLE m_hCompletionEvent[MAX_JOB];
#endif

public:
	static int GetNumProcessors(void);

public:
	CThreadManager(void);
	~CThreadManager(void);
	void SubmitJob(CThreadJob *pJob, uint32_t dwAffinityHint);
	void WaitForJobCompletion(void);

private:
#ifdef _WIN32
	static DWORD WINAPI StaticThreadProc(LPVOID lpParameter);
	DWORD ThreadProc(int nThreadIndex);
#endif
};
