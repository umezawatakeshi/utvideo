/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

const TUNEDFUNC tfnSSE2 = {
	x64_sse2_PredictWrongMedianAndCount_align16,
	x64_sse2_PredictWrongMedianAndCount_align1,
	x64_sse2_PredictLeftAndCount_align1,
	x64_sse1mmx_RestoreWrongMedian_align1,
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
	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha,
	x64_sse2_ConvertULY2ToBottomupBGR,
	x64_sse2_ConvertULY2ToBottomupBGRX,
	x64_sse2_ConvertULY2ToTopdownBGR,
	x64_sse2_ConvertULY2ToTopdownBGRX,
	x64_sse2_ConvertULY2ToTopdownRGB,
	x64_sse2_ConvertULY2ToTopdownXRGB,
	x64_sse2_ConvertBottomupBGRToULY2,
	x64_sse2_ConvertBottomupBGRXToULY2,
	x64_sse2_ConvertTopdownBGRToULY2,
	x64_sse2_ConvertTopdownBGRXToULY2,
	x64_sse2_ConvertTopdownRGBToULY2,
	x64_sse2_ConvertTopdownXRGBToULY2,
	cpp_ConvertBGRToULRG,
	cpp_ConvertBGRXToULRG,
	cpp_ConvertXRGBToULRG,
	cpp_ConvertBGRAToULRA,
	cpp_ConvertARGBToULRA,
	cpp_ConvertYUYVToULY2,
	cpp_ConvertUYVYToULY2,
	DummyTunedFunc
};

const TUNEDFUNC &tfnSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSSE3 = tfnSSE2;

const TUNEDFUNC &tfnSSE41 = tfnSSE2;

const TUNEDFUNC &tfnSSE42 = tfnSSE2;

const TUNEDFUNC &tfnAVX1 = tfnSSE2;

const TUNEDFUNC &tfnAVX2 = tfnSSE2;

#include "TunedFunc_x86x64.cpp"
