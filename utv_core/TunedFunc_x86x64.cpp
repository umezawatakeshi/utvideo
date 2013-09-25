/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"
#include "Predict.h"
#include "HuffmanCode.h"
#include "Convert.h"
#include "ColorOrder.h"
#include "Coefficient.h"
#include "CPUID.h"

#define FEATURE0_SSE2     (1 <<  0)
#define FEATURE0_SSE3     (1 <<  1)
#define FEATURE0_SSSE3    (1 <<  2)
#define FEATURE0_SSE41    (1 <<  3)
#define FEATURE0_SSE42    (1 <<  4)
#define FEATURE0_AVX1     (1 <<  5)
#define FEATURE0_AVX2     (1 <<  6)
#define FEATURE0_FMA3     (1 <<  7) /* 3-operand FMA */
//#define FEATURE0_AVX512F  (1 <<  8) /* AVX-512 Foundation */
//#define FEATURE0_AVX512PF (1 <<  9) /* AVX-512 Prefetch */
//#define FEATURE0_AVX512ER (1 << 10) /* AVX-512 Exponential & Reciprocal */
//#define FEATURE0_AVX512CD (1 << 11) /* AVX-512 Conflict Detection */

#define FEATURE1_MOVBE    (1 <<  0)
#define FEATURE1_POPCNT   (1 <<  1)
#define FEATURE1_LZCNT    (1 <<  2)
#define FEATURE1_BMI1     (1 <<  3)
#define FEATURE1_BMI2     (1 <<  4)

#ifdef __i386__
const TUNEDFUNC_PREDICT tfnPredictI686 = {
	NULL,
	{ 0, 0 },
	cpp_PredictWrongMedianAndCount,
	cpp_PredictWrongMedianAndCount,
	cpp_PredictLeftAndCount,
	i686_RestoreWrongMedian_align1,
};
#endif

const TUNEDFUNC_PREDICT tfnPredictSSE2 = {
#ifdef __i386__
	&tfnPredictI686,
#else
	NULL,
#endif
	{ FEATURE0_SSE2, 0 },
	sse2_PredictWrongMedianAndCount_align16,
	sse2_PredictWrongMedianAndCount_align1,
	sse2_PredictLeftAndCount_align1,
	sse1mmx_RestoreWrongMedian_align1,
};


const TUNEDFUNC_HUFFMAN_ENCODE tfnHuffmanEncodeI686 = {
	NULL,
	{ 0, 0 },
	i686_HuffmanEncode,
};


const TUNEDFUNC_HUFFMAN_DECODE tfnHuffmanDecodeI686 = {
	NULL,
	{ 0, 0 },
	i686_HuffmanDecode,
	i686_HuffmanDecodeAndAccum,
	i686_HuffmanDecodeAndAccumStep2,
	i686_HuffmanDecodeAndAccumStep3,
	i686_HuffmanDecodeAndAccumStep4,
	i686_HuffmanDecodeAndAccumStep3ForBGRBlue,
	i686_HuffmanDecodeAndAccumStep3ForBGRRed,
	i686_HuffmanDecodeAndAccumStep4ForBGRXBlue,
	i686_HuffmanDecodeAndAccumStep4ForBGRXRed,
	i686_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha,
	i686_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha,
};

#ifdef __x86_64__
const TUNEDFUNC_HUFFMAN_DECODE tfnHuffmanDecodeBMI2 = {
	&tfnHuffmanDecodeI686,
	{ 0, FEATURE1_BMI2 },
	bmi2_HuffmanDecode,
	bmi2_HuffmanDecodeAndAccum,
	bmi2_HuffmanDecodeAndAccumStep2,
	bmi2_HuffmanDecodeAndAccumStep3,
	bmi2_HuffmanDecodeAndAccumStep4,
	bmi2_HuffmanDecodeAndAccumStep3ForBGRBlue,
	bmi2_HuffmanDecodeAndAccumStep3ForBGRRed,
	bmi2_HuffmanDecodeAndAccumStep4ForBGRXBlue,
	bmi2_HuffmanDecodeAndAccumStep4ForBGRXRed,
	bmi2_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha,
	bmi2_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha,
};
#endif


