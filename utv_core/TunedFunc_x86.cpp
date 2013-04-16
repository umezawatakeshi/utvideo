/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

const TUNEDFUNC tfnI686 = {
	cpp_PredictWrongMedianAndCount,
	cpp_PredictWrongMedianAndCount,
	cpp_PredictLeftAndCount,
	x86_i686_RestoreWrongMedian_align1,
	x86_i686_HuffmanEncode,
	x86_i686_HuffmanDecode,
	x86_i686_HuffmanDecodeAndAccum,
	x86_i686_HuffmanDecodeAndAccumStep2,
	x86_i686_HuffmanDecodeAndAccumStep4,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha,
	cpp_ConvertULY2ToBottomupBGR,
	cpp_ConvertULY2ToBottomupBGRX,
	cpp_ConvertULY2ToTopdownBGR,
	cpp_ConvertULY2ToTopdownBGRX,
	cpp_ConvertULY2ToTopdownRGB,
	cpp_ConvertULY2ToTopdownXRGB,
	cpp_ConvertBottomupBGRToULY2,
	cpp_ConvertBottomupBGRXToULY2,
	cpp_ConvertTopdownBGRToULY2,
	cpp_ConvertTopdownBGRXToULY2,
	cpp_ConvertTopdownRGBToULY2,
	cpp_ConvertTopdownXRGBToULY2,
	cpp_ConvertBGRToULRG,
	cpp_ConvertBGRXToULRG,
	cpp_ConvertXRGBToULRG,
	cpp_ConvertBGRAToULRA,
	cpp_ConvertARGBToULRA,
	DummyTunedFunc
};

const TUNEDFUNC tfnSSE2 = {
	x86_sse2_PredictWrongMedianAndCount_align16,
	x86_sse2_PredictWrongMedianAndCount_align1,
	x86_sse2_PredictLeftAndCount_align1,
	x86_sse1mmx_RestoreWrongMedian_align1,
	x86_i686_HuffmanEncode,
	x86_i686_HuffmanDecode,
	x86_i686_HuffmanDecodeAndAccum,
	x86_i686_HuffmanDecodeAndAccumStep2,
	x86_i686_HuffmanDecodeAndAccumStep4,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha,
	x86_sse2_ConvertULY2ToBottomupBGR,
	x86_sse2_ConvertULY2ToBottomupBGRX,
	x86_sse2_ConvertULY2ToTopdownBGR,
	x86_sse2_ConvertULY2ToTopdownBGRX,
	x86_sse2_ConvertULY2ToTopdownRGB,
	x86_sse2_ConvertULY2ToTopdownXRGB,
	x86_sse2_ConvertBottomupBGRToULY2,
	x86_sse2_ConvertBottomupBGRXToULY2,
	x86_sse2_ConvertTopdownBGRToULY2,
	x86_sse2_ConvertTopdownBGRXToULY2,
	x86_sse2_ConvertTopdownRGBToULY2,
	x86_sse2_ConvertTopdownXRGBToULY2,
	cpp_ConvertBGRToULRG,
	cpp_ConvertBGRXToULRG,
	cpp_ConvertXRGBToULRG,
	cpp_ConvertBGRAToULRA,
	cpp_ConvertARGBToULRA,
	DummyTunedFunc
};

const TUNEDFUNC &tfnSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSE41 = tfnSSE2;

const TUNEDFUNC &tfnSSE42 = tfnSSE2;

const TUNEDFUNC tfnAVX1 = {
	x86_sse2_PredictWrongMedianAndCount_align16,
	x86_sse2_PredictWrongMedianAndCount_align1,
	x86_sse2_PredictLeftAndCount_align1,
	x86_sse1mmx_RestoreWrongMedian_align1,
	x86_i686_HuffmanEncode,
	x86_i686_HuffmanDecode,
	x86_i686_HuffmanDecodeAndAccum,
	x86_i686_HuffmanDecodeAndAccumStep2,
	x86_i686_HuffmanDecodeAndAccumStep4,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue,
	x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue,
	x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBRed,
	x86_i686_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha,
	x86_sse2_ConvertULY2ToBottomupBGR,
	x86_sse2_ConvertULY2ToBottomupBGRX,
	x86_sse2_ConvertULY2ToTopdownBGR,
	x86_sse2_ConvertULY2ToTopdownBGRX,
	x86_sse2_ConvertULY2ToTopdownRGB,
	x86_sse2_ConvertULY2ToTopdownXRGB,
	x86_sse2_ConvertBottomupBGRToULY2,
	x86_sse2_ConvertBottomupBGRXToULY2,
	x86_sse2_ConvertTopdownBGRToULY2,
	x86_sse2_ConvertTopdownBGRXToULY2,
	x86_sse2_ConvertTopdownRGBToULY2,
	x86_sse2_ConvertTopdownXRGBToULY2,
	x86_avx1_ConvertBGRToULRG,
	x86_avx1_ConvertBGRXToULRG,
	x86_avx1_ConvertXRGBToULRG,
	x86_avx1_ConvertBGRAToULRA,
	x86_avx1_ConvertARGBToULRA,
	DummyTunedFunc
};

const TUNEDFUNC &tfnAVX2 = tfnAVX1;

#include "TunedFunc_x86x64.cpp"
