/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

const TUNEDFUNC tfnCPP = {
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

TUNEDFUNC tfn = tfnCPP;
