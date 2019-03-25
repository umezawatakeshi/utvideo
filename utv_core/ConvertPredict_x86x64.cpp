/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1) && !defined(GENERATE_AVX2)
#error
#endif

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertBGRToULRG_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	typedef CBGRColorOrder T;

	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			auto planar = tuned_ConvertPackedBGRToPlanarElement<F, false>(pp);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;

			b += 16;
			g += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));

		for (; pp < p + cbWidth; pp += 3)
		{
			uint8_t gg = pp[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;

			b += 1;
			g += 1;
			r += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			auto planar = tuned_ConvertPackedBGRToPlanarElement<F, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32)))
			);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;

			b += 16;
			g += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));

		for (; pp < p + cbWidth; pp += 3)
		{
			uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;

			b += 1;
			g += 1;
			r += 1;
		}
	}

	gprevb = 0;
	bprevb = 0;
	rprevb = 0;

	uint8_t gtopprevb = 0;
	uint8_t btopprevb = 0;
	uint8_t rtopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);
		__m128i gtopprev = _mm_slli_si128(_mm_cvtsi32_si128(gtopprevb), 15);
		__m128i btopprev = _mm_slli_si128(_mm_cvtsi32_si128(btopprevb), 15);
		__m128i rtopprev = _mm_slli_si128(_mm_cvtsi32_si128(rtopprevb), 15);

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			auto planar = tuned_ConvertPackedBGRToPlanarElement<F, true>(pp);
			auto top = tuned_ConvertPackedBGRToPlanarElement<F, true>(pp - scbStride);
			_mm_storeu_si128((__m128i *)g, tuned_PredictWrongMedianAndCount8Element<F>(gtopprev, top.g, gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictWrongMedianAndCount8Element<F>(btopprev, top.b, bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictWrongMedianAndCount8Element<F>(rtopprev, top.r, rprev, planar.r, pRCountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			gtopprev = top.g;
			btopprev = top.b;
			rtopprev = top.r;

			b += 16;
			g += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
		gtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(gtopprev, 15));
		btopprevb = _mm_cvtsi128_si32(_mm_srli_si128(btopprev, 15));
		rtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(rtopprev, 15));

		for (; pp < p + cbWidth; pp += 3)
		{
			uint8_t gg = pp[T::G];
			uint8_t gtop = (pp - scbStride)[T::G];
			*g = gg - median<uint8_t>(gprevb, gtop, gprevb + gtop - gtopprevb);
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - gg + 0x80;
			uint8_t btop = (pp - scbStride)[T::B] - gtop + 0x80;
			*b = bb - median<uint8_t>(bprevb, btop, bprevb + btop - btopprevb);
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - gg + 0x80;
			uint8_t rtop = (pp - scbStride)[T::R] - gtop + 0x80;
			*r = rr - median<uint8_t>(rprevb, rtop, rprevb + rtop - rtopprevb);
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			gtopprevb = gtop;
			btopprevb = btop;
			rtopprevb = rtop;

			b += 1;
			g += 1;
			r += 1;
		}
	}
}

