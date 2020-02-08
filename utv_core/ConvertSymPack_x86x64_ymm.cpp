/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

#if !defined(GENERATE_AVX2)
#error
#endif

template<int F, class T, bool A>
static inline void tuned_ConvertRGBXToULRX_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	auto gp = pGPacked;
	auto bp = pBPacked;
	auto rp = pRPacked;
	auto ap = pAPacked;

	auto gc = pGControl;
	auto bc = pBControl;
	auto rc = pRControl;
	auto ac = pAControl;

	size_t cbControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + T::BYPP * 64 - 1) / (T::BYPP * 64)) * 3;
	memset(pGControl, 0, cbControl);
	memset(pBControl, 0, cbControl);
	memset(pRControl, 0, cbControl);
	if (A)
		memset(pAControl, 0, cbControl);

	union padsolve
	{
		uint8_t b[64];
		__m256i ymm[2];
	};

	{
		const auto p = pSrcBegin;
		auto pp = p;

		__m256i gprev = _mm256_set1_epi8((char)0x80);
		__m256i bprev = _mm256_setzero_si256();
		__m256i rprev = _mm256_setzero_si256();
		__m256i aprev;
		if (A)
			aprev = _mm256_set1_epi8((char)0x80);

		for (; pp <= p + cbWidth - T::BYPP * 64; pp += T::BYPP * 64)
		{
			auto planar0 = tuned_ConvertPackedRGBXToPlanarElement<F, __m256i, T, false>(pp);
			auto planar1 = tuned_ConvertPackedRGBXToPlanarElement<F, __m256i, T, false>(pp + T::BYPP * 32);

			__m256i gresidual0 = tuned_PredictLeft8Element<F>(gprev, planar0.g);
			__m256i gresidual1 = tuned_PredictLeft8Element<F>(planar0.g, planar1.g);
			__m256i bresidual0 = tuned_PredictLeft8Element<F>(bprev, planar0.b);
			__m256i bresidual1 = tuned_PredictLeft8Element<F>(planar0.b, planar1.b);
			__m256i rresidual0 = tuned_PredictLeft8Element<F>(rprev, planar0.r);
			__m256i rresidual1 = tuned_PredictLeft8Element<F>(planar0.r, planar1.r);

			PackForIntra<F>(gp, gc, gresidual0, gresidual1);
			PackForIntra<F>(bp, bc, bresidual0, bresidual1);
			PackForIntra<F>(rp, rc, rresidual0, rresidual1);

			gprev = planar1.g;
			bprev = planar1.b;
			rprev = planar1.r;

			if (A)
			{
				__m256i aresidual0 = tuned_PredictLeft8Element<F>(aprev, planar0.a);
				__m256i aresidual1 = tuned_PredictLeft8Element<F>(planar0.a, planar1.a);
				PackForIntra<F>(ap, ac, aresidual0, aresidual1);
				aprev = planar1.a;
			}
		}

		if (pp < p + cbWidth)
		{
			uint8_t gprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(gprev, 1), 15));
			uint8_t bprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(bprev, 1), 15));
			uint8_t rprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(rprev, 1), 15));
			uint8_t aprevb;
			if (A)
				aprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(aprev, 1), 15));

			int n = 0;
			padsolve gps, bps, rps, aps;

			for (; pp < p + cbWidth; pp += T::BYPP, ++n)
			{
				uint8_t gg = pp[T::G];
				gps.b[n] = gg - gprevb;
				uint8_t bb = pp[T::B] - gg;
				bps.b[n] = bb - bprevb;
				uint8_t rr = pp[T::R] - gg;
				rps.b[n] = rr - rprevb;

				gprevb = gg;
				bprevb = bb;
				rprevb = rr;

				if (A)
				{
					uint8_t aa = pp[T::A];
					aps.b[n] = aa - aprevb;
					aprevb = aa;
				}
			}
			for (; n < 64; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
				if (A)
					aps.b[n] = 0;
			}

			PackForIntra<F>(gp, gc, gps.ymm[0], gps.ymm[1]);
			PackForIntra<F>(bp, bc, bps.ymm[0], bps.ymm[1]);
			PackForIntra<F>(rp, rc, rps.ymm[0], rps.ymm[1]);
			if (A)
				PackForIntra<F>(ap, ac, aps.ymm[0], aps.ymm[1]);
		}
	}

	for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m256i gprev = _mm256_setzero_si256();
		__m256i bprev = _mm256_setzero_si256();
		__m256i rprev = _mm256_setzero_si256();
		__m256i aprev;
		if (A)
			aprev = _mm256_setzero_si256();

		for (; pp <= p + cbWidth - T::BYPP * 64; pp += T::BYPP * 64)
		{
			auto planar0 = tuned_ConvertPackedRGBXToPlanarElement<F, __m256i, T, false>(pp, scbStride);
			auto planar1 = tuned_ConvertPackedRGBXToPlanarElement<F, __m256i, T, false>(pp + T::BYPP * 32, scbStride);

			__m256i gresidual0 = tuned_PredictLeft8Element<F>(gprev, planar0.g);
			__m256i gresidual1 = tuned_PredictLeft8Element<F>(planar0.g, planar1.g);
			__m256i bresidual0 = tuned_PredictLeft8Element<F>(bprev, planar0.b);
			__m256i bresidual1 = tuned_PredictLeft8Element<F>(planar0.b, planar1.b);
			__m256i rresidual0 = tuned_PredictLeft8Element<F>(rprev, planar0.r);
			__m256i rresidual1 = tuned_PredictLeft8Element<F>(planar0.r, planar1.r);

			PackForIntra<F>(gp, gc, gresidual0, gresidual1);
			PackForIntra<F>(bp, bc, bresidual0, bresidual1);
			PackForIntra<F>(rp, rc, rresidual0, rresidual1);

			gprev = planar1.g;
			bprev = planar1.b;
			rprev = planar1.r;

			if (A)
			{
				__m256i aresidual0 = tuned_PredictLeft8Element<F>(aprev, planar0.a);
				__m256i aresidual1 = tuned_PredictLeft8Element<F>(planar0.a, planar1.a);
				PackForIntra<F>(ap, ac, aresidual0, aresidual1);
				aprev = planar1.a;
			}
		}

		if (pp < p + cbWidth)
		{
			uint8_t gprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(gprev, 1), 15));
			uint8_t bprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(bprev, 1), 15));
			uint8_t rprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(rprev, 1), 15));
			uint8_t aprevb;
			if (A)
				aprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(aprev, 1), 15));

			int n = 0;
			padsolve gps, bps, rps, aps;

			for (; pp < p + cbWidth; pp += T::BYPP, ++n)
			{
				uint8_t gg = pp[T::G] - (pp - scbStride)[T::G];
				gps.b[n] = gg - gprevb;
				uint8_t bb = pp[T::B] - (pp - scbStride)[T::B] - gg;
				bps.b[n] = bb - bprevb;
				uint8_t rr = pp[T::R] - (pp - scbStride)[T::R] - gg;
				rps.b[n] = rr - rprevb;

				gprevb = gg;
				bprevb = bb;
				rprevb = rr;

				if (A)
				{
					uint8_t aa = pp[T::A] - (pp - scbStride)[T::A];
					aps.b[n] = aa - aprevb;
					aprevb = aa;
				}
			}
			for (; n < 64; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
				if (A)
					aps.b[n] = 0;
			}

			PackForIntra<F>(gp, gc, gps.ymm[0], gps.ymm[1]);
			PackForIntra<F>(bp, bc, bps.ymm[0], bps.ymm[1]);
			PackForIntra<F>(rp, rc, rps.ymm[0], rps.ymm[1]);
			if (A)
				PackForIntra<F>(ap, ac, aps.ymm[0], aps.ymm[1]);
		}
	}

	*cbGPacked = gp - pGPacked;
	*cbBPacked = bp - pBPacked;
	*cbRPacked = rp - pRPacked;
	if (A)
		*cbAPacked = ap - pAPacked;
}