const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBSSE2 = {
	&tfnConvertYUVRGBCPP,
	{ 0, 0 },
	{
		sse2_ConvertULY2ToBGR,
		sse2_ConvertULY2ToBGRX,
		sse2_ConvertULY2ToRGB,
		sse2_ConvertULY2ToXRGB,
		sse2_ConvertBGRToULY2,
		sse2_ConvertBGRXToULY2,
		sse2_ConvertRGBToULY2,
		sse2_ConvertXRGBToULY2,
	},
	{
		sse2_ConvertULH2ToBGR,
		sse2_ConvertULH2ToBGRX,
		sse2_ConvertULH2ToRGB,
		sse2_ConvertULH2ToXRGB,
		sse2_ConvertBGRToULH2,
		sse2_ConvertBGRXToULH2,
		sse2_ConvertRGBToULH2,
		sse2_ConvertXRGBToULH2,
	},
};

const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBSSSE3 = {
	&tfnConvertYUVRGBSSE2,
	{ FEATURE0_SSSE3, 0 },
	{
		sse2_ConvertULY2ToBGR,
		sse2_ConvertULY2ToBGRX,
		sse2_ConvertULY2ToRGB,
		sse2_ConvertULY2ToXRGB,
		ssse3_ConvertBGRToULY2,
		ssse3_ConvertBGRXToULY2,
		ssse3_ConvertRGBToULY2,
		ssse3_ConvertXRGBToULY2,
	},
	{
		sse2_ConvertULH2ToBGR,
		sse2_ConvertULH2ToBGRX,
		sse2_ConvertULH2ToRGB,
		sse2_ConvertULH2ToXRGB,
		ssse3_ConvertBGRToULH2,
		ssse3_ConvertBGRXToULH2,
		ssse3_ConvertRGBToULH2,
		ssse3_ConvertXRGBToULH2,
	},
};

const TUNEDFUNC_CONVERT_YUVRGB tfnConvertYUVRGBSSE41 = {
	&tfnConvertYUVRGBSSSE3,
	{ FEATURE0_SSE41, 0 },
	{
		sse41_ConvertULY2ToBGR,
		sse41_ConvertULY2ToBGRX,
		sse41_ConvertULY2ToRGB,
		sse41_ConvertULY2ToXRGB,
		ssse3_ConvertBGRToULY2,
		ssse3_ConvertBGRXToULY2,
		ssse3_ConvertRGBToULY2,
		ssse3_ConvertXRGBToULY2,
	},
	{
		sse41_ConvertULH2ToBGR,
		sse41_ConvertULH2ToBGRX,
		sse41_ConvertULH2ToRGB,
		sse41_ConvertULH2ToXRGB,
		ssse3_ConvertBGRToULH2,
		ssse3_ConvertBGRXToULH2,
		ssse3_ConvertRGBToULH2,
		ssse3_ConvertXRGBToULH2,
	},
};


const TUNEDFUNC_CONVERT_SHUFFLE tfnConvertShuffleSSSE3 = {
	&tfnConvertShuffleCPP,
	{ FEATURE0_SSSE3, 0 },
	ssse3_ConvertBGRToULRG,
	ssse3_ConvertBGRXToULRG,
	ssse3_ConvertXRGBToULRG,
	ssse3_ConvertBGRAToULRA,
	ssse3_ConvertARGBToULRA,
	ssse3_ConvertYUYVToULY2,
	ssse3_ConvertUYVYToULY2,
	ssse3_ConvertULRGToBGR,
	ssse3_ConvertULRGToBGRX,
	ssse3_ConvertULRGToXRGB,
	ssse3_ConvertULRAToBGRA,
	ssse3_ConvertULRAToARGB,
	ssse3_ConvertULY2ToYUYV,
	ssse3_ConvertULY2ToUYVY,
};

const TUNEDFUNC tfnRoot = {
	&tfnPredictSSE2,
	&tfnHuffmanEncodeI686,
#ifdef __x86_64__
	&tfnHuffmanDecodeBMI2,
#else
	&tfnHuffmanDecodeI686,
#endif
	&tfnConvertYUVRGBSSE41,
	&tfnConvertShuffleSSSE3,
};

uint32_t dwSupportedFeatures[FEATURESIZE];


class CTunedFuncInitializer
{
public:
	static inline void cpuid(cpuid_result *result, uint32_t leaf, uint32_t subleaf)
	{
		::cpuid(result, leaf, subleaf);
#ifdef _DEBUG
		char buf[16];
		if (leaf >= 0x80000000)
			sprintf(buf, "ex%d", leaf - 0x80000000);
		else if (leaf == 7)
			sprintf(buf, "7.%d", subleaf);
		else
			sprintf(buf, "%d", leaf);
		_RPT5(_CRT_WARN, "CPUID.%-3s EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", buf,
			result->eax, result->ebx, result->ecx, result->edx);
#endif
	}

