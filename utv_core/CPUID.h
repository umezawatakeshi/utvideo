/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
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

extern "C" void cpuid(cpuid_result *result, uint32_t leaf, uint32_t subleaf);
extern "C" void xgetbv(xgetbv_result *result, uint32_t idx);
