/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
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

#define GENERATE_AVX512_ICL 1
#include "Predict_x86x64.cpp"
#include "SymPack_x86x64_zmm.cpp"
