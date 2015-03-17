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

#ifdef _WIN32

static CRITICAL_SECTION csLock;

static HANDLE hPipes[NCLIENTS];
static HANDLE hThreads[NCLIENTS];
static int idcounter = 0;

static unsigned int __stdcall PipeServerProc(void *arg)
{
	const int idx = (int)(intptr_t)arg;
	HANDLE hPipe = hPipes[idx];
	int id;

	//printf("[%d] Thread Start\n", idx);

	for (;;)
	{
		if (!ConnectNamedPipe(hPipe, NULL))
		{
			//fprintf(stderr, "ConnectNamedPipe error GetLastError == %d\n", GetLastError());
			return -1;
		}

		EnterCriticalSection(&csLock);
		id = idcounter++;
		LeaveCriticalSection(&csLock);

		char ident[256];
		char buf[256];
		DWORD cb = 0;
		bool idented = false;

		for (;;)
		{
			DWORD cbRead;
			if (ReadFile(hPipe, buf + cb, sizeof(buf) - cb - 1, &cbRead, NULL))
			{
				cb += cbRead;
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
						break;
					}
				}
			}
			else if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				if (idented)
					fnCloseProc(id, idx, ident);
				break;
			}
		}

		DisconnectNamedPipe(hPipe);
	}
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
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, PipeServerProc, (void *)(intptr_t)(i), 0, NULL);
	}

	WaitForMultipleObjects(NCLIENTS, hThreads, TRUE, INFINITE);

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

#endif
