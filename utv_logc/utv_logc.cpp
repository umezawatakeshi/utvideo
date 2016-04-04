/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#include <LogReader.h>


void OpenProc(int id, int idx, const char *ident);
void ReadProc(int id, int idx, const char *ident, const char *buf);
void CloseProc(int id, int idx, const char *ident);

int main(int argc, char **argv)
{
	InitializeLogReader(OpenProc, ReadProc, CloseProc, true);
#if defined(_WIN32)
	printf("%d\n", GetLastError());
#elif defined(__APPLE__) || defined(__unix__)
	printf("%s (%d)\n", strerror(errno), errno);
#endif
	return 0;
}

void GetLocalTimeString(char *buf)
{
#if defined(_WIN32)
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#elif defined(__APPLE__) || defined(__unix__)
	timeval tv;
	tm lt;
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &lt);
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, tv.tv_usec / 1000);
#endif
}

void OpenProc(int id, int idx, const char *ident)
{
	char ltstr[64];
	GetLocalTimeString(ltstr);
	printf("%s {%d} %s: OPEN\n", ltstr, id, ident);
}

void ReadProc(int id, int idx, const char *ident, const char *buf)
{
	char ltstr[64];
	GetLocalTimeString(ltstr);
	printf("%s {%d} %s: %s\n", ltstr, id, ident, buf);
}

void CloseProc(int id, int idx, const char *ident)
{
	char ltstr[64];
	GetLocalTimeString(ltstr);
	printf("%s {%d} %s: CLOSE\n", ltstr, id, ident);
}
