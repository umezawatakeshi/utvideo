/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "TunedFunc_x86x64.h"
#include "Predict.h"
#include "Predict_x86x64.h"
#include "HuffmanCode.h"
#include "Convert.h"
#include "ConvertYUVRGB_x86x64.h"
#include "ConvertShuffle_x86x64.h"
#include "SymPack.h"
#include "SymPack_x86x64.h"
#include "ConvertPredict.h"
#include "ConvertPredict_x86x64.h"
#include "ConvertSymPack_x86x64.h"
#include "ColorOrder.h"
#include "Coefficient.h"
#include "CPUID.h"
#include "Log.h"
#include "EnvFlag.h"

#define FEATURE0_SSE2                   (1 <<  0)
#define FEATURE0_SSE3                   (1 <<  1)
#define FEATURE0_SSSE3                  (1 <<  2)
#define FEATURE0_SSE41                  (1 <<  3)
#define FEATURE0_SSE42                  (1 <<  4)
#define FEATURE0_AVX1                   (1 <<  5)
#define FEATURE0_AVX2                   (1 <<  6)
#define FEATURE0_FMA3                   (1 <<  7) /* 3-operand FMA */
#define FEATURE0_AVX512F                (1 <<  8) /* AVX-512 Foundation */
#define FEATURE0_AVX512CD               (1 <<  9) /* AVX-512 Conflict Detection */
#define FEATURE0_AVX512ER               (1 << 10) /* AVX-512 Exponential & Reciprocal */
#define FEATURE0_AVX512PF               (1 << 11) /* AVX-512 Prefetch */
#define FEATURE0_AVX512DQ               (1 << 12) /* AVX-512 Doubleword & Quadword */
#define FEATURE0_AVX512BW               (1 << 13) /* AVX-512 Byte & Word */
#define FEATURE0_AVX512VL               (1 << 14) /* AVX-512 Vector Length */
#define FEATURE0_AVX512_IFMA            (1 << 15) /* AVX-512 Integer FMA */
#define FEATURE0_AVX512_VBMI            (1 << 16) /* AVX-512 Vector Byte Manipulation Instructions */
#define FEATURE0_AVX512_4FMAPS          (1 << 17) /* AVX-512 4-iteration FMA, Packed Single */
#define FEATURE0_AVX512_4VNNIW          (1 << 18) /* AVX-512 4-iteration Vector Neural Network Instructions, Word */
#define FEATURE0_AVX512_VNNI            (1 << 19) /* AVX-512 Vector Neural Network Instructions */
#define FEATURE0_AVX512_VPOPCNTDQ       (1 << 20) /* AVX-512 VPOPCNTDQ */
#define FEATURE0_AVX512_VBMI2           (1 << 21) /* AVX-512 VBMI2 */
#define FEATURE0_AVX512_BITALG          (1 << 22) /* AVX-512 Bit Algorithms */

#define FEATURE1_MOVBE                  (1 <<  0)
#define FEATURE1_POPCNT                 (1 <<  1)
#define FEATURE1_LZCNT                  (1 <<  2)
#define FEATURE1_BMI1                   (1 <<  3)
#define FEATURE1_BMI2                   (1 <<  4)


#define MA_INVALID            0x00000000
#define MA_UNKNOWN            0xffffffff

#define MA_INTEL_MEROM        0x00010000
#define MA_INTEL_PENRYN       0x00010001
#define MA_INTEL_NEHALEM      0x00010002
#define MA_INTEL_WESTMERE     0x00010003
#define MA_INTEL_SANDY_BRIDGE 0x00010004
#define MA_INTEL_IVY_BRIDGE   0x00010005
#define MA_INTEL_HASWELL      0x00010006
#define MA_INTEL_BROADWELL    0x00010007
#define MA_INTEL_SKYLAKE      0x00010008

struct MAFM
{
	const char *name;
	uint32_t ma;
	uint16_t fm[6];
};

