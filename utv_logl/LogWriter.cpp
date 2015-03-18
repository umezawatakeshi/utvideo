/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "LogPath.h"
#include "LogWriter.h"

#ifdef _WIN32

HANDLE hLogPipe = INVALID_HANDLE_VALUE;

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

int WriteLog(const char *p)
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

#endif
