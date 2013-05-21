/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"
#include "ColorOrder.h"

const TUNEDFUNC tfnCPP = {
	cpp_PredictWrongMedianAndCount,
	cpp_PredictWrongMedianAndCount,
	cpp_PredictLeftAndCount,
	cpp_RestoreWrongMedian,
	cpp_HuffmanEncode,
	cpp_HuffmanDecode,
	cpp_HuffmanDecodeAndAccum,
	cpp_HuffmanDecodeAndAccumStep2,
	cpp_HuffmanDecodeAndAccumStep3,
	cpp_HuffmanDecodeAndAccumStep4,
	cpp_HuffmanDecodeAndAccumStep3ForBGRBlue,
	cpp_HuffmanDecodeAndAccumStep3ForBGRRed,
	cpp_HuffmanDecodeAndAccumStep4ForBGRXBlue,
	cpp_HuffmanDecodeAndAccumStep4ForBGRXRed,
	cpp_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha,
	cpp_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha,
	cpp_ConvertULY2ToRGB<CBGRColorOrder>,
	cpp_ConvertULY2ToRGB<CBGRAColorOrder>,
	cpp_ConvertULY2ToRGB<CRGBColorOrder>,
	cpp_ConvertULY2ToRGB<CARGBColorOrder>,
	cpp_ConvertRGBToULY2<CBGRColorOrder>,
	cpp_ConvertRGBToULY2<CBGRAColorOrder>,
	cpp_ConvertRGBToULY2<CRGBColorOrder>,
	cpp_ConvertRGBToULY2<CARGBColorOrder>,
	cpp_ConvertRGBToULRG<CBGRColorOrder>,
	cpp_ConvertRGBToULRG<CBGRAColorOrder>,
	cpp_ConvertRGBToULRG<CARGBColorOrder>,
	cpp_ConvertARGBToULRA<CBGRAColorOrder>,
	cpp_ConvertARGBToULRA<CARGBColorOrder>,
	cpp_ConvertYUV422ToULY2<CYUYVColorOrder>,
	cpp_ConvertYUV422ToULY2<CUYVYColorOrder>,
	cpp_ConvertULRGToRGB<CBGRColorOrder>,
	cpp_ConvertULRGToRGB<CBGRAColorOrder>,
	cpp_ConvertULRGToRGB<CARGBColorOrder>,
	cpp_ConvertULRAToARGB<CBGRAColorOrder>,
	cpp_ConvertULRAToARGB<CARGBColorOrder>,
	cpp_ConvertULY2ToYUV422<CYUYVColorOrder>,
	cpp_ConvertULY2ToYUV422<CUYVYColorOrder>,
	DummyTunedFunc
};

TUNEDFUNC tfn = tfnCPP;