/*
 * Reference:
 *   - http://software.intel.com/en-us/articles/intel-architecture-and-processor-identification-with-cpuid-model-and-family-numbers
 * and
 *   - Intel 64 and IA-32 Architectures Optimization Reference Manual, Appendix C
 */
static const struct MAFM mafm[] = {
	{ "Merom",        MA_INTEL_MEROM,        { 0x060f, 0x0616 } },
	{ "Penryn",       MA_INTEL_PENRYN,       { 0x0617, 0x061d } },
	{ "Nehalem",      MA_INTEL_NEHALEM,      { 0x061a, 0x061e, 0x061f, 0x062e } },
	{ "Westmere",     MA_INTEL_WESTMERE,     { 0x0625, 0x062c, 0x062f } },
	{ "Sandy Bridge", MA_INTEL_SANDY_BRIDGE, { 0x062a, 0x062d } },
	{ "Ivy Bridge",   MA_INTEL_IVY_BRIDGE,   { 0x063a, 0x063e } },
	{ "Haswell",      MA_INTEL_HASWELL,      { 0x063c, 0x0645, 0x0646, 0x063f } },
	{ "Broadwell",    MA_INTEL_BROADWELL,    { 0x063d, 0x0647, 0x0656 } },
	{ "Skylake",      MA_INTEL_SKYLAKE,      { 0x064e, 0x065e } },
//	{ "", MA_, { 0x0600 } },
};


const TUNEDFUNC_PREDICT tfnPredictSSE41 = {
	NULL,
	{ FEATURE0_SSE41, 0 },
	tuned_PredictCylindricalWrongMedianAndCount8<CODEFEATURE_SSE41>,
	tuned_PredictCylindricalLeftAndCount8<CODEFEATURE_SSE41>,
	tuned_RestoreCylindricalWrongMedian8<CODEFEATURE_SSE41>,
	tuned_RestoreCylindricalLeft8<CODEFEATURE_SSE41>,
	tuned_PredictCylindricalLeftAndCount10<CODEFEATURE_SSE41>,
	tuned_RestoreCylindricalLeft10<CODEFEATURE_SSE41>,
	tuned_PredictPlanarGradientAndCount8<CODEFEATURE_SSE41>,
	tuned_PredictPlanarGradient8<CODEFEATURE_SSE41>,
	tuned_RestorePlanarGradient8<CODEFEATURE_SSE41>,
	tuned_PredictPlanarGradientAndCount10<CODEFEATURE_SSE41>,
	tuned_RestorePlanarGradient10<CODEFEATURE_SSE41>,
};

const TUNEDFUNC_PREDICT tfnPredictAVX1 = {
	&tfnPredictSSE41,
	{ FEATURE0_AVX1, 0 },
	tuned_PredictCylindricalWrongMedianAndCount8<CODEFEATURE_AVX1>,
	tuned_PredictCylindricalLeftAndCount8<CODEFEATURE_AVX1>,
	tuned_RestoreCylindricalWrongMedian8<CODEFEATURE_AVX1>,
	tuned_RestoreCylindricalLeft8<CODEFEATURE_AVX1>,
	tuned_PredictCylindricalLeftAndCount10<CODEFEATURE_AVX1>,
	tuned_RestoreCylindricalLeft10<CODEFEATURE_AVX1>,
	tuned_PredictPlanarGradientAndCount8<CODEFEATURE_AVX1>,
	tuned_PredictPlanarGradient8<CODEFEATURE_AVX1>,
	tuned_RestorePlanarGradient8<CODEFEATURE_AVX1>,
	tuned_PredictPlanarGradientAndCount10<CODEFEATURE_AVX1>,
	tuned_RestorePlanarGradient10<CODEFEATURE_AVX1>,
};


const TUNEDFUNC_HUFFMAN_ENCODE tfnHuffmanEncodeI686 = {
	NULL,
	{ 0, 0 },
	i686_HuffmanEncode8,
	i686_HuffmanEncode10,
};


