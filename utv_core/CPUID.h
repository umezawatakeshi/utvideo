/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

struct cpuid_result
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
};

struct xgetbv_result
{
	uint32_t eax;
	uint32_t edx;
};

static inline void cpuid(cpuid_result *result, uint32_t leaf, uint32_t subleaf)
{
#if defined(_MSC_VER)
	__cpuidex((int*)result, leaf, subleaf);
#elif defined(__GNUC__)
	__asm__ __volatile__
	(
		"cpuid"
		:
		"=a"(res->eax),
		"=b"(res->ebx),
		"=c"(res->ecx),
		"=d"(res->edx)
		:
		"a"(leaf),
		"c"(subleaf)
	);
#else
#error
#endif

#ifdef _DEBUG
	char buf[16];
	if (leaf >= 0x80000000)
		sprintf(buf, "ex%d", leaf - 0x80000000);
	else if (leaf == 7)
		sprintf(buf, "7.%d", subleaf);
	else
		sprintf(buf, "%d", leaf);
	_RPT5(_CRT_WARN, "CPUID.%-3s EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", buf,
		result->eax, result->ebx, result->ecx, result->edx);
#endif
}

static inline void xgetbv(xgetbv_result *result, uint32_t idx)
{
#if defined(_MSC_VER)
	unsigned long long res = _xgetbv(idx);
	result->eax = res & 0xffffffff;
	result->edx = res >> 32;
#elif defined(__GNUC__)
	__asm__ __volatile__
	(
		"xgetbv"
		:
		"=a"(res->eax),
		"=d"(res->edx)
		:
		"c"(idx)
	);
#else
#error
#endif

#ifdef _DEBUG
	_RPT3(_CRT_WARN, "XGETBV.%-2d EAX=%08X EDX=%08X\n", idx, result->eax, result->edx);
#endif
}