template<int F, class T>
void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertRGBXToULRX_Pack8SymAfterPredictPlanarGradient8<F, T, false>(pGPacked, cbGPacked, pGControl, pBPacked, cbBPacked, pBControl, pRPacked, cbRPacked, pRControl, NULL, NULL, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertRGBXToULRX_Pack8SymAfterPredictPlanarGradient8<F, T, true>(pGPacked, cbGPacked, pGControl, pBPacked, cbBPacked, pBControl, pRPacked, cbRPacked, pRControl, pAPacked, cbAPacked, pAControl, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

#ifdef GENERATE_AVX2
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CBGRColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, typename T, bool A>
static inline void tuned_ConvertULRXToRGBX_Unpack8SymAndRestorePlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, const uint8_t *pAPacked, const uint8_t *pAControl, size_t cbWidth, ssize_t scbStride)
{
	auto gp = pGPacked;
	auto bp = pBPacked;
	auto rp = pRPacked;
	auto ap = pAPacked;

	auto gc = pGControl;
	auto bc = pBControl;
	auto rc = pRControl;
	auto ac = pAControl;

	union padsolve
	{
		uint8_t b[32];
		__m256i ymm;
	};

	{
		const auto p = pDstBegin;
		auto pp = p;

		__m256i gprev = _mm256_set1_epi8((char)0x80);
		__m256i bprev = _mm256_setzero_si256();
		__m256i rprev = _mm256_setzero_si256();
		__m256i aprev = _mm256_set1_epi8((char)0x80);

		int gshift = 0;
		int bshift = 0;
		int rshift = 0;
		int ashift = 0;

		for (; pp <= p + cbWidth - T::BYPP * 32; pp += T::BYPP * 32)
		{
			__m256i gresidual = UnpackForIntra<F>(gp, gc, gshift);
			__m256i bresidual = UnpackForIntra<F>(bp, bc, bshift);
			__m256i rresidual = UnpackForIntra<F>(rp, rc, rshift);
			__m256i aresidual;
			if (A)
				aresidual = UnpackForIntra<F>(ap, ac, ashift);

			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gresidual);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bresidual);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rresidual);
			VECTOR2<__m256i> avalue;
			if (A)
				avalue = tuned_RestoreLeft8Element<F>(aprev, aresidual);

			tuned_ConvertPlanarRGBXToPackedElement<F, __m256i, T, false>(pp, gvalue.v0, bvalue.v0, rvalue.v0, A ? avalue.v0 : _mm256_set1_epi8((char)0xff));

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;
			if (A)
				aprev = avalue.v1;
		}

		if (pp < p + cbWidth)
		{
			__m256i gresidual = UnpackForIntra<F>(gp, gc, gshift);
			__m256i bresidual = UnpackForIntra<F>(bp, bc, bshift);
			__m256i rresidual = UnpackForIntra<F>(rp, rc, rshift);
			__m256i aresidual;
			if (A)
				aresidual = UnpackForIntra<F>(ap, ac, ashift);

			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gresidual);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bresidual);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rresidual);
			VECTOR2<__m256i> avalue;
			if (A)
				avalue = tuned_RestoreLeft8Element<F>(aprev, aresidual);

			int n = 0;
			padsolve gps, bps, rps, aps;

			gps.ymm = gvalue.v0;
			bps.ymm = _mm256_add_epi8(bvalue.v0, gvalue.v0);
			rps.ymm = _mm256_add_epi8(rvalue.v0, gvalue.v0);
			if (A)
				aps.ymm = avalue.v0;

			for (; pp < p + cbWidth; pp += T::BYPP, ++n)
			{
				pp[T::G] = gps.b[n];
				pp[T::B] = bps.b[n];
				pp[T::R] = rps.b[n];
				if (A)
					pp[T::A] = aps.b[n];
				else if (T::HAS_ALPHA)
					pp[T::A] = 0xff;
			}
		}

		if (gshift != 0)
		{
			gc += 3;
			bc += 3;
			rc += 3;
			if (A)
				ac += 3;
		}
	}

	for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m256i gprev = _mm256_setzero_si256();
		__m256i bprev = _mm256_setzero_si256();
		__m256i rprev = _mm256_setzero_si256();
		__m256i aprev = _mm256_setzero_si256();

		int gshift = 0;
		int bshift = 0;
		int rshift = 0;
		int ashift = 0;

		for (; pp <= p + cbWidth - T::BYPP * 32; pp += T::BYPP * 32)
		{
			__m256i gresidual = UnpackForIntra<F>(gp, gc, gshift);
			__m256i bresidual = UnpackForIntra<F>(bp, bc, bshift);
			__m256i rresidual = UnpackForIntra<F>(rp, rc, rshift);
			__m256i aresidual;
			if (A)
				aresidual = UnpackForIntra<F>(ap, ac, ashift);

			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gresidual);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bresidual);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rresidual);
			VECTOR2<__m256i> avalue;
			if (A)
				avalue = tuned_RestoreLeft8Element<F>(aprev, aresidual);

			tuned_ConvertPlanarRGBXToPackedElement<F, __m256i, T, false>(pp, gvalue.v0, bvalue.v0, rvalue.v0, A ? avalue.v0 : _mm256_set1_epi8(0), scbStride);

			gprev = gvalue.v1;
			bprev = bvalue.v1;
			rprev = rvalue.v1;
			if (A)
				aprev = avalue.v1;
		}

		if (pp < p + cbWidth)
		{
			__m256i gresidual = UnpackForIntra<F>(gp, gc, gshift);
			__m256i bresidual = UnpackForIntra<F>(bp, bc, bshift);
			__m256i rresidual = UnpackForIntra<F>(rp, rc, rshift);
			__m256i aresidual;
			if (A)
				aresidual = UnpackForIntra<F>(ap, ac, ashift);

			auto gvalue = tuned_RestoreLeft8Element<F>(gprev, gresidual);
			auto bvalue = tuned_RestoreLeft8Element<F>(bprev, bresidual);
			auto rvalue = tuned_RestoreLeft8Element<F>(rprev, rresidual);
			VECTOR2<__m256i> avalue;
			if (A)
				avalue = tuned_RestoreLeft8Element<F>(aprev, aresidual);

			int n = 0;
			padsolve gps, bps, rps, aps;

			gps.ymm = gvalue.v0;
			bps.ymm = _mm256_add_epi8(bvalue.v0, gvalue.v0);
			rps.ymm = _mm256_add_epi8(rvalue.v0, gvalue.v0);
			if (A)
				aps.ymm = avalue.v0;

			for (; pp < p + cbWidth; pp += T::BYPP, ++n)
			{
				pp[T::G] = gps.b[n] + (pp - scbStride)[T::G];
				pp[T::B] = bps.b[n] + (pp - scbStride)[T::B];
				pp[T::R] = rps.b[n] + (pp - scbStride)[T::R];
				if (A)
					pp[T::A] = aps.b[n] + (pp - scbStride)[T::A];
				else if (T::HAS_ALPHA)
					pp[T::A] = 0xff;
			}
		}

		if (gshift != 0)
		{
			gc += 3;
			bc += 3;
			rc += 3;
			if (A)
				ac += 3;
		}
	}
}

