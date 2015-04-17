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
	char buf[256];
	DWORD cbWritten;

	if (GetLogSocketPath(szPipeName, _countof(szPipeName)) != 0)
		return -1;

	hLogPipe = CreateFile(szPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING /* Ç±ÇÍïKê{ÇÁÇµÇ¢ */, 0, NULL);

	if (hLogPipe == INVALID_HANDLE_VALUE)
		return -1;

	GetModuleFileName(NULL, buf, sizeof(buf));
	char *p = strrchr(buf, '\\');
	if (p != NULL)
		p++;
	else
		p = buf;
	if (!WriteFile(hLogPipe, p, strlen(p), &cbWritten, NULL) || cbWritten != strlen(p))
	{
		CloseHandle(hLogPipe);
		return -1;
	}
	sprintf(buf, "[%d]\n", GetCurrentProcessId());
	if (!WriteFile(hLogPipe, buf, strlen(buf), &cbWritten, NULL) || cbWritten != strlen(buf))
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

	if (!WriteFile(hLogPipe, p, strlen(p), &cbWritten, NULL) || cbWritten != strlen(p))
	{
		UninitializeLogWriter();
		return -1;
	}
	if (!WriteFile(hLogPipe, "\n", 1, &cbWritten, NULL) || cbWritten != 1)
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
	if (connect(fdLogSock, (sockaddr *)&sun, sizeof(sun)) != 0)
	{
		close(fdLogSock);
		fdLogSock = -1;
		return -1;
	}

	sprintf(buf, "%s[%d]\n", getprogname(), getpid()); // XXX getprogname: only BSD family and MacOS
	if (write(fdLogSock, buf, strlen(buf)) != strlen(buf))
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
	if (write(fdLogSock, p, strlen(p)) != strlen(p))
	{
		UninitializeLogWriter();
		return -1;
	}
	if (write(fdLogSock, "\n", 1) != 1)
	{
		UninitializeLogWriter();
		return -1;
	}

	return 0;
}

#endif
