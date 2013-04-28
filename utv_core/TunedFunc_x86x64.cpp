/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "CPUID.h"

class CTunedFuncInitializer
{
public:
	CTunedFuncInitializer()
	{
		cpuid_result cpuid_0   = { 0, 0, 0, 0 };
		cpuid_result cpuid_1   = { 0, 0, 0, 0 };
		cpuid_result cpuid_7_0 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex0 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex2 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex3 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex4 = { 0, 0, 0, 0 };

		char vendor[16];
		char procbrand[64];

		cpuid(&cpuid_0, 0, 0);
		_RPT4(_CRT_WARN, "CPUID.0   EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_0.eax, cpuid_0.ebx, cpuid_0.ecx, cpuid_0.edx);
		*(uint32_t *)(vendor+0) = cpuid_0.ebx;
		*(uint32_t *)(vendor+4) = cpuid_0.edx;
		*(uint32_t *)(vendor+8) = cpuid_0.ecx;
		vendor[12] = '\0';
		_RPT1(_CRT_WARN, "CPUID     vendor=\"%s\"\n", vendor);

		cpuid(&cpuid_ex0, 0x80000000, 0);
		_RPT4(_CRT_WARN, "CPUID.ex0 EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_ex0.eax, cpuid_ex0.ebx, cpuid_ex0.ecx, cpuid_ex0.edx);

		if (cpuid_ex0.eax >= 0x80000004)
		{
			cpuid(&cpuid_ex2, 0x80000002, 0);
			_RPT4(_CRT_WARN, "CPUID.ex2 EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_ex2.eax, cpuid_ex2.ebx, cpuid_ex2.ecx, cpuid_ex2.edx);
			cpuid(&cpuid_ex3, 0x80000003, 0);
			_RPT4(_CRT_WARN, "CPUID.ex3 EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_ex3.eax, cpuid_ex3.ebx, cpuid_ex3.ecx, cpuid_ex3.edx);
			cpuid(&cpuid_ex4, 0x80000004, 0);
			_RPT4(_CRT_WARN, "CPUID.ex4 EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_ex4.eax, cpuid_ex4.ebx, cpuid_ex4.ecx, cpuid_ex4.edx);
			*(uint32_t *)(procbrand+ 0) = cpuid_ex2.eax;
			*(uint32_t *)(procbrand+ 4) = cpuid_ex2.ebx;
			*(uint32_t *)(procbrand+ 8) = cpuid_ex2.ecx;
			*(uint32_t *)(procbrand+12) = cpuid_ex2.edx;
			*(uint32_t *)(procbrand+16) = cpuid_ex3.eax;
			*(uint32_t *)(procbrand+20) = cpuid_ex3.ebx;
			*(uint32_t *)(procbrand+24) = cpuid_ex3.ecx;
			*(uint32_t *)(procbrand+28) = cpuid_ex3.edx;
			*(uint32_t *)(procbrand+32) = cpuid_ex4.eax;
			*(uint32_t *)(procbrand+36) = cpuid_ex4.ebx;
			*(uint32_t *)(procbrand+40) = cpuid_ex4.ecx;
			*(uint32_t *)(procbrand+44) = cpuid_ex4.edx;
			procbrand[48] = '\0';
			_RPT1(_CRT_WARN, "CPUID     procbrand=\"%s\"\n", procbrand);
		}

		if (cpuid_0.eax >= 1)
		{
			cpuid(&cpuid_1, 1, 0);
			_RPT4(_CRT_WARN, "CPUID.1   EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_1.eax, cpuid_1.ebx, cpuid_1.ecx, cpuid_1.edx);
		}

		if (cpuid_0.eax >= 7)
		{
			cpuid(&cpuid_7_0, 7, 0);
			_RPT4(_CRT_WARN, "CPUID.7.0 EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_7_0.eax, cpuid_7_0.ebx, cpuid_7_0.ecx, cpuid_7_0.edx);
		}


		if (cpuid_1.ecx & (1 << 27))
		{
			_RPT0(_CRT_WARN, "supports OSXSAVE\n");

			xgetbv_result xgetbv_0 = { 0, 0 };

			xgetbv(&xgetbv_0, 0);
			_RPT2(_CRT_WARN, "XGETBV.0  EAX=%08X EDX=%08X\n", xgetbv_0.eax, xgetbv_0.edx);

			if ((xgetbv_0.eax & 6) == 6)
			{
				_RPT0(_CRT_WARN, "supports XMM/YMM state by OS\n");

				if (cpuid_7_0.ebx & (1 << 5))
				{
					_RPT0(_CRT_WARN, "supports AVX2\n");
					tfn = tfnAVX2;
					return;
				}

				if (cpuid_1.ecx & (1 << 28))
				{
					_RPT0(_CRT_WARN, "supports AVX1\n");
					tfn = tfnAVX1;
					return;
				}
			}
			else
			{
				_RPT0(_CRT_WARN, "does not support XMM/YMM state by OS\n");
			}
		}
		else
		{
			_RPT0(_CRT_WARN, "does not support OSXSAVE\n");
		}

		if (cpuid_1.ecx & (1 << 20))
		{
			_RPT0(_CRT_WARN, "supports SSE4.2\n");
			tfn = tfnSSE42;
			return;
		}

		if (cpuid_1.ecx & (1 << 19))
		{
			_RPT0(_CRT_WARN, "supports SSE4.1\n");
			tfn = tfnSSE41;
			return;
		}

		if (cpuid_1.ecx & (1 << 9))
		{
			_RPT0(_CRT_WARN, "supports SSSE3\n");
			tfn = tfnSSSE3;
			return;
		}

		if (cpuid_1.ecx & (1 << 0))
		{
			_RPT0(_CRT_WARN, "supports SSE3\n");
			tfn = tfnSSE3;
			return;
		}

#ifndef __x86_64__
		if (cpuid_1.edx & (1 << 26))
#endif
		{
			_RPT0(_CRT_WARN, "supports SSE2\n");
			tfn = tfnSSE2;
			return;
		}

#ifndef __x86_64__
		{
			_RPT0(_CRT_WARN, "supports no SSE-integer\n");
			tfn = tfnI686;
		}
#endif
	}
} tfi;
