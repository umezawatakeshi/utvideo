/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

const TUNEDFUNC tfnSSE2 = {
	cpp_PredictMedian,
	cpp_PredictMedianAndCount,
	cpp_PredictMedianAndCount,
	cpp_PredictLeftAndCount,
	cpp_RestoreMedian,
	x64_i686_HuffmanEncode,
	cpp_HuffmanDecode,
	cpp_HuffmanDecodeAndAccum,
	cpp_HuffmanDecodeAndAccumStep2,
	cpp_HuffmanDecodeAndAccumStep4,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha,
	cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,
	cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,
	cpp_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,
	cpp_ConvertULY2ToBottomupRGB24,
	cpp_ConvertULY2ToBottomupRGB32,
	cpp_ConvertBottomupRGB24ToULY2,
	cpp_ConvertBottomupRGB32ToULY2,
};

const TUNEDFUNC &tfnSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSSE3 = tfnSSE2;

void InitializeTunedFunc(void)
{
	DWORD	cpuid_1_ecx = 0;
	DWORD	cpuid_1_edx = 0;
	int info[4];

	__cpuid(info, 0);
	if (info[0] >= 1)
	{
		__cpuid(info, 1);
		cpuid_1_ecx = info[2];
		cpuid_1_edx = info[3];
	}

	_RPT2(_CRT_WARN, "CPUID.1 ECX=%08X EDX=%08X\n", cpuid_1_ecx, cpuid_1_edx);

	if (cpuid_1_ecx & (1 << 9))
	{
		_RPT0(_CRT_WARN, "supports SSSE3\n");
		tfn = tfnSSSE3;
	}
	else if (cpuid_1_ecx & (1 << 0))
	{
		_RPT0(_CRT_WARN, "supports SSE3\n");
		tfn = tfnSSE3;
	}
	else
	{
		// AMD64 / Intel 64 ‚Í SSE2 ‚ª•K{B
		_RPT0(_CRT_WARN, "supports SSE2\n");
		tfn = tfnSSE2;
	}
}
