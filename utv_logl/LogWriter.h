/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

int InitializeLogWriter(void);
int WriteLog(const char *p);
int UninitializeLogWriter(void);

#ifdef _WIN32

extern HANDLE hLogPipe;

#define LOGPRINTF(__fmt__, ...) \
	do \
	{ \
		if (::hLogPipe != INVALID_HANDLE_VALUE) \
		{ \
			char __LOGPRINTF_local_buf__[256]; \
			sprintf(__LOGPRINTF_local_buf__, __fmt__, __VA_ARGS__); \
			WriteLog(__LOGPRINTF_local_buf__); \
		} \
	} while (false)

#endif
