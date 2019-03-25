/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "EnvFlag.h"

bool GetEnvFlagBool(const char* pszEnvName)
{
#if defined(_WIN32)
	char p[4];
	auto n = GetEnvironmentVariableA(pszEnvName, p, sizeof(p));
	if (n == 0)
		return false;
#endif
#if defined(__APPLE__) || defined(__unix__)
	char *p;
	p = getenv(pszEnvName);
	if (p == NULL)
		return false;
#endif
	if (strcmp(p, "") == 0)
		return false;

	return true;
}