const TUNEDFUNC_HUFFMAN_DECODE tfnHuffmanDecodeI686 = {
	NULL,
	{ 0, 0 },
	i686_HuffmanDecode8,
	i686_HuffmanDecode10,
};

#ifdef __x86_64__
const TUNEDFUNC_HUFFMAN_DECODE tfnHuffmanDecodeBMI2 = {
	&tfnHuffmanDecodeI686,
	{ 0, FEATURE1_BMI2 },
	bmi2_HuffmanDecode8,
	bmi2_HuffmanDecode10,
};
#endif


const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBSSE41 = {
	NULL,
	{ FEATURE0_SSE41, 0 },
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT601Coefficient, CARGBColorOrder>,
	},
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_SSE41, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_SSE41, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_SSE41, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_SSE41, CBT709Coefficient, CARGBColorOrder>,
	},
};

const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBAVX1 = {
	&tfnConvertYUVRGBSSE41,
	{ FEATURE0_AVX1, 0 },
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
	},
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
	},
};

const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBAVX2 = {
	&tfnConvertYUVRGBAVX1,
	{ FEATURE0_AVX2, 0 },
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
	},
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX2, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
	},
};

const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBAVX512ICL = {
	&tfnConvertYUVRGBAVX2,
	{ FEATURE0_AVX512F | FEATURE0_AVX512_VNNI | FEATURE0_AVX512_VBMI, 0 },
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT601Coefficient, CARGBColorOrder>,
	},
	{
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY2ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY2<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY4ToRGB<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY4<CODEFEATURE_AVX512_ICL, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertULY0ToRGB<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CBGRAColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CRGBColorOrder>,
		tuned_ConvertRGBToULY0<CODEFEATURE_AVX1, CBT709Coefficient, CARGBColorOrder>,
	},
};


const TUNEDFUNC_CONVERT_SHUFFLE tfnConvertShuffleSSE41 = {
	NULL,
	{ FEATURE0_SSE41, 0 },
	tuned_ConvertRGBToULRG<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertRGBToULRG<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_SSE41, CUYVYColorOrder>,
	cpp_ConvertRGBToUQRG<CB48rColorOrder>,
	tuned_ConvertRGBToUQRG<CODEFEATURE_SSE41, CB64aColorOrder>,
	tuned_ConvertB64aToUQRA<CODEFEATURE_SSE41>,
	cpp_ConvertUQRGToRGB<CB48rColorOrder>,
	tuned_ConvertUQRGToRGB<CODEFEATURE_SSE41, CB64aColorOrder>,
	tuned_ConvertUQRAToB64a<CODEFEATURE_SSE41>,
	cpp_ConvertV210ToUQY2,
	cpp_ConvertUQY2ToV210,
	tuned_ConvertR210ToUQRG<CODEFEATURE_SSE41>,
	tuned_ConvertUQRGToR210<CODEFEATURE_SSE41>,
	tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
	tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_SSE41, VALUERANGE::NOROUND>,
	tuned_ConvertHostEndian10ToLittleEndian16<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_SSE41, VALUERANGE::NOROUND>,
	tuned_ConvertPlanarHostEndian10ToPackedUVLittleEndian16<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
};

const TUNEDFUNC_CONVERT_SHUFFLE tfnConvertShuffleAVX1 = {
	&tfnConvertShuffleSSE41,
	{ FEATURE0_AVX1, 0 },
	tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX1, CUYVYColorOrder>,
	cpp_ConvertRGBToUQRG<CB48rColorOrder>,
	tuned_ConvertRGBToUQRG<CODEFEATURE_AVX1, CB64aColorOrder>,
	tuned_ConvertB64aToUQRA<CODEFEATURE_AVX1>,
	cpp_ConvertUQRGToRGB<CB48rColorOrder>,
	tuned_ConvertUQRGToRGB<CODEFEATURE_AVX1, CB64aColorOrder>,
	tuned_ConvertUQRAToB64a<CODEFEATURE_AVX1>,
	cpp_ConvertV210ToUQY2,
	cpp_ConvertUQY2ToV210,
	tuned_ConvertR210ToUQRG<CODEFEATURE_AVX1>,
	tuned_ConvertUQRGToR210<CODEFEATURE_AVX1>,
	tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertHostEndian10ToLittleEndian16<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertPlanarHostEndian10ToPackedUVLittleEndian16<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
};