	static inline void xgetbv(xgetbv_result *result, uint32_t idx)
	{
		::xgetbv(result, idx);
#ifdef _DEBUG
		_RPT2(_CRT_WARN, "XGETBV.%-2d EAX=%08X EDX=%08X\n", result->eax, result->edx);
#endif
	}

	CTunedFuncInitializer()
	{
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

		memset(dwSupportedFeatures, 0, sizeof(dwSupportedFeatures));

		cpuid(&cpuid_0, 0, 0);
		*(uint32_t *)(vendor+0) = cpuid_0.ebx;
		*(uint32_t *)(vendor+4) = cpuid_0.edx;
		*(uint32_t *)(vendor+8) = cpuid_0.ecx;
		vendor[12] = '\0';
		_RPT1(_CRT_WARN, "CPUID     vendor=\"%s\"\n", vendor);

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
			_RPT1(_CRT_WARN, "CPUID     procbrand=\"%s\"\n", procbrand);
		}

		if (cpuid_0.eax >= 1)
		{
			cpuid(&cpuid_1, 1, 0);
		}

		if (cpuid_0.eax >= 7)
		{
			cpuid(&cpuid_7_0, 7, 0);
		}


		if (cpuid_7_0.ebx & (1 << 3))
		{
			_RPT0(_CRT_WARN, "supports BMI1\n");
			dwSupportedFeatures[1] |= FEATURE1_BMI1;
		}

		if (cpuid_7_0.ebx & (1 << 8))
		{
			_RPT0(_CRT_WARN, "supports BMI2\n");
			dwSupportedFeatures[1] |= FEATURE1_BMI2;
		}

		if (cpuid_ex1.ecx & (1 << 5))
		{
			_RPT0(_CRT_WARN, "supports LZCNT\n");
			dwSupportedFeatures[1] |= FEATURE1_LZCNT;
		}

		if (cpuid_1.ecx & (1 << 23))
		{
			_RPT0(_CRT_WARN, "supports POPCNT\n");
			dwSupportedFeatures[1] |= FEATURE1_POPCNT;
		}

		if (cpuid_1.ecx & (1 << 22))
		{
			_RPT0(_CRT_WARN, "supports MOVBE\n");
			dwSupportedFeatures[1] |= FEATURE1_MOVBE;
		}


		if (cpuid_1.ecx & (1 << 27))
		{
			_RPT0(_CRT_WARN, "supports OSXSAVE\n");

			xgetbv_result xgetbv_0 = { 0, 0 };

			xgetbv(&xgetbv_0, 0);

			if ((xgetbv_0.eax & 6) == 6)
			{
				_RPT0(_CRT_WARN, "supports XMM/YMM state by OS\n");

				if (cpuid_7_0.ebx & (1 << 5))
				{
					_RPT0(_CRT_WARN, "supports AVX2\n");
					dwSupportedFeatures[0] |= FEATURE0_AVX2;
				}

				if (cpuid_1.ecx & (1 << 28))
				{
					_RPT0(_CRT_WARN, "supports AVX1\n");
					dwSupportedFeatures[0] |= FEATURE0_AVX1;
				}
			}
			else
			{
				_RPT0(_CRT_WARN, "does not support XMM/YMM state by OS\n");
			}
		}
		else
		{
			_RPT0(_CRT_WARN, "does not support OSXSAVE\n");
		}

		if (cpuid_1.ecx & (1 << 20))
		{
			_RPT0(_CRT_WARN, "supports SSE4.2\n");
			dwSupportedFeatures[0] |= FEATURE0_SSE42;
		}

		if (cpuid_1.ecx & (1 << 19))
		{
			_RPT0(_CRT_WARN, "supports SSE4.1\n");
			dwSupportedFeatures[0] |= FEATURE0_SSE41;
		}

		if (cpuid_1.ecx & (1 << 9))
		{
			_RPT0(_CRT_WARN, "supports SSSE3\n");
			dwSupportedFeatures[0] |= FEATURE0_SSSE3;
		}

		if (cpuid_1.ecx & (1 << 0))
		{
			_RPT0(_CRT_WARN, "supports SSE3\n");
			dwSupportedFeatures[0] |= FEATURE0_SSE3;
		}

		if (cpuid_1.edx & (1 << 26))
		{
			_RPT0(_CRT_WARN, "supports SSE2\n");
			dwSupportedFeatures[0] |= FEATURE0_SSE2;
		}


		ResolveTunedFunc(&tfnRoot, dwSupportedFeatures);
	}
} tfi;
