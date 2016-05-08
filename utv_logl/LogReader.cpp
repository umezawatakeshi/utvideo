/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "LogPath.h"
#include "LogReader.h"

#define NCLIENTS 32
#define LINEBUFSIZE 256

static LogReaderOpenProc fnOpenProc;
static LogReaderReadProc fnReadProc;
static LogReaderCloseProc fnCloseProc;

#if defined(_WIN32)

static CRITICAL_SECTION csLock;

static HANDLE hPipes[NCLIENTS];
static HANDLE hThreads[NCLIENTS];

#elif defined(__APPLE__) || defined(__unix__)

static pthread_mutex_t mutexLock;

static int fdSock;
static pthread_t threads[NCLIENTS];

#endif

static int idcounter = 0;

static int LogReaderProc(int idx)
{
	int id;

	//printf("[%d] Thread Start\n", idx);

	for (;;)
	{
#if defined(_WIN32)
		HANDLE hPipe = hPipes[idx];
		if (!ConnectNamedPipe(hPipe, NULL))
		{
			//fprintf(stderr, "ConnectNamedPipe error GetLastError == %d\n", GetLastError());
			return -1;
		}
		EnterCriticalSection(&csLock);
		id = idcounter++;
		LeaveCriticalSection(&csLock);
#elif defined(__APPLE__) || defined(__unix__)
		sockaddr_un sun;
		socklen_t slen = sizeof(sun);
		int s = accept(fdSock, (sockaddr *)&sun, &slen);
		if (s < 0)
			return -1;
		pthread_mutex_lock(&mutexLock);
		id = idcounter++;
		pthread_mutex_unlock(&mutexLock);
#endif

		char ident[256];
		char buf[256];
		size_t cb = 0;
		bool idented = false;

		for (;;)
		{
#if defined(_WIN32)
			DWORD cbRead;
			if (ReadFile(hPipe, buf + cb, (DWORD)(sizeof(buf) - cb - 1), &cbRead, NULL))
#elif defined(__APPLE__) || defined(__unix__)
			ssize_t cbRead;
			if ((cbRead = read(s, buf + cb, sizeof(buf) - cb - 1)) > 0)
#endif
			{
				cb += cbRead;
				for (;;)
				{
					char *eol = (char *)memchr(buf, '\n', cb);
					if (eol != NULL)
					{
						*eol = '\0';
						if (!idented)
						{
							strcpy(ident, buf);
							idented = true;
							fnOpenProc(id, idx, ident);
						}
						else
						{
							fnReadProc(id, idx, ident, buf);
						}
						memmove(buf, eol + 1, cb - (eol + 1 - buf));
						cb -= (eol + 1 - buf);
					}
					else
					{
						if (cb == sizeof(buf) - 1)
						{
							if (idented)
								fnCloseProc(id, idx, ident);
							goto disconnect;
						}
						break;
					}
				}
			}
#if defined(_WIN32)
			else if (GetLastError() == ERROR_BROKEN_PIPE)
#elif defined(__APPLE__) || defined(__unix__)
			else // XXX
#endif
			{
				if (idented)
					fnCloseProc(id, idx, ident);
				goto disconnect;
			}
		}

	disconnect:
#if defined(_WIN32)
		DisconnectNamedPipe(hPipe);
#elif defined(__APPLE__) || defined(__unix__)
		close(s);
#endif
	}

	return 0;
}

#if defined(_WIN32)

static unsigned int __stdcall ThreadProc(void *arg)
{
	LogReaderProc((int)(intptr_t)arg);

	return 0;
}

int InitializeLogReader(LogReaderOpenProc fnOpenProc, LogReaderReadProc fnReadProc, LogReaderCloseProc fnCloseProc, bool bLoop)
{
	char szPipeName[256];

	::fnOpenProc = fnOpenProc;
	::fnReadProc = fnReadProc;
	::fnCloseProc = fnCloseProc;

	InitializeCriticalSection(&csLock);

	if (GetLogSocketPath(szPipeName, _countof(szPipeName)) != 0)
		return -1;

	// FILE_FLAG_FIRST_PIPE_INSTANCE Ç…ÇÊÇËÉpÉCÉvÉTÅ[ÉoìØémÇÃîrëºêßå‰Ç™é¿åªÇ≈Ç´ÇÈ
	hPipes[0] = CreateNamedPipe(szPipeName, PIPE_ACCESS_INBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE, PIPE_TYPE_BYTE, NCLIENTS, 65536, 65536, 0, NULL);
	if (hPipes[0] == INVALID_HANDLE_VALUE)
	{
		//fprintf(stderr, "CreateNamedPipe error GetLastError == %d\n", GetLastError());
		return -1;
	}
	for (int i = 1; i < NCLIENTS; i++)
	{
		hPipes[i] = CreateNamedPipe(szPipeName, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE, NCLIENTS, 65536, 65536, 0, NULL);
		if (hPipes[i] == INVALID_HANDLE_VALUE)
		{
			DWORD err = GetLastError();
			for (int j = 0; j < i; j++)
				CloseHandle(hPipes[j]);
			//fprintf(stderr, "CreateNamedPipe error GetLastError == %d\n", GetLastError());
			return -1;
		}
	}

	for (int i = 0; i < NCLIENTS; i++)
	{
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (void *)(intptr_t)(i), 0, NULL);
	}

	if (bLoop)
	{
		WaitForMultipleObjects(NCLIENTS, hThreads, TRUE, INFINITE);

		for (int i = 0; i < NCLIENTS; i++)
		{
			CloseHandle(hThreads[i]);
			CloseHandle(hPipes[i]);
		}
	}

	return 0;
}

int UninitializeLogReader(void)
{
	return -1;
}

#elif defined(__APPLE__) || defined(__unix__)

#define _countof(x) (sizeof(x) / sizeof((x)[0]))

static void *ThreadProc(void *arg)
{
	LogReaderProc((int)(intptr_t)arg);

	return 0;
}

int InitializeLogReader(LogReaderOpenProc fnOpenProc, LogReaderReadProc fnReadProc, LogReaderCloseProc fnCloseProc, bool bLoop)
{
	char szPipeName[256];
	sockaddr_un sun;

	::fnOpenProc = fnOpenProc;
	::fnReadProc = fnReadProc;
	::fnCloseProc = fnCloseProc;

	pthread_mutex_init(&mutexLock, NULL);

	memset(&sun, 0, sizeof(sun));
	sun.sun_len = sizeof(sun);
	sun.sun_family = AF_UNIX;
	if (GetLogSocketPath(sun.sun_path, _countof(sun.sun_path)) != 0)
		return -1;
	unlink(sun.sun_path);

	fdSock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fdSock < 0)
		return -1;
	bind(fdSock, (sockaddr *)&sun, sizeof(sun));
	listen(fdSock, 5);

	for (int i = 0; i < NCLIENTS; i++)
	{
		if (pthread_create(&threads[i], NULL, ThreadProc, (void *)(intptr_t)(i)) != 0)
			return -1;
	}

	if (bLoop)
	{
		void *val;

		for (int i = 0; i < NCLIENTS; i++)
			if (pthread_join(threads[i], &val) != 0)
				return -1;

		close(fdSock);
	}

	return 0;
}

int UninitializeLogReader(void)
{
	return -1;
}

#endif

