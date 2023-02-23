/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "TunedFunc_x86x64.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"
#include "ColorOrder.h"
#include "Coefficient.h"

#define GENERATE_AVX2_ADL 1
#include "ConvertYUVRGB_x86x64.cpp"
