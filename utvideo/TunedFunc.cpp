/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "StdAfx.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

const TUNEDFUNC tfnCPP = {
	cpp_PredictMedian,
	cpp_PredictMedianAndCount,
	cpp_PredictMedianAndCount,
	cpp_PredictLeftAndCount,
	cpp_RestoreMedian,
	cpp_HuffmanEncode,
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

#ifdef _WIN64

// nothing

#else

const TUNEDFUNC tfnI686 = {
	cpp_PredictMedian,
	cpp_PredictMedianAndCount,
	cpp_PredictMedianAndCount,
	cpp_PredictLeftAndCount,
	i686_RestoreMedian_align1,
	i686_HuffmanEncode,
	i686_HuffmanDecode,
	i686_HuffmanDecodeAndAccum,
	i686_HuffmanDecodeAndAccumStep2,
	i686_HuffmanDecodeAndAccumStep4,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,
	cpp_ConvertULY2ToBottomupRGB24,
	cpp_ConvertULY2ToBottomupRGB32,
	cpp_ConvertBottomupRGB24ToULY2,
	cpp_ConvertBottomupRGB32ToULY2,
};

const TUNEDFUNC tfnSSE2 = {
	sse2_PredictMedian_align16,
	sse2_PredictMedianAndCount_align16,
	sse2_PredictMedianAndCount_align1,
	sse2_PredictLeftAndCount_align1,
	sse1mmx_RestoreMedian_align1,
	i686_HuffmanEncode,
	i686_HuffmanDecode,
	i686_HuffmanDecodeAndAccum,
	i686_HuffmanDecodeAndAccumStep2,
	i686_HuffmanDecodeAndAccumStep4,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,
	sse2_ConvertULY2ToBottomupRGB24,
	sse2_ConvertULY2ToBottomupRGB32,
	sse2_ConvertBottomupRGB24ToULY2,
	sse2_ConvertBottomupRGB32ToULY2,
};

const TUNEDFUNC tfnSSE3 = {
	sse2_PredictMedian_align16,
	sse2_PredictMedianAndCount_align16,
	sse2_PredictMedianAndCount_align1,
	sse2_PredictLeftAndCount_align1,
	sse1mmx_RestoreMedian_align1,
	i686_HuffmanEncode,
	i686_HuffmanDecode,
	i686_HuffmanDecodeAndAccum,
	i686_HuffmanDecodeAndAccumStep2,
	i686_HuffmanDecodeAndAccumStep4,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,
	sse2_ConvertULY2ToBottomupRGB24,
	sse2_ConvertULY2ToBottomupRGB32,
	sse2_ConvertBottomupRGB24ToULY2,
	sse2_ConvertBottomupRGB32ToULY2,
};

const TUNEDFUNC tfnSSSE3 = {
	sse2_PredictMedian_align16,
	sse2_PredictMedianAndCount_align16,
	sse2_PredictMedianAndCount_align1,
	sse2_PredictLeftAndCount_align1,
	sse1mmx_RestoreMedian_align1,
	i686_HuffmanEncode,
	i686_HuffmanDecode,
	i686_HuffmanDecodeAndAccum,
	i686_HuffmanDecodeAndAccumStep2,
	i686_HuffmanDecodeAndAccumStep4,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,
	i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,
	i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,
	sse2_ConvertULY2ToBottomupRGB24,
	sse2_ConvertULY2ToBottomupRGB32,
	sse2_ConvertBottomupRGB24ToULY2,
	sse2_ConvertBottomupRGB32ToULY2,
};

#endif

TUNEDFUNC tfn = tfnCPP;

#ifdef _WIN64

void InitializeTunedFunc(void)
{
	// nothing to do
}

#else

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
	else if (cpuid_1_edx & (1 << 26))
	{
		_RPT0(_CRT_WARN, "supports SSE2\n");
		tfn = tfnSSE2;
	}
	else
	{
		_RPT0(_CRT_WARN, "supports no SSE-integer\n");
		tfn = tfnI686;
	}
}

#endif