const TUNEDFUNC_CONVERT_SHUFFLE tfnConvertShuffleAVX2 = {
	&tfnConvertShuffleAVX1,
	{ FEATURE0_AVX2, 0 },
	tuned_ConvertRGBToULRG<CODEFEATURE_AVX2, CBGRColorOrder>,
	tuned_ConvertRGBToULRG<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX2, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2<CODEFEATURE_AVX2, CUYVYColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_AVX2, CBGRColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX2, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422<CODEFEATURE_AVX2, CUYVYColorOrder>,
	cpp_ConvertRGBToUQRG<CB48rColorOrder>,
	tuned_ConvertRGBToUQRG<CODEFEATURE_AVX1, CB64aColorOrder>,
	tuned_ConvertB64aToUQRA<CODEFEATURE_AVX1>,
	cpp_ConvertUQRGToRGB<CB48rColorOrder>,
	tuned_ConvertUQRGToRGB<CODEFEATURE_AVX1, CB64aColorOrder>,
	tuned_ConvertUQRAToB64a<CODEFEATURE_AVX1>,
	cpp_ConvertV210ToUQY2,
	cpp_ConvertUQY2ToV210,
	tuned_ConvertR210ToUQRG<CODEFEATURE_AVX1>,
	tuned_ConvertUQRGToR210<CODEFEATURE_AVX1>,
	tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertLittleEndian16ToHostEndian10<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertHostEndian10ToLittleEndian16<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertPlanarHostEndian10ToPackedUVLittleEndian16<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
};


extern const TUNEDFUNC_SYMPACK tfnSymPackSSE41 = {
	&tfnSymPackCPP,
	{ FEATURE0_SSE41, 0 },
	tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41>,
	tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_SSE41>,
	tuned_Pack8SymWithDiff8<CODEFEATURE_SSE41>,
	tuned_Unpack8SymWithDiff8<CODEFEATURE_SSE41>,
};

extern const TUNEDFUNC_SYMPACK tfnSymPackAVX1 = {
	&tfnSymPackSSE41,
	{ FEATURE0_AVX1, 0 },
	tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1>,
	tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX1>,
	tuned_Pack8SymWithDiff8<CODEFEATURE_AVX1>,
	tuned_Unpack8SymWithDiff8<CODEFEATURE_AVX1>,
};

extern const TUNEDFUNC_SYMPACK tfnSymPackAVX2 = {
	&tfnSymPackAVX1,
	{ FEATURE0_AVX2, FEATURE1_BMI2 },
	tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2>,
	tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2>,
	tuned_Pack8SymWithDiff8<CODEFEATURE_AVX2>,
	tuned_Unpack8SymWithDiff8<CODEFEATURE_AVX2>,
};

#ifdef __x86_64__
extern const TUNEDFUNC_SYMPACK tfnSymPackAVX512ICL = {
	&tfnSymPackAVX2,
	{ FEATURE0_AVX512F | FEATURE0_AVX512_VBMI2, FEATURE1_BMI2 },
	tuned_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL>,
	tuned_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX512_ICL>,
	tuned_Pack8SymWithDiff8<CODEFEATURE_AVX512_ICL>,
	tuned_Unpack8SymWithDiff8<CODEFEATURE_AVX512_ICL>,
};
#endif


