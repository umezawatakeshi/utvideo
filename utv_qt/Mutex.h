/*  */
/* $Id$ */

#pragma once

#include "pthread.h"

class CMutex
{
private:
	pthread_mutex_t m_mutex;

public:
	CMutex()
	{
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	}

	~CMutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	int Lock()
	{
		return pthread_mutex_lock(&m_mutex);
	}

	int TryLock()
	{
		return pthread_mutex_trylock(&m_mutex);
	}

	int Unlock()
	{
		return pthread_mutex_unlock(&m_mutex);
	}
};

class CMutexLock
{
private:
	CMutex *m_mutex;

public:
	CMutexLock(CMutex *mutex)
	{
		m_mutex = mutex;
		m_mutex->Lock();
	}
	~CMutexLock()
	{
		m_mutex->Unlock();
	}
};
