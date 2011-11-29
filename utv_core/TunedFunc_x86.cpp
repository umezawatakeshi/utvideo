/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

#ifdef _MSC_VER
// intrin.h をインクルードすると
// error C2733: オーバーロードされた関数 '_interlockedbittestandset' の C リンケージの 2 回以上の宣言は許されません。
// などとエラーが出る。
extern "C" void __cpuid(int *, int);
#endif

const TUNEDFUNC tfnI686 = {
	cpp_PredictMedianAndCount,
	cpp_PredictMedianAndCount,
	cpp_PredictLeftAndCount,
	x86_i686_RestoreMedian_align1,
	x86_i686_HuffmanEncode,
	x86_i686_HuffmanDecode,
	x86_i686_HuffmanDecodeAndAccum,
	x86_i686_HuffmanDecodeAndAccumStep2,
	x86_i686_HuffmanDecodeAndAccumStep4,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Green,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Blue,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Red,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Green,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Blue,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Red,
	cpp_ConvertULY2ToBottomupRGB24,
	cpp_ConvertULY2ToBottomupRGB32,
	cpp_ConvertBottomupRGB24ToULY2,
	cpp_ConvertBottomupRGB32ToULY2,
};

const TUNEDFUNC tfnSSE2 = {
	x86_sse2_PredictMedianAndCount_align16,
	x86_sse2_PredictMedianAndCount_align1,
	x86_sse2_PredictLeftAndCount_align1,
	x86_sse1mmx_RestoreMedian_align1,
	x86_i686_HuffmanEncode,
	x86_i686_HuffmanDecode,
	x86_i686_HuffmanDecodeAndAccum,
	x86_i686_HuffmanDecodeAndAccumStep2,
	x86_i686_HuffmanDecodeAndAccumStep4,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Green,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Blue,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Red,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Green,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Blue,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Red,
	x86_sse2_ConvertULY2ToBottomupRGB24,
	x86_sse2_ConvertULY2ToBottomupRGB32,
	x86_sse2_ConvertBottomupRGB24ToULY2,
	x86_sse2_ConvertBottomupRGB32ToULY2,
};

const TUNEDFUNC &tfnSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSSE3 = tfnSSE2;

class CTunedFuncInitializer
{
public:
	CTunedFuncInitializer()
	{
		uint32_t cpuid_1_ecx = 0;
		uint32_t cpuid_1_edx = 0;

#if defined(_MSC_VER)
		int info[4];
		__cpuid(info, 0);
		if (info[0] >= 1)
		{
			__cpuid(info, 1);
			cpuid_1_ecx = info[2];
			cpuid_1_edx = info[3];
		}
#elif defined(__GNUC__)
		int tmp0, tmp1;
		asm volatile (
			"cpuid"
			: "=a"(tmp0), "=b"(tmp1), "=c"(cpuid_1_ecx), "=d"(cpuid_1_edx)
			: "a"(1)
		);
#else
#error
#endif

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
} tfi;