const TUNEDFUNC_CONVERT_PREDICT tfnConvertPredictSSE41 = {
	&tfnConvertPredictCPP,
	{ FEATURE0_SSE41, 0 },
	tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertPackedUVToPlanar_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertPlanarToPackedUV_RestoreCylindricalLeft<CODEFEATURE_SSE41>,
	tuned_ConvertPlanarToPackedUV_RestorePlanarGradient<CODEFEATURE_SSE41>,
	tuned_ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41>,
	tuned_ConvertB64aToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertB64aToUQRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertB64aToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertB64aToUQRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertUQRGToB64a_RestoreCylindricalLeft<CODEFEATURE_SSE41>,
	tuned_ConvertUQRAToB64a_RestoreCylindricalLeft<CODEFEATURE_SSE41>,
	tuned_ConvertUQRGToB64a_RestorePlanarGradient<CODEFEATURE_SSE41>,
	tuned_ConvertUQRAToB64a_RestorePlanarGradient<CODEFEATURE_SSE41>,
	tuned_ConvertR210ToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertR210ToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41>,
	tuned_ConvertUQRGToR210_RestoreCylindricalLeft<CODEFEATURE_SSE41>,
	tuned_ConvertUQRGToR210_RestorePlanarGradient<CODEFEATURE_SSE41>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>,
	tuned_ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_SSE41>,
	tuned_ConvertHostEndian16ToLittleEndian16_RestorePlanarGradient<CODEFEATURE_SSE41>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, VALUERANGE::NOROUND>,
	tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_SSE41>,
	tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient<CODEFEATURE_SSE41>,
};

const TUNEDFUNC_CONVERT_PREDICT tfnConvertPredictAVX1 = {
	&tfnConvertPredictSSE41,
	{ FEATURE0_AVX1, 0 },
	tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertPackedUVToPlanar_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertPackedUVToPlanar_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertPackedUVToPlanar_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertPlanarToPackedUV_RestoreCylindricalLeft<CODEFEATURE_AVX1>,
	tuned_ConvertPlanarToPackedUV_RestorePlanarGradient<CODEFEATURE_AVX1>,
	tuned_ConvertPlanarToPackedUV_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1>,
	tuned_ConvertB64aToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertB64aToUQRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertB64aToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertB64aToUQRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertUQRGToB64a_RestoreCylindricalLeft<CODEFEATURE_AVX1>,
	tuned_ConvertUQRAToB64a_RestoreCylindricalLeft<CODEFEATURE_AVX1>,
	tuned_ConvertUQRGToB64a_RestorePlanarGradient<CODEFEATURE_AVX1>,
	tuned_ConvertUQRAToB64a_RestorePlanarGradient<CODEFEATURE_AVX1>,
	tuned_ConvertR210ToUQRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertR210ToUQRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1>,
	tuned_ConvertUQRGToR210_RestoreCylindricalLeft<CODEFEATURE_AVX1>,
	tuned_ConvertUQRGToR210_RestorePlanarGradient<CODEFEATURE_AVX1>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertLittleEndian16ToHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertHostEndian16ToLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_AVX1>,
	tuned_ConvertHostEndian16ToLittleEndian16_RestorePlanarGradient<CODEFEATURE_AVX1>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::LIMITED>,
	tuned_ConvertPackedUVLittleEndian16ToPlanarHostEndian10_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, VALUERANGE::NOROUND>,
	tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestoreCylindricalLeft<CODEFEATURE_AVX1>,
	tuned_ConvertPlanarHostEndian16ToPackedUVLittleEndian16_RestorePlanarGradient<CODEFEATURE_AVX1>,
};


const TUNEDFUNC_CONVERT_SYMPACK tfnConvertSymPackSSE41 = {
	&tfnConvertSymPackCPP,
	{ FEATURE0_SSE41, 0 },
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_SSE41, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_SSE41, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_SSE41, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_SSE41, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_SSE41, CUYVYColorOrder>,
};

const TUNEDFUNC_CONVERT_SYMPACK tfnConvertSymPackAVX1 = {
	&tfnConvertSymPackSSE41,
	{ FEATURE0_AVX1, 0 },
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX1, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX1, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX1, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX1, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX1, CUYVYColorOrder>,
};