template<int F, class T>
void tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Unpack8SymAndRestorePlanarGradient8<F, T, false>(pDstBegin, pDstEnd, pGPacked, pGControl, pBPacked, pBControl, pRPacked, pRControl, NULL, NULL, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, const uint8_t *pAPacked, const uint8_t *pAControl, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertULRXToRGBX_Unpack8SymAndRestorePlanarGradient8<F, T, true>(pDstBegin, pDstEnd, pGPacked, pGControl, pBPacked, pBControl, pRPacked, pRControl, pAPacked, pAControl, cbWidth, scbStride);
}

#ifdef GENERATE_AVX2
template void tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CBGRColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRGToRGB_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CBGRAColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, const uint8_t *pAPacked, const uint8_t *pAControl, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULRAToRGBA_Unpack8SymAndRestorePlanarGradient8<CODEFEATURE_AVX2, CARGBColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pGPacked, const uint8_t *pGControl, const uint8_t *pBPacked, const uint8_t *pBControl, const uint8_t *pRPacked, const uint8_t *pRControl, const uint8_t *pAPacked, const uint8_t *pAControl, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, typename T>
void tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	auto yp = pYPacked;
	auto up = pUPacked;
	auto vp = pVPacked;

	auto yc = pYControl;
	auto uc = pUControl;
	auto vc = pVControl;

	size_t cbYControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + 127) / 128) * 3;
	size_t cbCControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + 255) / 256) * 3;
	memset(pYControl, 0, cbYControl);
	memset(pUControl, 0, cbCControl);
	memset(pVControl, 0, cbCControl);

	union padsolve
	{
		uint8_t b[64];
		__m256i ymm[2];
	};

	{
		const auto p = pSrcBegin;
		auto pp = p;

		__m256i yprev = _mm256_set1_epi8((char)0x80);
		__m256i uprev = _mm256_set1_epi8((char)0x80);
		__m256i vprev = _mm256_set1_epi8((char)0x80);

		for (; pp <= p + cbWidth - 256; pp += 256)
		{
			auto planar0 = tuned_ConvertPackedYUV422ToPlanarElement<F, __m256i, T>(pp);
			auto planar1 = tuned_ConvertPackedYUV422ToPlanarElement<F, __m256i, T>(pp + 128);

			__m256i yresidual0 = tuned_PredictLeft8Element<F>(yprev, planar0.y0);
			__m256i yresidual1 = tuned_PredictLeft8Element<F>(planar0.y0, planar0.y1);
			__m256i yresidual2 = tuned_PredictLeft8Element<F>(planar0.y1, planar1.y0);
			__m256i yresidual3 = tuned_PredictLeft8Element<F>(planar1.y0, planar1.y1);
			__m256i uresidual0 = tuned_PredictLeft8Element<F>(uprev, planar0.u);
			__m256i uresidual1 = tuned_PredictLeft8Element<F>(planar0.u, planar1.u);
			__m256i vresidual0 = tuned_PredictLeft8Element<F>(vprev, planar0.v);
			__m256i vresidual1 = tuned_PredictLeft8Element<F>(planar0.v, planar1.v);

			PackForIntra<F>(yp, yc, yresidual0, yresidual1);
			PackForIntra<F>(yp, yc, yresidual2, yresidual3);
			PackForIntra<F>(up, uc, uresidual0, uresidual1);
			PackForIntra<F>(vp, vc, vresidual0, vresidual1);

			yprev = planar1.y1;
			uprev = planar1.u;
			vprev = planar1.v;
		}

		if (pp < p + cbWidth)
		{
			uint8_t yprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(yprev, 1), 15));
			uint8_t uprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(uprev, 1), 15));
			uint8_t vprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(vprev, 1), 15));

			int n = 0;
			padsolve yps[2], ups, vps;

			for (; pp < p + cbWidth && n < 64; pp += 4, ++n)
			{
				uint8_t yy0 = pp[T::Y0];
				yps[0].b[n * 2] = yy0 - yprevb;
				uint8_t yy1 = pp[T::Y1];
				yps[0].b[n * 2 + 1] = yy1 - yy0;
				uint8_t uu = pp[T::U];
				ups.b[n] = uu - uprevb;
				uint8_t vv = pp[T::V];
				vps.b[n] = vv - vprevb;

				yprevb = yy1;
				uprevb = uu;
				vprevb = vv;
			}
			int m = n;
			for (; n < 64; ++n)
			{
				yps[0].b[n * 2] = 0;
				yps[0].b[n * 2 + 1] = 0;
				ups.b[n] = 0;
				vps.b[n] = 0;
			}

			PackForIntra<F>(yp, yc, yps[0].ymm[0], yps[0].ymm[1]);
			if (m > 32)
				PackForIntra<F>(yp, yc, yps[1].ymm[0], yps[1].ymm[1]);
			PackForIntra<F>(up, uc, ups.ymm[0], ups.ymm[1]);
			PackForIntra<F>(vp, vc, vps.ymm[0], vps.ymm[1]);
		}
	}

	for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m256i yprev = _mm256_setzero_si256();
		__m256i uprev = _mm256_setzero_si256();
		__m256i vprev = _mm256_setzero_si256();

		for (; pp <= p + cbWidth - 256; pp += 256)
		{
			auto planar0 = tuned_ConvertPackedYUV422ToPlanarElement<F, __m256i, T>(pp, scbStride);
			auto planar1 = tuned_ConvertPackedYUV422ToPlanarElement<F, __m256i, T>(pp + 128, scbStride);

			__m256i yresidual0 = tuned_PredictLeft8Element<F>(yprev, planar0.y0);
			__m256i yresidual1 = tuned_PredictLeft8Element<F>(planar0.y0, planar0.y1);
			__m256i yresidual2 = tuned_PredictLeft8Element<F>(planar0.y1, planar1.y0);
			__m256i yresidual3 = tuned_PredictLeft8Element<F>(planar1.y0, planar1.y1);
			__m256i uresidual0 = tuned_PredictLeft8Element<F>(uprev, planar0.u);
			__m256i uresidual1 = tuned_PredictLeft8Element<F>(planar0.u, planar1.u);
			__m256i vresidual0 = tuned_PredictLeft8Element<F>(vprev, planar0.v);
			__m256i vresidual1 = tuned_PredictLeft8Element<F>(planar0.v, planar1.v);

			PackForIntra<F>(yp, yc, yresidual0, yresidual1);
			PackForIntra<F>(yp, yc, yresidual2, yresidual3);
			PackForIntra<F>(up, uc, uresidual0, uresidual1);
			PackForIntra<F>(vp, vc, vresidual0, vresidual1);

			yprev = planar1.y1;
			uprev = planar1.u;
			vprev = planar1.v;
		}

		if (pp < p + cbWidth)
		{
			uint8_t yprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(yprev, 1), 15));
			uint8_t uprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(uprev, 1), 15));
			uint8_t vprevb = _mm_cvtsi128_si32(_mm_srli_si128(_mm256_extracti128_si256(vprev, 1), 15));

			int n = 0;
			padsolve yps[2], ups, vps;

			for (; pp < p + cbWidth && n < 64; pp += 4, ++n)
			{
				uint8_t yy0 = pp[T::Y0] - (pp - scbStride)[T::Y0];
				yps[0].b[n * 2] = yy0 - yprevb;
				uint8_t yy1 = pp[T::Y1] - (pp - scbStride)[T::Y1];
				yps[0].b[n * 2 + 1] = yy1 - yy0;
				uint8_t uu = pp[T::U] - (pp - scbStride)[T::U];
				ups.b[n] = uu - uprevb;
				uint8_t vv = pp[T::V] - (pp - scbStride)[T::V];
				vps.b[n] = vv - vprevb;

				yprevb = yy1;
				uprevb = uu;
				vprevb = vv;
			}
			int m = n;
			for (; n < 64; ++n)
			{
				yps[0].b[n * 2] = 0;
				yps[0].b[n * 2 + 1] = 0;
				ups.b[n] = 0;
				vps.b[n] = 0;
			}

			PackForIntra<F>(yp, yc, yps[0].ymm[0], yps[0].ymm[1]);
			if (m > 32)
				PackForIntra<F>(yp, yc, yps[1].ymm[0], yps[1].ymm[1]);
			PackForIntra<F>(up, uc, ups.ymm[0], ups.ymm[1]);
			PackForIntra<F>(vp, vc, vps.ymm[0], vps.ymm[1]);
		}
	}

	*cbYPacked = yp - pYPacked;
	*cbUPacked = up - pUPacked;
	*cbVPacked = vp - pVPacked;
}