template<int F, class T, bool A, PREDICTION_TYPE Pred>
static inline void tuned_ConvertRGBXToULRX_PredictAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;
	uint8_t aprevb = 0x80;

	auto r = pRBegin;
	auto g = pGBegin;
	auto b = pBBegin;
	auto a = pABegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);
		__m128i aprev;
		if (A)
			aprev = _mm_slli_si128(_mm_cvtsi32_si128(aprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, T, A, false>(pp);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictLeftAndCount8Element<F>(aprev, planar.a, pACountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
		if (A)
			aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 15));

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t gg = pp[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A) {
				uint8_t aa = pp[T::A];
				*a = aa - aprevb;
				++pACountTable[*a];
				aprevb = aa;
			}

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();
		__m128i aprev;
		if (A)
			aprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, T, A, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48)))
			);
			_mm_storeu_si128((__m128i *)g, tuned_PredictLeftAndCount8Element<F>(gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictLeftAndCount8Element<F>(bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictLeftAndCount8Element<F>(rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictLeftAndCount8Element<F>(aprev, planar.a, pACountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
		if (A)
			aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 15));

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
			*g = gg - gprevb;
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
			*b = bb - bprevb;
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
			*r = rr - rprevb;
			++pRCountTable[*r];

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			if (A) {
				uint8_t aa = pp[T::A] - (pp - scbStride)[T::A];
				*a = aa - aprevb;
				++pACountTable[*a];
				aprevb = aa;
			}

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	gprevb = 0;
	bprevb = 0;
	rprevb = 0;
	if (A)
		aprevb = 0;

	uint8_t gtopprevb = 0;
	uint8_t btopprevb = 0;
	uint8_t rtopprevb = 0;
	uint8_t atopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_slli_si128(_mm_cvtsi32_si128(gprevb), 15);
		__m128i bprev = _mm_slli_si128(_mm_cvtsi32_si128(bprevb), 15);
		__m128i rprev = _mm_slli_si128(_mm_cvtsi32_si128(rprevb), 15);
		__m128i aprev;
		if (A)
			aprev = _mm_slli_si128(_mm_cvtsi32_si128(aprevb), 15);
		__m128i gtopprev = _mm_slli_si128(_mm_cvtsi32_si128(gtopprevb), 15);
		__m128i btopprev = _mm_slli_si128(_mm_cvtsi32_si128(btopprevb), 15);
		__m128i rtopprev = _mm_slli_si128(_mm_cvtsi32_si128(rtopprevb), 15);
		__m128i atopprev;
		if (A)
			atopprev = _mm_slli_si128(_mm_cvtsi32_si128(atopprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedRGBXToPlanarElement<F, T, A, true>(pp);
			auto top = tuned_ConvertPackedRGBXToPlanarElement<F, T, A, true>(pp - scbStride);
			_mm_storeu_si128((__m128i *)g, tuned_PredictWrongMedianAndCount8Element<F>(gtopprev, top.g, gprev, planar.g, pGCountTable));
			_mm_storeu_si128((__m128i *)b, tuned_PredictWrongMedianAndCount8Element<F>(btopprev, top.b, bprev, planar.b, pBCountTable));
			_mm_storeu_si128((__m128i *)r, tuned_PredictWrongMedianAndCount8Element<F>(rtopprev, top.r, rprev, planar.r, pRCountTable));
			if (A)
				_mm_storeu_si128((__m128i *)a, tuned_PredictWrongMedianAndCount8Element<F>(atopprev, top.a, aprev, planar.a, pACountTable));

			bprev = planar.b;
			gprev = planar.g;
			rprev = planar.r;
			if (A)
				aprev = planar.a;
			gtopprev = top.g;
			btopprev = top.b;
			rtopprev = top.r;
			if (A)
				atopprev = top.a;

			b += 16;
			g += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
		bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
		rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
		if (A)
			aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 15));
		gtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(gtopprev, 15));
		btopprevb = _mm_cvtsi128_si32(_mm_srli_si128(btopprev, 15));
		rtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(rtopprev, 15));
		if (A)
			atopprevb = _mm_cvtsi128_si32(_mm_srli_si128(atopprev, 15));

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t gg = pp[T::G];
			uint8_t gtop = (pp - scbStride)[T::G];
			*g = gg - median<uint8_t>(gprevb, gtop, gprevb + gtop - gtopprevb);
			++pGCountTable[*g];
			uint8_t bb = pp[T::B] - gg + 0x80;
			uint8_t btop = (pp - scbStride)[T::B] - gtop + 0x80;
			*b = bb - median<uint8_t>(bprevb, btop, bprevb + btop - btopprevb);
			++pBCountTable[*b];
			uint8_t rr = pp[T::R] - gg + 0x80;
			uint8_t rtop = (pp - scbStride)[T::R] - gtop + 0x80;
			*r = rr - median<uint8_t>(rprevb, rtop, rprevb + rtop - rtopprevb);
			++pRCountTable[*r];
			if (A)
			{
				uint8_t aa = pp[T::A];
				uint8_t atop = (pp - scbStride)[T::A];
				*a = aa - median<uint8_t>(aprevb, atop, aprevb + atop - atopprevb);
				++pACountTable[*a];
				aprevb = aa;
				atopprevb = atop;
			}

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			gtopprevb = gtop;
			btopprevb = btop;
			rtopprevb = rtop;

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertBGRToULRG_PredictAndCount<F, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
	else
		tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, CYLINDRICAL_LEFT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertBGRToULRG_PredictAndCount<F, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
	else
		tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, PLANAR_GRADIENT>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

template<int F, class T>
void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertBGRToULRG_PredictAndCount<F, CYLINDRICAL_WRONG_MEDIAN>(pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable);
	else
		tuned_ConvertRGBXToULRX_PredictAndCount<F, T, false, CYLINDRICAL_WRONG_MEDIAN>(pGBegin, pBBegin, pRBegin, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, NULL);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable)
{
	tuned_ConvertRGBXToULRX_PredictAndCount<F, T, true, CYLINDRICAL_WRONG_MEDIAN>(pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pGCountTable, pBCountTable, pRCountTable, pACountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBToULRG_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
template void tuned_ConvertRGBAToULRA_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGBegin, uint8_t *pBBegin, uint8_t *pRBegin, uint8_t *pABegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pGCountTable, uint32_t *pBCountTable, uint32_t *pRCountTable, uint32_t *pACountTable);
#endif

//

template<int F, PREDICTION_TYPE Pred>
static inline void tuned_ConvertULRGToBGR_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	typedef CBGRColorOrder T;

	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;

	auto g = pGBegin;
	auto b = pBBegin;
	auto r = pRBegin;

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_set1_epi8(gprevb);
		__m128i bprev = _mm_set1_epi8(bprevb);
		__m128i rprev = _mm_set1_epi8(rprevb);

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			tuned_ConvertPlanarBGRToPackedElement<F, false>(pp, gvalue.v0, bvalue.v0, rvalue.v0);

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);

		for (; pp < p + cbWidth; pp += 3)
		{
			pp[T::G] = gprevb += g[0];
			pp[T::B] = (bprevb += b[0]) + gprevb;
			pp[T::R] = (rprevb += r[0]) + gprevb;

			g += 1;
			b += 1;
			r += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_set1_epi8(0);
		__m128i bprev = _mm_set1_epi8(0);
		__m128i rprev = _mm_set1_epi8(0);

		for (; pp <= p + cbWidth - 48; pp += 48)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			auto packed = tuned_ConvertPlanarBGRToPackedElement<F, false>(gvalue.v0, bvalue.v0, rvalue.v0);
			_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(packed.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(packed.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(packed.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);

		for (; pp < p + cbWidth; pp += 3)
		{
			pp[T::G] = (gprevb += g[0]) + (pp - scbStride)[T::G];
			pp[T::B] = (bprevb += b[0]) + (pp - scbStride)[T::B] + gprevb;
			pp[T::R] = (rprevb += r[0]) + (pp - scbStride)[T::R] + gprevb;

			g += 1;
			b += 1;
			r += 1;
		}
	}

	gprevb = 0;
	bprevb = 0;
	rprevb = 0;

	uint8_t gtopprevb = 0;
	uint8_t btopprevb = 0;
	uint8_t rtopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i left;
		__m128i prevtmp;
		__m128i topprevtmp;
		__m128i ctl;

		ctl = _mm_set_epi8(-1, -1, -1, -1, 10, -1, 10, 7, -1, 7, 4, -1, 4, 1, -1, 1);

		left = _mm_cvtsi32_si128((gprevb << 8) | (bprevb) | (rprevb << 16));
		prevtmp = _mm_slli_si128(left, 13);
		topprevtmp = _mm_slli_si128(_mm_cvtsi32_si128((gtopprevb << 8) | (btopprevb) | (rtopprevb << 16)), 13);

		for (; pp <= p + cbWidth - 16; pp += 12)
		{
			__m128i gg = _mm_cvtsi32_si128(*(const uint32_t *)g);
			__m128i bb = _mm_cvtsi32_si128(*(const uint32_t *)b);
			__m128i rr = _mm_cvtsi32_si128(*(const uint32_t *)r);

			__m128i residual;
			__m128i top;

			residual = _mm_shuffle_epi8(_mm_unpacklo_epi64(_mm_unpacklo_epi32(bb, gg), rr), _mm_set_epi8(-1, -1, -1, -1, 11, 7, 3, 10, 6, 2, 9, 5, 1, 8, 4, 0));
			__m128i toptmp = _mm_loadu_si128((const __m128i *)(pp - scbStride));
			top = _mm_sub_epi8(_mm_add_epi8(toptmp, _mm_set_epi8(0, 0, 0, 0, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80)), _mm_shuffle_epi8(toptmp, ctl));

			__m128i value;
			__m128i grad;
			__m128i pred;
			__m128i topleft;

			topleft = _mm_alignr_epi8(top, topprevtmp, 13);
			__m128i top_minus_topleft = _mm_sub_epi8(top, topleft);

			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prevtmp, 13);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prevtmp, 13);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prevtmp, 13);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(_mm_sub_epi8(value, _mm_set_epi8(0, 0, 0, 0, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80, (char)0x80, 0, (char)0x80)), _mm_shuffle_epi8(value, ctl)));

			prevtmp = _mm_alignr_epi8(value, value, 12);
			left = _mm_alignr_epi8(value, value, 9);
			topprevtmp = _mm_alignr_epi8(top, top, 12);

			b += 4;
			g += 4;
			r += 4;
		}

		uint32_t tmp = _mm_cvtsi128_si32(left);
		gprevb = tmp >> 8;
		bprevb = tmp;
		rprevb = tmp >> 16;

		uint32_t toptmp = _mm_cvtsi128_si32(_mm_srli_si128(topprevtmp, 13));
		gtopprevb = toptmp >> 8;
		btopprevb = toptmp;
		rtopprevb = toptmp >> 16;

		for (; pp < p + cbWidth; pp += 3)
		{
			uint8_t gtop = (pp - scbStride)[T::G];
			uint8_t gg = *g + median<uint8_t>(gprevb, gtop, gprevb + gtop - gtopprevb);
			pp[T::G] = gg;
			uint8_t btop = (pp - scbStride)[T::B] - gtop + 0x80;
			uint8_t bb = *b + median<uint8_t>(bprevb, btop, bprevb + btop - btopprevb);
			pp[T::B] = bb + gg - 0x80;
			uint8_t rtop = (pp - scbStride)[T::R] - gtop + 0x80;
			uint8_t rr = *r + median<uint8_t>(rprevb, rtop, rprevb + rtop - rtopprevb);
			pp[T::R] = rr + gg - 0x80;

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			gtopprevb = gtop;
			btopprevb = btop;
			rtopprevb = rtop;

			b += 1;
			g += 1;
			r += 1;
		}
	}
}