const TUNEDFUNC_CONVERT_SYMPACK tfnConvertSymPackAVX2 = {
	&tfnConvertSymPackAVX1,
	{ FEATURE0_AVX2, 0 },
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX2, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX2, CUYVYColorOrder>,
};

#ifdef __x86_64__
const TUNEDFUNC_CONVERT_SYMPACK tfnConvertSymPackAVX512ICL = {
	&tfnConvertSymPackAVX2,
	{ FEATURE0_AVX512F | FEATURE0_AVX512_VBMI2, 0 },
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CBGRColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>,
	tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CARGBColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>,
	tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CARGBColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX512_ICL, CBGRColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>,
	tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX512_ICL, CARGBColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX512_ICL, CBGRAColorOrder>,
	tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX512_ICL, CARGBColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CYUYVColorOrder>,
	tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CUYVYColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CYUYVColorOrder>,
	tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX512_ICL, CUYVYColorOrder>,
};
#endif


const TUNEDFUNC tfnRoot = {
	&tfnPredictAVX1,
	&tfnHuffmanEncodeI686,
#ifdef __x86_64__
	&tfnHuffmanDecodeBMI2,
	& tfnConvertYUVRGBAVX512ICL,
#else
	&tfnHuffmanDecodeI686,
	&tfnConvertYUVRGBAVX2,
#endif
	&tfnConvertShuffleAVX2,
#ifdef __x86_64__
	&tfnSymPackAVX512ICL,
#else
	&tfnSymPackAVX2,
#endif
	&tfnConvertPredictAVX1,
#ifdef __x86_64__
	&tfnConvertSymPackAVX512ICL,
#else
	&tfnConvertSymPackAVX2,
#endif
};

uint32_t dwSupportedFeatures[FEATURESIZE];