#ifdef GENERATE_AVX2
template void tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CYUYVColorOrder>(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertPackedYUV422ToULY2_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX2, CUYVYColorOrder>(uint8_t *pYPacked, size_t *cbYPacked, uint8_t *pYControl, uint8_t *pUPacked, size_t *cbUPacked, uint8_t *pUControl, uint8_t *pVPacked, size_t *cbVPacked, uint8_t *pVControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

//

template<int F, typename T>
void tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYPacked, const uint8_t *pYControl, const uint8_t *pUPacked, const uint8_t *pUControl, const uint8_t *pVPacked, const uint8_t *pVControl, size_t cbWidth, ssize_t scbStride)
{
	auto yp = pYPacked;
	auto up = pUPacked;
	auto vp = pVPacked;

	auto yc = pYControl;
	auto uc = pUControl;
	auto vc = pVControl;

	union padsolve
	{
		uint8_t b[32];
		__m256i ymm;
	};

	{
		const auto p = pDstBegin;
		auto pp = p;

		__m256i yprev = _mm256_set1_epi8((char)0x80);
		__m256i uprev = _mm256_set1_epi8((char)0x80);
		__m256i vprev = _mm256_set1_epi8((char)0x80);

		int yshift = 0;
		int ushift = 0;
		int vshift = 0;

		for (; pp <= p + cbWidth - 128; pp += 128)
		{
			__m256i yresidual0 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i yresidual1 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i uresidual = UnpackForIntra<F>(up, uc, ushift);
			__m256i vresidual = UnpackForIntra<F>(vp, vc, vshift);

			auto yvalue0 = tuned_RestoreLeft8Element<F>(yprev, yresidual0);
			auto yvalue1 = tuned_RestoreLeft8Element<F>(yvalue0.v1, yresidual1);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uresidual);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vresidual);

			tuned_ConvertPlanarYUV422ToPackedElement<F, __m256i, T>(pp, yvalue0.v0, yvalue1.v0, uvalue.v0, vvalue.v0);

			yprev = yvalue1.v1;
			uprev = uvalue.v1;
			vprev = vvalue.v1;
		}

		if (pp < p + cbWidth)
		{
			__m256i yresidual0 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i yresidual1 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i uresidual = UnpackForIntra<F>(up, uc, ushift);
			__m256i vresidual = UnpackForIntra<F>(vp, vc, vshift);

			auto yvalue0 = tuned_RestoreLeft8Element<F>(yprev, yresidual0);
			auto yvalue1 = tuned_RestoreLeft8Element<F>(yvalue0.v1, yresidual1);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uresidual);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vresidual);

			int n = 0;
			padsolve yps[2], ups, vps;

			yps[0].ymm = yvalue0.v0;
			yps[1].ymm = yvalue1.v0;
			ups.ymm = uvalue.v0;
			vps.ymm = vvalue.v0;

			for (; pp < p + cbWidth; pp += 4, ++n)
			{
				pp[T::Y0] = yps[0].b[n * 2];
				pp[T::Y1] = yps[0].b[n * 2 + 1];
				pp[T::U] = ups.b[n];
				pp[T::V] = vps.b[n];
			}
		}

		if (ushift != 0)
		{
			uc += 3;
			vc += 3;
		}
	}

	for (auto p = pDstBegin + scbStride; p != pDstEnd; p += scbStride)
	{
		auto pp = p;

		__m256i yprev = _mm256_setzero_si256();
		__m256i uprev = _mm256_setzero_si256();
		__m256i vprev = _mm256_setzero_si256();

		int yshift = 0;
		int ushift = 0;
		int vshift = 0;

		for (; pp <= p + cbWidth - 128; pp += 128)
		{
			__m256i yresidual0 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i yresidual1 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i uresidual = UnpackForIntra<F>(up, uc, ushift);
			__m256i vresidual = UnpackForIntra<F>(vp, vc, vshift);

			auto yvalue0 = tuned_RestoreLeft8Element<F>(yprev, yresidual0);
			auto yvalue1 = tuned_RestoreLeft8Element<F>(yvalue0.v1, yresidual1);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uresidual);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vresidual);

			tuned_ConvertPlanarYUV422ToPackedElement<F, __m256i, T>(pp, yvalue0.v0, yvalue1.v0, uvalue.v0, vvalue.v0, scbStride);

			yprev = yvalue1.v1;
			uprev = uvalue.v1;
			vprev = vvalue.v1;
		}

		if (pp < p + cbWidth)
		{
			__m256i yresidual0 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i yresidual1 = UnpackForIntra<F>(yp, yc, yshift);
			__m256i uresidual = UnpackForIntra<F>(up, uc, ushift);
			__m256i vresidual = UnpackForIntra<F>(vp, vc, vshift);

			auto yvalue0 = tuned_RestoreLeft8Element<F>(yprev, yresidual0);
			auto yvalue1 = tuned_RestoreLeft8Element<F>(yvalue0.v1, yresidual1);
			auto uvalue = tuned_RestoreLeft8Element<F>(uprev, uresidual);
			auto vvalue = tuned_RestoreLeft8Element<F>(vprev, vresidual);

			int n = 0;
			padsolve yps[2], ups, vps;

			yps[0].ymm = yvalue0.v0;
			yps[1].ymm = yvalue1.v0;
			ups.ymm = uvalue.v0;
			vps.ymm = vvalue.v0;

			for (; pp < p + cbWidth; pp += 4, ++n)
			{
				pp[T::Y0] = yps[0].b[n * 2] + (pp - scbStride)[T::Y0];
				pp[T::Y1] = yps[0].b[n * 2 + 1] + (pp - scbStride)[T::Y1];
				pp[T::U] = ups.b[n] + (pp - scbStride)[T::U];
				pp[T::V] = vps.b[n] + (pp - scbStride)[T::V];
			}
		}

		if (ushift != 0)
		{
			uc += 3;
			vc += 3;
		}
	}
}

#ifdef GENERATE_AVX2
template void tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX2, CYUYVColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYPacked, const uint8_t *pYControl, const uint8_t *pUPacked, const uint8_t *pUControl, const uint8_t *pVPacked, const uint8_t *pVControl, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertULY2ToPackedYUV422_Unpack8SymAndRestorePredictPlanarGradient8<CODEFEATURE_AVX2, CUYVYColorOrder>(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYPacked, const uint8_t *pYControl, const uint8_t *pUPacked, const uint8_t *pUControl, const uint8_t *pVPacked, const uint8_t *pVControl, size_t cbWidth, ssize_t scbStride);
#endif