template<int F, class T, bool A, PREDICTION_TYPE Pred>
static inline void tuned_ConvertULRXToRGBX_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t gprevb = 0x80;
	uint8_t bprevb = 0;
	uint8_t rprevb = 0;
	uint8_t aprevb = 0x80;

	auto g = pGBegin;
	auto b = pBBegin;
	auto r = pRBegin;
	auto a = pABegin;

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_set1_epi8(gprevb);
		__m128i bprev = _mm_set1_epi8(bprevb);
		__m128i rprev = _mm_set1_epi8(rprevb);
		__m128i aprev;
		if (A)
			aprev = _mm_set1_epi8(aprevb);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			__m128i avalue_v0;
			if (A)
			{
				__m128i aa = _mm_loadu_si128((const __m128i *)a);
				auto avalue = tuned_RestoreLeft8Element<F>(aprev, aa);
				aprev = avalue.v1;
				avalue_v0 = avalue.v0;
			}
			else
			{
				avalue_v0 = _mm_set1_epi8((char)0xff);
			}
			tuned_ConvertPlanarRGBXToPackedElement<F, T, false>(pp, gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);


			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);
		if (A)
			aprevb = _mm_cvtsi128_si32(aprev);

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::G] = gprevb += g[0];
			pp[T::B] = (bprevb += b[0]) + gprevb;
			pp[T::R] = (rprevb += r[0]) + gprevb;
			if (A)
				pp[T::A] = aprevb += a[0];
			else
				pp[T::A] = 0xff;

			g += 1;
			b += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_set1_epi8(0);
		__m128i bprev = _mm_set1_epi8(0);
		__m128i rprev = _mm_set1_epi8(0);
		__m128i aprev;
		if (A)
			aprev = _mm_set1_epi8(0);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i gg = _mm_loadu_si128((const __m128i *)g);
			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gg);
			__m128i bb = _mm_loadu_si128((const __m128i *)b);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bb);
			__m128i rr = _mm_loadu_si128((const __m128i *)r);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rr);
			__m128i avalue_v0;
			if (A)
			{
				__m128i aa = _mm_loadu_si128((const __m128i *)a);
				auto avalue = tuned_RestoreLeft8Element<F>(aprev, aa);
				aprev = avalue.v1;
				avalue_v0 = avalue.v0;
			}
			else
			{
				avalue_v0 = _mm_set1_epi8(0);
			}
			auto packed = tuned_ConvertPlanarRGBXToPackedElement<F, T, false>(gvalue.v0, bvalue.v0, rvalue.v0, avalue_v0);
			_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(packed.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(packed.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(packed.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));
			_mm_storeu_si128((__m128i *)(pp + 48), _mm_add_epi8(packed.v3, _mm_loadu_si128((__m128i *)(pp - scbStride + 48))));

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;

			g += 16;
			b += 16;
			r += 16;
			if (A)
				a += 16;
		}

		gprevb = _mm_cvtsi128_si32(gprev);
		bprevb = _mm_cvtsi128_si32(bprev);
		rprevb = _mm_cvtsi128_si32(rprev);
		if (A)
			aprevb = _mm_cvtsi128_si32(aprev);

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::G] = (gprevb += g[0]) + (pp - scbStride)[T::G];
			pp[T::B] = (bprevb += b[0]) + (pp - scbStride)[T::B] + gprevb;
			pp[T::R] = (rprevb += r[0]) + (pp - scbStride)[T::R] + gprevb;
			if (A)
				pp[T::A] = (aprevb += a[0]) + (pp - scbStride)[T::A];
			else
				pp[T::A] = 0xff;

			g += 1;
			b += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}

	gprevb = 0;
	bprevb = 0;
	rprevb = 0;
	if (A)
		aprevb = 0;

	uint8_t gtopprevb = 0;
	uint8_t btopprevb = 0;
	uint8_t rtopprevb = 0;
	uint8_t atopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i left;
		__m128i prev;
		__m128i topprev;
		__m128i ctl;

		if (std::is_same<T, CBGRAColorOrder>::value)
			ctl = _mm_set_epi8(-1, 13, -1, 13, -1, 9, -1, 9, -1, 5, -1, 5, -1, 1, -1, 1);
		else
			ctl = _mm_set_epi8(14, -1, 14, -1, 10, -1, 10, -1, 6, -1, 6, -1, 2, -1, 2, -1);

		if (std::is_same<T, CBGRAColorOrder>::value)
		{
			left = _mm_cvtsi32_si128((gprevb << 8) | (bprevb) | (rprevb << 16) | (A ? aprevb << 24 : 0));
			prev = _mm_slli_si128(left, 12);
			topprev = _mm_slli_si128(_mm_cvtsi32_si128((gtopprevb << 8) | (btopprevb) | (rtopprevb << 16) | (A ? atopprevb << 24 : 0)), 12);
		}
		else
		{
			left = _mm_cvtsi32_si128((gprevb << 16) | (bprevb << 24) | (rprevb << 8) | (A ? aprevb : 0));
			prev = _mm_slli_si128(left, 12);
			topprev = _mm_slli_si128(_mm_cvtsi32_si128((gtopprevb << 16) | (btopprevb << 24) | (rtopprevb << 8) | (A ? atopprevb : 0)), 12);
		}

		for (; pp <= p + cbWidth - 16; pp += 16)
		{
			__m128i gg = _mm_cvtsi32_si128(*(const uint32_t *)g);
			__m128i bb = _mm_cvtsi32_si128(*(const uint32_t *)b);
			__m128i rr = _mm_cvtsi32_si128(*(const uint32_t *)r);
			__m128i aa = A ? _mm_cvtsi32_si128(*(const uint32_t *)a) : _mm_set1_epi8(0);

			__m128i residual;
			__m128i top;
			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				residual = _mm_unpacklo_epi16(_mm_unpacklo_epi8(bb, gg), _mm_unpacklo_epi8(rr, aa));
				__m128i toptmp = _mm_loadu_si128((const __m128i *)(pp - scbStride));
				top = _mm_sub_epi8(_mm_add_epi8(toptmp, _mm_set1_epi32(0x00800080)), _mm_shuffle_epi8(toptmp, ctl));
			}
			else
			{
				residual = _mm_unpacklo_epi16(_mm_unpacklo_epi8(aa, rr), _mm_unpacklo_epi8(gg, bb));
				__m128i toptmp = _mm_loadu_si128((const __m128i *)(pp - scbStride));
				top = _mm_sub_epi8(_mm_add_epi8(toptmp, _mm_set1_epi32(0x80008000)), _mm_shuffle_epi8(toptmp, ctl));
			}

			__m128i value;
			__m128i grad;
			__m128i pred;
			__m128i topleft;

			topleft = _mm_alignr_epi8(top, topprev, 12);
			__m128i top_minus_topleft = _mm_sub_epi8(top, topleft);

			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 12);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 12);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_alignr_epi8(value, prev, 12);
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			if (std::is_same<T, CBGRAColorOrder>::value)
			{
				_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(_mm_sub_epi8(value, _mm_set1_epi32(0x00800080)), _mm_shuffle_epi8(value, ctl)));
			}
			else
			{
				_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(_mm_sub_epi8(value, _mm_set1_epi32(0x80008000)), _mm_shuffle_epi8(value, ctl)));
			}

			prev = value;
			left = _mm_alignr_epi8(value, value, 12);
			topprev = top;

			b += 4;
			g += 4;
			r += 4;
			if (A)
				a += 4;
		}

		if (std::is_same<T, CBGRAColorOrder>::value)
		{
			uint32_t tmp = _mm_cvtsi128_si32(left);
			gprevb = tmp >> 8;
			bprevb = tmp;
			rprevb = tmp >> 16;
			if (A)
				aprevb = tmp >> 24;

			uint32_t toptmp = _mm_cvtsi128_si32(_mm_srli_si128(topprev, 12));
			gtopprevb = toptmp >> 8;
			btopprevb = toptmp;
			rtopprevb = toptmp >> 16;
			if (A)
				atopprevb = toptmp >> 24;
		}
		else
		{
			uint32_t tmp = _mm_cvtsi128_si32(left);
			gprevb = tmp >> 16;
			bprevb = tmp >> 24;
			rprevb = tmp >> 8;
			if (A)
				aprevb = tmp;

			uint32_t toptmp = _mm_cvtsi128_si32(_mm_srli_si128(topprev, 12));
			gtopprevb = toptmp >> 16;
			btopprevb = toptmp >> 24;
			rtopprevb = toptmp >> 8;
			if (A)
				atopprevb = toptmp;
		}

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t gtop = (pp - scbStride)[T::G];
			uint8_t gg = *g + median<uint8_t>(gprevb, gtop, gprevb + gtop - gtopprevb);
			pp[T::G] = gg;
			uint8_t btop = (pp - scbStride)[T::B] - gtop + 0x80;
			uint8_t bb = *b + median<uint8_t>(bprevb, btop, bprevb + btop - btopprevb);
			pp[T::B] = bb + gg - 0x80;
			uint8_t rtop = (pp - scbStride)[T::R] - gtop + 0x80;
			uint8_t rr = *r + median<uint8_t>(rprevb, rtop, rprevb + rtop - rtopprevb);
			pp[T::R] = rr + gg - 0x80;
			if (A)
			{
				uint8_t atop = (pp - scbStride)[T::A];
				uint8_t aa = *a + median<uint8_t>(aprevb, atop, aprevb + atop - atopprevb);
				pp[T::A] = aa;
				aprevb = aa;
				atopprevb = atop;
			}
			else
				pp[T::A] = 0xff;

			gprevb = gg;
			bprevb = bb;
			rprevb = rr;
			gtopprevb = gtop;
			btopprevb = btop;
			rtopprevb = rtop;

			b += 1;
			g += 1;
			r += 1;
			if (A)
				a += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertULRGToBGR_Restore<F, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
	else
		tuned_ConvertULRXToRGBX_Restore<F, T, false, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertULRGToBGR_Restore<F, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
	else
		tuned_ConvertULRXToRGBX_Restore<F, T, false, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride)
{
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertULRGToBGR_Restore<F, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride);
	else
		tuned_ConvertULRXToRGBX_Restore<F, T, false, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Restore<F, T, true, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalLeft<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalLeft<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestorePlanarGradient<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestorePlanarGradient<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGBegin, const uint8_t *pBBegin, const uint8_t *pRBegin, const uint8_t *pABegin, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, class T, PREDICTION_TYPE Pred>
