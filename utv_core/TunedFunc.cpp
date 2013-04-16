/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"

const TUNEDFUNC tfnCPP = {
	cpp_PredictWrongMedianAndCount,
	cpp_PredictWrongMedianAndCount,
	cpp_PredictLeftAndCount,
	cpp_RestoreWrongMedian,
	cpp_HuffmanEncode,
	cpp_HuffmanDecode,
	cpp_HuffmanDecodeAndAccum,
	cpp_HuffmanDecodeAndAccumStep2,
	cpp_HuffmanDecodeAndAccumStep4,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed,
	cpp_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha,
	cpp_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen,
	cpp_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue,
	cpp_HuffmanDecodeAndAccumStep3ForBottomupBGRRed,
	cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen,
	cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue,
	cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed,
	cpp_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha,
	cpp_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen,
	cpp_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue,
	cpp_HuffmanDecodeAndAccumStep3ForTopdownRGBRed,
	cpp_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha,
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
	DummyTunedFunc
};

TUNEDFUNC tfn = tfnCPP;