class CTunedFuncInitializer
{
public:
	CTunedFuncInitializer()
	{
		CLogInitializer::Initialize();

		if (GetEnvFlagBool("UTVIDEO_DISABLE_TUNED_FUNC"))
		{
			LOGPRINTF("TunedFunc is disabled. CPU feature detection is skipped.");
			return;
		}

		cpuid_result cpuid_0   = { 0, 0, 0, 0 };
		cpuid_result cpuid_1   = { 0, 0, 0, 0 };
		cpuid_result cpuid_7_0 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex0 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex1 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex2 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex3 = { 0, 0, 0, 0 };
		cpuid_result cpuid_ex4 = { 0, 0, 0, 0 };

		char vendor[16];
		char procbrand[64];
		unsigned int family, model;
		const char *maname;
		uint32_t ma;

		memset(dwSupportedFeatures, 0, sizeof(dwSupportedFeatures));

		cpuid(&cpuid_0, 0, 0);
		*(uint32_t *)(vendor+0) = cpuid_0.ebx;
		*(uint32_t *)(vendor+4) = cpuid_0.edx;
		*(uint32_t *)(vendor+8) = cpuid_0.ecx;
		vendor[12] = '\0';
		LOGPRINTF("CPUID     vendor=\"%s\"", vendor);

		cpuid(&cpuid_ex0, 0x80000000, 0);

		if (cpuid_ex0.eax >= 0x80000001)
		{
			cpuid(&cpuid_ex1, 0x80000001, 0);
		}

		if (cpuid_ex0.eax >= 0x80000004)
		{
			cpuid(&cpuid_ex2, 0x80000002, 0);
			cpuid(&cpuid_ex3, 0x80000003, 0);
			cpuid(&cpuid_ex4, 0x80000004, 0);
			*(uint32_t *)(procbrand+ 0) = cpuid_ex2.eax;
			*(uint32_t *)(procbrand+ 4) = cpuid_ex2.ebx;
			*(uint32_t *)(procbrand+ 8) = cpuid_ex2.ecx;
			*(uint32_t *)(procbrand+12) = cpuid_ex2.edx;
			*(uint32_t *)(procbrand+16) = cpuid_ex3.eax;
			*(uint32_t *)(procbrand+20) = cpuid_ex3.ebx;
			*(uint32_t *)(procbrand+24) = cpuid_ex3.ecx;
			*(uint32_t *)(procbrand+28) = cpuid_ex3.edx;
			*(uint32_t *)(procbrand+32) = cpuid_ex4.eax;
			*(uint32_t *)(procbrand+36) = cpuid_ex4.ebx;
			*(uint32_t *)(procbrand+40) = cpuid_ex4.ecx;
			*(uint32_t *)(procbrand+44) = cpuid_ex4.edx;
			procbrand[48] = '\0';
			LOGPRINTF("CPUID     procbrand=\"%s\"", procbrand);
		}

		if (cpuid_0.eax >= 1)
		{
			cpuid(&cpuid_1, 1, 0);
		}

		if (cpuid_0.eax >= 7)
		{
			cpuid(&cpuid_7_0, 7, 0);
		}


		family = ((cpuid_1.eax >> 20) & 0xff) + ((cpuid_1.eax >> 8) & 0xf);
		model  = ((cpuid_1.eax >> 12) & 0xf0) | ((cpuid_1.eax >> 4) & 0xf);
		LOGPRINTF("CPUID     family=%02XH model=%02XH", family, model);
		for (int i = 0; i < _countof(mafm); i++)
		{
			for (int j = 0; j < _countof(mafm[i].fm); j++)
			{
				if (mafm[i].fm[j] == ((family << 8) | model))
				{
					ma = mafm[i].ma;
					maname = mafm[i].name;
					goto ma_found;
				}
			}
		}
		maname = "Unknown";
		ma = MA_UNKNOWN;
ma_found:
		LOGPRINTF("CPUID     march=\"%s\" (ID %08X)", maname, ma);


		if (cpuid_7_0.ebx & (1 << 3))
		{
			LOGPRINTF("supports BMI1");
			dwSupportedFeatures[1] |= FEATURE1_BMI1;
		}

		if (cpuid_7_0.ebx & (1 << 8))
		{
			LOGPRINTF("supports BMI2");
			dwSupportedFeatures[1] |= FEATURE1_BMI2;
		}

		if (cpuid_ex1.ecx & (1 << 5))
		{
			LOGPRINTF("supports LZCNT");
			dwSupportedFeatures[1] |= FEATURE1_LZCNT;
		}

		if (cpuid_1.ecx & (1 << 23))
		{
			LOGPRINTF("supports POPCNT");
			dwSupportedFeatures[1] |= FEATURE1_POPCNT;
		}

		if (cpuid_1.ecx & (1 << 22))
		{
			LOGPRINTF("supports MOVBE");
			dwSupportedFeatures[1] |= FEATURE1_MOVBE;
		}


		if (cpuid_1.ecx & (1 << 27))
		{
			LOGPRINTF("supports OSXSAVE");

			xgetbv_result xgetbv_0 = { 0, 0 };

			xgetbv(&xgetbv_0, 0);

			if ((xgetbv_0.eax & 6) == 6)
			{
				LOGPRINTF("supports XMM/YMM state by OS");

				if (cpuid_7_0.ebx & (1 << 5))
				{
					LOGPRINTF("supports AVX2");
					dwSupportedFeatures[0] |= FEATURE0_AVX2;
				}

				if (cpuid_1.ecx & (1 << 28))
				{
					LOGPRINTF("supports AVX1");
					dwSupportedFeatures[0] |= FEATURE0_AVX1;
				}

				if ((xgetbv_0.eax & 0xe0) == 0xe0)
				{
					LOGPRINTF("supports opmask/ZMM state by OS");

					if (cpuid_7_0.ebx & (1 << 16))
					{
						LOGPRINTF("supports AVX512F");
						dwSupportedFeatures[0] |= FEATURE0_AVX512F;
					}

					if (cpuid_7_0.ebx & (1 << 28))
					{
						LOGPRINTF("supports AVX512CD");
						dwSupportedFeatures[0] |= FEATURE0_AVX512CD;
					}

					if (cpuid_7_0.ebx & (1 << 27))
					{
						LOGPRINTF("supports AVX512ER");
						dwSupportedFeatures[0] |= FEATURE0_AVX512ER;
					}

					if (cpuid_7_0.ebx & (1 << 26))
					{
						LOGPRINTF("supports AVX512PF");
						dwSupportedFeatures[0] |= FEATURE0_AVX512PF;
					}

					if (cpuid_7_0.ebx & (1 << 17))
					{
						LOGPRINTF("supports AVX512DQ");
						dwSupportedFeatures[0] |= FEATURE0_AVX512DQ;
					}

					if (cpuid_7_0.ebx & (1 << 30))
					{
						LOGPRINTF("supports AVX512BW");
						dwSupportedFeatures[0] |= FEATURE0_AVX512BW;
					}

					if (cpuid_7_0.ebx & (1 << 31))
					{
						LOGPRINTF("supports AVX512VL");
						dwSupportedFeatures[0] |= FEATURE0_AVX512VL;
					}

					if (cpuid_7_0.ebx & (1 << 21))
					{
						LOGPRINTF("supports AVX512_IFMA");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_IFMA;
					}

					if (cpuid_7_0.ecx & (1 << 1))
					{
						LOGPRINTF("supports AVX512_VBMI");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_VBMI;
					}

					if (cpuid_7_0.edx & (1 << 3))
					{
						LOGPRINTF("supports AVX512_4FMAPS");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_4FMAPS;
					}

					if (cpuid_7_0.edx & (1 << 2))
					{
						LOGPRINTF("supports AVX512_4VNNIW");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_4VNNIW;
					}

					if (cpuid_7_0.ecx & (1 << 11))
					{
						LOGPRINTF("supports AVX512_VNNI");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_VNNI;
					}

					if (cpuid_7_0.ecx & (1 << 14))
					{
						LOGPRINTF("supports AVX512_VPOPCNTDQ");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_VPOPCNTDQ;
					}

					if (cpuid_7_0.ecx & (1 << 6))
					{
						LOGPRINTF("supports AVX512_VBMI2");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_VBMI2;
					}

					if (cpuid_7_0.ecx & (1 << 12))
					{
						LOGPRINTF("supports AVX512_BITALG");
						dwSupportedFeatures[0] |= FEATURE0_AVX512_BITALG;
					}
				}
				else
				{
					LOGPRINTF("does not support opmask/ZMM state by OS");
				}
			}
			else
			{
				LOGPRINTF("does not support XMM/YMM state by OS");
			}
		}
		else
		{
			LOGPRINTF("does not support OSXSAVE");
		}

		if (cpuid_1.ecx & (1 << 20))
		{
			LOGPRINTF("supports SSE4.2");
			dwSupportedFeatures[0] |= FEATURE0_SSE42;
		}

		if (cpuid_1.ecx & (1 << 19))
		{
			LOGPRINTF("supports SSE4.1");
			dwSupportedFeatures[0] |= FEATURE0_SSE41;
		}

		if (cpuid_1.ecx & (1 << 9))
		{
			LOGPRINTF("supports SSSE3");
			dwSupportedFeatures[0] |= FEATURE0_SSSE3;
		}

		if (cpuid_1.ecx & (1 << 0))
		{
			LOGPRINTF("supports SSE3");
			dwSupportedFeatures[0] |= FEATURE0_SSE3;
		}

		if (cpuid_1.edx & (1 << 26))
		{
			LOGPRINTF("supports SSE2");
			dwSupportedFeatures[0] |= FEATURE0_SSE2;
		}

		ResolveTunedFunc(&tfnRoot, dwSupportedFeatures);
	}
} tfi;
