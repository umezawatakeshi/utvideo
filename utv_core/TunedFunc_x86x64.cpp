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

		cpuid(&cpuid_0, 0, 0);
		_RPT4(_CRT_WARN, "CPUID.0   EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", cpuid_0.eax, cpuid_0.ebx, cpuid_0.ecx, cpuid_0.edx);

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
