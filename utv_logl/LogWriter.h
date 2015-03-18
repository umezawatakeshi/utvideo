/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

int InitializeLogWriter(void);
int WriteLog(const char *p);
int UninitializeLogWriter(void);

#ifdef _WIN32

#include <crtdbg.h>

extern HANDLE hLogPipe;

static inline bool IsLogWriterInitialized()
{
	return ::hLogPipe != INVALID_HANDLE_VALUE;
}

#ifdef _DEBUG

static inline bool IsLogWriterInitializedOrDebugBuild()
{
	return true;
}

#define LOGPRINTF(__fmt__, ...) \
	do \
	{ \
		char __LOGPRINTF_local_buf__[256]; \
		sprintf(__LOGPRINTF_local_buf__, __fmt__, __VA_ARGS__); \
		if (IsLogWriterInitialized()) \
		{ \
			WriteLog(__LOGPRINTF_local_buf__); \
		} \
		strcat(__LOGPRINTF_local_buf__, "\n"); \
		OutputDebugString(__LOGPRINTF_local_buf__); \
	} while (false)

#define DBGPRINTF(__fmt__, ...) \
	do \
	{ \
		char __LOGPRINTF_local_buf__[256]; \
		sprintf(__LOGPRINTF_local_buf__, __fmt__, __VA_ARGS__); \
		strcat(__LOGPRINTF_local_buf__, "\n"); \
		OutputDebugString(__LOGPRINTF_local_buf__); \
	} while (false)

#else

static inline bool IsLogWriterInitializedOrDebugBuild()
{
	return IsLogWriterInitialized();
}

#define LOGPRINTF(__fmt__, ...) \
	do \
	{ \
		if (IsLogWriterInitialized()) \
		{ \
			char __LOGPRINTF_local_buf__[256]; \
			sprintf(__LOGPRINTF_local_buf__, __fmt__, __VA_ARGS__); \
			WriteLog(__LOGPRINTF_local_buf__); \
		} \
	} while (false)

#define DBGPRINTF(__fmt__, ...) \
	do \
	{ \
	} while (false)

#endif

#endif