static inline void tuned_ConvertPackedYUV422ToULY2_PredictAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	uint8_t yprevb = 0x80;
	uint8_t uprevb = 0x80;
	uint8_t vprevb = 0x80;

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pSrcBegin; p != (Pred != CYLINDRICAL_LEFT ? pSrcBegin + scbStride : pSrcEnd); p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_slli_si128(_mm_cvtsi32_si128(yprevb), 15);
		__m128i uprev = _mm_slli_si128(_mm_cvtsi32_si128(uprevb), 15);
		__m128i vprev = _mm_slli_si128(_mm_cvtsi32_si128(vprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(pp);
			_mm_storeu_si128((__m128i *)y, tuned_PredictLeftAndCount8Element<F>(yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictLeftAndCount8Element<F>(planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictLeftAndCount8Element<F>(uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictLeftAndCount8Element<F>(vprev, planar.v, pVCountTable));

			yprev = planar.y1;
			uprev = planar.u;
			vprev = planar.v;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(_mm_srli_si128(yprev, 15));
		uprevb = _mm_cvtsi128_si32(_mm_srli_si128(uprev, 15));
		vprevb = _mm_cvtsi128_si32(_mm_srli_si128(vprev, 15));

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0];
			*y = yy0 - yprevb;
			++pYCountTable[*y];
			uint8_t yy1 = pp[T::Y1];
			*(y + 1) = yy1 - yy0;
			++pYCountTable[*(y + 1)];
			uint8_t uu = pp[T::U];
			*u = uu - uprevb;
			++pUCountTable[*u];
			uint8_t vv = pp[T::V];
			*v = vv - vprevb;
			++pVCountTable[*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;

			y += 2;
			u += 1;
			v += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_setzero_si128();
		__m128i uprev = _mm_setzero_si128();
		__m128i vprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48)))
			);
			_mm_storeu_si128((__m128i *)y, tuned_PredictLeftAndCount8Element<F>(yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictLeftAndCount8Element<F>(planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictLeftAndCount8Element<F>(uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictLeftAndCount8Element<F>(vprev, planar.v, pVCountTable));

			yprev = planar.y1;
			uprev = planar.u;
			vprev = planar.v;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(_mm_srli_si128(yprev, 15));
		uprevb = _mm_cvtsi128_si32(_mm_srli_si128(uprev, 15));
		vprevb = _mm_cvtsi128_si32(_mm_srli_si128(vprev, 15));

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0] - (pp - scbStride)[T::Y0];
			*y = yy0 - yprevb;
			++pYCountTable[*y];
			uint8_t yy1 = pp[T::Y1] - (pp - scbStride)[T::Y1];
			*(y + 1) = yy1 - yy0;
			++pYCountTable[*(y + 1)];
			uint8_t uu = pp[T::U] - (pp - scbStride)[T::U];
			*u = uu - uprevb;
			++pUCountTable[*u];
			uint8_t vv = pp[T::V] - (pp - scbStride)[T::V];
			*v = vv - vprevb;
			++pVCountTable[*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;

			y += 2;
			u += 1;
			v += 1;
		}
	}

	yprevb = 0;
	uprevb = 0;
	vprevb = 0;

	uint8_t ytopprevb = 0;
	uint8_t utopprevb = 0;
	uint8_t vtopprevb = 0;

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_slli_si128(_mm_cvtsi32_si128(yprevb), 15);
		__m128i uprev = _mm_slli_si128(_mm_cvtsi32_si128(uprevb), 15);
		__m128i vprev = _mm_slli_si128(_mm_cvtsi32_si128(vprevb), 15);
		__m128i ytopprev = _mm_slli_si128(_mm_cvtsi32_si128(ytopprevb), 15);
		__m128i utopprev = _mm_slli_si128(_mm_cvtsi32_si128(utopprevb), 15);
		__m128i vtopprev = _mm_slli_si128(_mm_cvtsi32_si128(vtopprevb), 15);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			auto planar = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(pp);
			auto top = tuned_ConvertPackedYUV422ToPlanarElement<F, T>(pp - scbStride);
			_mm_storeu_si128((__m128i *)y, tuned_PredictWrongMedianAndCount8Element<F>(ytopprev, top.y0, yprev, planar.y0, pYCountTable));
			_mm_storeu_si128((__m128i *)(y + 16), tuned_PredictWrongMedianAndCount8Element<F>(top.y0, top.y1, planar.y0, planar.y1, pYCountTable));
			_mm_storeu_si128((__m128i *)u, tuned_PredictWrongMedianAndCount8Element<F>(utopprev, top.u, uprev, planar.u, pUCountTable));
			_mm_storeu_si128((__m128i *)v, tuned_PredictWrongMedianAndCount8Element<F>(vtopprev, top.v, vprev, planar.v, pVCountTable));

			yprev = planar.y1;
			uprev = planar.u;
			vprev = planar.v;
			ytopprev = top.y1;
			utopprev = top.u;
			vtopprev = top.v;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(_mm_srli_si128(yprev, 15));
		uprevb = _mm_cvtsi128_si32(_mm_srli_si128(uprev, 15));
		vprevb = _mm_cvtsi128_si32(_mm_srli_si128(vprev, 15));
		ytopprevb = _mm_cvtsi128_si32(_mm_srli_si128(ytopprev, 15));
		utopprevb = _mm_cvtsi128_si32(_mm_srli_si128(utopprev, 15));
		vtopprevb = _mm_cvtsi128_si32(_mm_srli_si128(vtopprev, 15));

		for (; pp < p + cbWidth; pp += 4)
		{
			uint8_t yy0 = pp[T::Y0];
			uint8_t ytop0 = (pp - scbStride)[T::Y0];
			*y = yy0 - median<uint8_t>(yprevb, ytop0, yprevb + ytop0 - ytopprevb);
			++pYCountTable[*y];
			uint8_t yy1 = pp[T::Y1];
			uint8_t ytop1 = (pp - scbStride)[T::Y1];
			*(y + 1) = yy1 - median<uint8_t>(yy0, ytop1, yy0 + ytop1 - ytop0);
			++pYCountTable[*(y + 1)];
			uint8_t uu = pp[T::U];
			uint8_t utop = (pp - scbStride)[T::U];
			*u = uu - median<uint8_t>(uprevb, utop, uprevb + utop - utopprevb);
			++pUCountTable[*u];
			uint8_t vv = pp[T::V];
			uint8_t vtop = (pp - scbStride)[T::V];
			*v = vv - median<uint8_t>(vprevb, vtop, vprevb + vtop - vtopprevb);
			++pVCountTable[*v];

			yprevb = yy1;
			uprevb = uu;
			vprevb = vv;
			ytopprevb = ytop1;
			utopprevb = utop;
			vtopprevb = vtop;

			y += 2;
			u += 1;
			v += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, CYLINDRICAL_LEFT>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, PLANAR_GRADIENT>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

template<int F, class T>
void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable)
{
	tuned_ConvertPackedYUV422ToULY2_PredictAndCount<F, T, CYLINDRICAL_WRONG_MEDIAN>(pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride, pYCountTable, pUCountTable, pVCountTable);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalLeftAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictPlanarGradientAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
template void tuned_ConvertPackedYUV422ToULY2_PredictCylindricalWrongMedianAndCount<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride, uint32_t *pYCountTable, uint32_t *pUCountTable, uint32_t *pVCountTable);
#endif

//

template<int F, class T, PREDICTION_TYPE Pred>
static inline void tuned_ConvertULY2ToPackedYUV422_Restore(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	uint8_t yprevb = 0x80;
	uint8_t uprevb = 0x80;
	uint8_t vprevb = 0x80;

	auto y = pYBegin;
	auto u = pUBegin;
	auto v = pVBegin;

	for (auto p = pDstBegin; p != (Pred != CYLINDRICAL_LEFT ? pDstBegin + scbStride : pDstEnd); p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_set1_epi8(yprevb);
		__m128i uprev = _mm_set1_epi8(uprevb);
		__m128i vprev = _mm_set1_epi8(vprevb);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			auto yvalue = tuned_RestoreLeft8Element<F>(yprev, yy0, yy1);
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uu);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vv);
			tuned_ConvertPlanarYUV422ToPackedElement<F, T>(pp, yvalue.v0, yvalue.v1, uvalue.v0, vvalue.v0);

			yprev = yvalue.v2;
			uprev = uvalue.v1;
			vprev = vvalue.v1;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(yprev);
		uprevb = _mm_cvtsi128_si32(uprev);
		vprevb = _mm_cvtsi128_si32(vprev);

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::Y0] = yprevb += y[0];
			pp[T::Y1] = yprevb += y[1];
			pp[T::U] = uprevb += u[0];
			pp[T::V] = vprevb += v[0];

			y += 2;
			u += 1;
			v += 1;
		}
	}

	if (Pred == PLANAR_GRADIENT) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i yprev = _mm_set1_epi8(0);
		__m128i uprev = _mm_set1_epi8(0);
		__m128i vprev = _mm_set1_epi8(0);

		for (; pp <= p + cbWidth - 64; pp += 64)
		{
			__m128i yy0 = _mm_loadu_si128((const __m128i *)y);
			__m128i yy1 = _mm_loadu_si128((const __m128i *)(y + 16));
			auto yvalue = tuned_RestoreLeft8Element<F>(yprev, yy0, yy1);
			__m128i uu = _mm_loadu_si128((const __m128i *)u);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uu);
			__m128i vv = _mm_loadu_si128((const __m128i *)v);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vv);
			auto packed = tuned_ConvertPlanarYUV422ToPackedElement<F, T>(yvalue.v0, yvalue.v1, uvalue.v0, vvalue.v0);
			_mm_storeu_si128((__m128i *)pp, _mm_add_epi8(packed.v0, _mm_loadu_si128((__m128i *)(pp - scbStride))));
			_mm_storeu_si128((__m128i *)(pp + 16), _mm_add_epi8(packed.v1, _mm_loadu_si128((__m128i *)(pp - scbStride + 16))));
			_mm_storeu_si128((__m128i *)(pp + 32), _mm_add_epi8(packed.v2, _mm_loadu_si128((__m128i *)(pp - scbStride + 32))));
			_mm_storeu_si128((__m128i *)(pp + 48), _mm_add_epi8(packed.v3, _mm_loadu_si128((__m128i *)(pp - scbStride + 48))));

			yprev = yvalue.v2;
			uprev = uvalue.v1;
			vprev = vvalue.v1;

			y += 32;
			u += 16;
			v += 16;
		}

		yprevb = _mm_cvtsi128_si32(yprev);
		uprevb = _mm_cvtsi128_si32(uprev);
		vprevb = _mm_cvtsi128_si32(vprev);

		for (; pp < p + cbWidth; pp += 4)
		{
			pp[T::Y0] = (yprevb += y[0]) + (pp - scbStride)[T::Y0];
			pp[T::Y1] = (yprevb += y[1]) + (pp - scbStride)[T::Y1];
			pp[T::U] = (uprevb += u[0]) + (pp - scbStride)[T::U];
			pp[T::V] = (vprevb += v[0]) + (pp - scbStride)[T::V];

			y += 2;
			u += 1;
			v += 1;
		}
	}

	__m128i prev = _mm_set1_epi8(0);
	__m128i topprev = _mm_set1_epi8(0);

	if (Pred == CYLINDRICAL_WRONG_MEDIAN) for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m128i ctl_fromprev;
		__m128i ctl_fromcur;
		if (std::is_same<T, CYUYVColorOrder>::value)
		{
			ctl_fromprev = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, 1, 2);
			ctl_fromcur = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1);
		}
		else
		{
			ctl_fromprev = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 3, 0);
			ctl_fromcur = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1);
		}

		for (; pp < p + cbWidth; pp += 4)
		{
			__m128i residual;
			if (std::is_same<T, CYUYVColorOrder>::value)
				residual = _mm_cvtsi32_si128(y[0] | (u[0] << 8) | (y[1] << 16) | (v[0] << 24));
			else
				residual = _mm_cvtsi32_si128(u[0] | (y[0] << 8) | (v[0] << 16) | (y[1] << 24));
			__m128i top = _mm_cvtsi32_si128(*(const uint32_t *)(pp - scbStride));

			__m128i left = _mm_shuffle_epi8(prev, ctl_fromprev);
			__m128i topleft = _mm_or_si128(
				_mm_shuffle_epi8(topprev, ctl_fromprev),
				_mm_shuffle_epi8(top, ctl_fromcur));

			__m128i top_minus_topleft = _mm_sub_epi8(top, topleft);
			__m128i grad;
			__m128i pred;
			__m128i value;

			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			left = _mm_or_si128(left, _mm_shuffle_epi8(value, ctl_fromcur));
			grad = _mm_add_epi8(left, top_minus_topleft);
			pred = _mm_max_epu8(_mm_min_epu8(_mm_max_epu8(left, top), grad), _mm_min_epu8(left, top));
			value = _mm_add_epi8(residual, pred);

			*(uint32_t *)pp = _mm_cvtsi128_si32(value);

			prev = value;
			topprev = top;

			y += 2;
			u += 1;
			v += 1;
		}
	}
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, CYLINDRICAL_LEFT>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, PLANAR_GRADIENT>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULY2ToPackedYUV422_Restore<F, T, CYLINDRICAL_WRONG_MEDIAN>(pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_SSE41, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalLeft<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestorePlanarGradient<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_RestoreCylindricalWrongMedian<CODEFEATURE_AVX1, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, size_t cbWidth, ssize_t scbStride);
#endif