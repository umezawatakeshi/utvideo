/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

// intrin.h をインクルードすると
// error C2733: オーバーロードされた関数 '_interlockedbittestandset' の C リンケージの 2 回以上の宣言は許されません。
// などとエラーが出る。
extern "C" void __cpuid(int *, int);

const TUNEDFUNC tfnSSE2 = {
	x64_sse2_PredictMedianAndCount_align16,
	x64_sse2_PredictMedianAndCount_align1,
	x64_sse2_PredictLeftAndCount_align1,
	x64_sse1mmx_RestoreMedian_align1,
	x64_i686_HuffmanEncode,
	x64_i686_HuffmanDecode,
	x64_i686_HuffmanDecodeAndAccum,
	x64_i686_HuffmanDecodeAndAccumStep2,
	x64_i686_HuffmanDecodeAndAccumStep4,
	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen,
	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue,
	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed,
	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha,
	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen,
	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue,
	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRRed,
	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen,
	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue,
	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed,
	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha,
	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen,
	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue,
	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBRed,
	x64_sse2_ConvertULY2ToBottomupBGR,
	x64_sse2_ConvertULY2ToBottomupBGRX,
	cpp_ConvertULY2ToTopdownBGR,
	cpp_ConvertULY2ToTopdownBGRX,
	x64_sse2_ConvertULY2ToTopdownRGB,
	x64_sse2_ConvertULY2ToTopdownXRGB,
	x64_sse2_ConvertBottomupBGRToULY2,
	x64_sse2_ConvertBottomupBGRXToULY2,
	cpp_ConvertTopdownBGRToULY2,
	cpp_ConvertTopdownBGRXToULY2,
	x64_sse2_ConvertTopdownRGBToULY2,
	x64_sse2_ConvertTopdownXRGBToULY2,
	DummyTunedFunc
};

const TUNEDFUNC &tfnSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSE41 = tfnSSE2;

const TUNEDFUNC &tfnSSE42 = tfnSSE2;

class CTunedFuncInitializer
{
public:
	CTunedFuncInitializer()
	{
		uint32_t cpuid_1_ecx = 0;
		uint32_t cpuid_1_edx = 0;
		int info[4];

		__cpuid(info, 0);
		if (info[0] >= 1)
		{
			__cpuid(info, 1);
			cpuid_1_ecx = info[2];
			cpuid_1_edx = info[3];
		}

		_RPT2(_CRT_WARN, "CPUID.1 ECX=%08X EDX=%08X\n", cpuid_1_ecx, cpuid_1_edx);

		if (cpuid_1_ecx & (1 << 20))
		{
			_RPT0(_CRT_WARN, "supports SSE4.2\n");
			tfn = tfnSSE42;
		}
		else if (cpuid_1_ecx & (1 << 19))
		{
			_RPT0(_CRT_WARN, "supports SSE4.1\n");
			tfn = tfnSSE41;
		}
		else if (cpuid_1_ecx & (1 << 9))
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
			// AMD64 / Intel 64 は SSE2 が必須。
			_RPT0(_CRT_WARN, "supports SSE2\n");
			tfn = tfnSSE2;
		}
	}
} tfi;
