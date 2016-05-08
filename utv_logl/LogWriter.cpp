/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "LogPath.h"
#include "LogWriter.h"

#if defined(_WIN32)

HANDLE __declspec(dllexport) hLogPipe = INVALID_HANDLE_VALUE;

int InitializeLogWriter(void)
{
	char szPipeName[256];
	char szModuleFileName[256];
	char buf[256];
	DWORD cbWritten;

	if (GetLogSocketPath(szPipeName, _countof(szPipeName)) != 0)
		return -1;

	hLogPipe = CreateFile(szPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING /* Ç±ÇÍïKê{ÇÁÇµÇ¢ */, 0, NULL);

	if (hLogPipe == INVALID_HANDLE_VALUE)
		return -1;

	GetModuleFileName(NULL, szModuleFileName, sizeof(szModuleFileName));
	char *p = strrchr(szModuleFileName, '\\');
	if (p != NULL)
		p++;
	else
		p = szModuleFileName;
	sprintf(buf, "%s[%d]\n", p, GetCurrentProcessId());
	if (!WriteFile(hLogPipe, buf, (DWORD)strlen(buf), &cbWritten, NULL) || cbWritten != strlen(buf))
	{
		CloseHandle(hLogPipe);
		return -1;
	}

	FlushFileBuffers(hLogPipe);

	return 0;
}

int UninitializeLogWriter(void)
{
	if (hLogPipe != INVALID_HANDLE_VALUE)
		CloseHandle(hLogPipe);
	hLogPipe = INVALID_HANDLE_VALUE;

	return 0;
}

int __declspec(dllexport) WriteLog(const char *p)
{
	DWORD cbWritten;

	if (!WriteFile(hLogPipe, p, (DWORD)strlen(p), &cbWritten, NULL) || cbWritten != strlen(p))
	{
		UninitializeLogWriter();
		return -1;
	}

	FlushFileBuffers(hLogPipe);

	return 0;
}

#elif defined(__APPLE__) || defined(__unix__)

#define _countof(x) (sizeof(x) / sizeof(x[0]))

int fdLogSock = -1;

static int writeLog(const char *buf, size_t len)
{
	return send(fdLogSock, buf, len,
#ifdef MSG_NOSIGNAL
		MSG_NOSIGNAL
#else
		0
#endif
	);
}

int InitializeLogWriter(void)
{
	char buf[256];
	sockaddr_un sun;

	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
	if (GetLogSocketPath(sun.sun_path, _countof(sun.sun_path)) != 0)
		return -1;

	fdLogSock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fdLogSock == -1)
		return -1;
#ifdef SO_NOSIGPIPE
	const int on = 1;
	setsockopt(fdLogSock, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(int));
#endif
	if (connect(fdLogSock, (sockaddr *)&sun, sizeof(sun)) != 0)
	{
		close(fdLogSock);
		fdLogSock = -1;
		return -1;
	}

	sprintf(buf, "%s[%d]\n", getprogname(), getpid()); // XXX getprogname: only BSD family and MacOS
	if (writeLog(buf, strlen(buf)) != strlen(buf))
	{
		close(fdLogSock);
		fdLogSock = -1;
		return -1;
	}

	return 0;
}

int UninitializeLogWriter(void)
{
	if (fdLogSock != -1)
		close(fdLogSock);
	fdLogSock = -1;

	return 0;
}

int WriteLog(const char *p)
{
	if (writeLog(p, strlen(p)) != strlen(p))
	{
		UninitializeLogWriter();
		return -1;
	}

	return 0;
}

#endif
