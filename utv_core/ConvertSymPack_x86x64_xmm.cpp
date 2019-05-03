/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <myintrin_x86x64.h>
#include "ByteOrder.h"

#if !defined(GENERATE_SSE2) && !defined(GENERATE_SSSE3) && !defined(GENERATE_SSE41) && !defined(GENERATE_AVX1)
#error
#endif

template<int F>
static inline void tuned_ConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	typedef CBGRColorOrder T;

	auto gp = pGPacked;
	auto bp = pBPacked;
	auto rp = pRPacked;

	auto gc = pGControl;
	auto bc = pBControl;
	auto rc = pRControl;

	size_t cbControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + 191) / 192) * 3;
	memset(pGControl, 0, cbControl);
	memset(pBControl, 0, cbControl);
	memset(pRControl, 0, cbControl);

	int gshift = 0;
	int bshift = 0;
	int rshift = 0;

	union padsolve
	{
		uint8_t b[32];
		__m128i xmm[2];
	};

	{
		const auto p = pSrcBegin;
		auto pp = p;

		__m128i gprev = _mm_set1_epi8((char)0x80);
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 96; pp += 96)
		{
			auto planar0 = tuned_ConvertPackedBGRToPlanarElement<F, false>(pp);
			auto planar1 = tuned_ConvertPackedBGRToPlanarElement<F, false>(pp + 48);

			__m128i gresidual0 = tuned_PredictLeft8Element<F>(gprev, planar0.g);
			__m128i gresidual1 = tuned_PredictLeft8Element<F>(planar0.g, planar1.g);
			__m128i bresidual0 = tuned_PredictLeft8Element<F>(bprev, planar0.b);
			__m128i bresidual1 = tuned_PredictLeft8Element<F>(planar0.b, planar1.b);
			__m128i rresidual0 = tuned_PredictLeft8Element<F>(rprev, planar0.r);
			__m128i rresidual1 = tuned_PredictLeft8Element<F>(planar0.r, planar1.r);

			PackForIntra<F>(gp, gc, gshift, gresidual0, gresidual1);
			PackForIntra<F>(bp, bc, bshift, bresidual0, bresidual1);
			PackForIntra<F>(rp, rc, rshift, rresidual0, rresidual1);

			gprev = planar1.g;
			bprev = planar1.b;
			rprev = planar1.r;
		}

		if (pp != p + (cbWidth + 191) / 192 * 192)
		{
			uint8_t gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
			uint8_t bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
			uint8_t rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));

			int n = 0;
			padsolve gps, bps, rps;

			for (; pp < p + cbWidth && n < 32; pp += 3, ++n)
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
			}
			for (; n < 32; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
			}

			PackForIntra<F>(gp, gc, gshift, gps.xmm[0], gps.xmm[1]);
			PackForIntra<F>(bp, bc, bshift, bps.xmm[0], bps.xmm[1]);
			PackForIntra<F>(rp, rc, rshift, rps.xmm[0], rps.xmm[1]);

			if (gshift != 0)
			{
				gshift = 0;
				bshift = 0;
				rshift = 0;

				gc += 3;
				bc += 3;
				rc += 3;
			}
		}
	}

	for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 96; pp += 96)
		{
			auto planar0 = tuned_ConvertPackedBGRToPlanarElement<F, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32)))
			);
			auto planar1 = tuned_ConvertPackedBGRToPlanarElement<F, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 64)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 64))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 80)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 80)))
			);

			__m128i gresidual0 = tuned_PredictLeft8Element<F>(gprev, planar0.g);
			__m128i gresidual1 = tuned_PredictLeft8Element<F>(planar0.g, planar1.g);
			__m128i bresidual0 = tuned_PredictLeft8Element<F>(bprev, planar0.b);
			__m128i bresidual1 = tuned_PredictLeft8Element<F>(planar0.b, planar1.b);
			__m128i rresidual0 = tuned_PredictLeft8Element<F>(rprev, planar0.r);
			__m128i rresidual1 = tuned_PredictLeft8Element<F>(planar0.r, planar1.r);

			PackForIntra<F>(gp, gc, gshift, gresidual0, gresidual1);
			PackForIntra<F>(bp, bc, bshift, bresidual0, bresidual1);
			PackForIntra<F>(rp, rc, rshift, rresidual0, rresidual1);

			gprev = planar1.g;
			bprev = planar1.b;
			rprev = planar1.r;
		}

		if (pp != p + (cbWidth + 191) / 192 * 192)
		{
			uint8_t gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
			uint8_t bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
			uint8_t rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));

			int n = 0;
			padsolve gps, bps, rps;

			for (; pp < p + cbWidth && n < 32; pp += 3, ++n)
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
			}
			for (; n < 32; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
			}

			PackForIntra<F>(gp, gc, gshift, gps.xmm[0], gps.xmm[1]);
			PackForIntra<F>(bp, bc, bshift, bps.xmm[0], bps.xmm[1]);
			PackForIntra<F>(rp, rc, rshift, rps.xmm[0], rps.xmm[1]);

			if (gshift != 0)
			{
				gshift = 0;
				bshift = 0;
				rshift = 0;

				gc += 3;
				bc += 3;
				rc += 3;
			}
		}
	}

	*cbGPacked = gp - pGPacked;
	*cbBPacked = bp - pBPacked;
	*cbRPacked = rp - pRPacked;
}

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

	size_t cbControl = (pSrcEnd - pSrcBegin) / scbStride * ((cbWidth + 255) / 256) * 3;
	memset(pGControl, 0, cbControl);
	memset(pBControl, 0, cbControl);
	memset(pRControl, 0, cbControl);
	if (A)
		memset(pAControl, 0, cbControl);

	int gshift = 0;
	int bshift = 0;
	int rshift = 0;
	int ashift = 0;

	union padsolve
	{
		uint8_t b[32];
		__m128i xmm[2];
	};

	{
		const auto p = pSrcBegin;
		auto pp = p;

		__m128i gprev = _mm_set1_epi8((char)0x80);
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();
		__m128i aprev;
		if (A)
			aprev = _mm_set1_epi8((char)0x80);

		for (; pp <= p + cbWidth - 128; pp += 128)
		{
			auto planar0 = tuned_ConvertPackedRGBXToPlanarElement<F, T, false>(pp);
			auto planar1 = tuned_ConvertPackedRGBXToPlanarElement<F, T, false>(pp + 64);

			__m128i gresidual0 = tuned_PredictLeft8Element<F>(gprev, planar0.g);
			__m128i gresidual1 = tuned_PredictLeft8Element<F>(planar0.g, planar1.g);
			__m128i bresidual0 = tuned_PredictLeft8Element<F>(bprev, planar0.b);
			__m128i bresidual1 = tuned_PredictLeft8Element<F>(planar0.b, planar1.b);
			__m128i rresidual0 = tuned_PredictLeft8Element<F>(rprev, planar0.r);
			__m128i rresidual1 = tuned_PredictLeft8Element<F>(planar0.r, planar1.r);

			PackForIntra<F>(gp, gc, gshift, gresidual0, gresidual1);
			PackForIntra<F>(bp, bc, bshift, bresidual0, bresidual1);
			PackForIntra<F>(rp, rc, rshift, rresidual0, rresidual1);

			gprev = planar1.g;
			bprev = planar1.b;
			rprev = planar1.r;

			if (A)
			{
				__m128i aresidual0 = tuned_PredictLeft8Element<F>(aprev, planar0.a);
				__m128i aresidual1 = tuned_PredictLeft8Element<F>(planar0.a, planar1.a);
				PackForIntra<F>(ap, ac, ashift, aresidual0, aresidual1);
				aprev = planar1.a;
			}
		}

		if (pp != p + (cbWidth + 255) / 256 * 256)
		{
			uint8_t gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
			uint8_t bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
			uint8_t rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
			uint8_t aprevb;
			if (A)
				aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 15));

			int n = 0;
			padsolve gps, bps, rps, aps;

			for (; pp < p + cbWidth && n < 32; pp += 4, ++n)
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
			for (; n < 32; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
				if (A)
					aps.b[n] = 0;
			}

			PackForIntra<F>(gp, gc, gshift, gps.xmm[0], gps.xmm[1]);
			PackForIntra<F>(bp, bc, bshift, bps.xmm[0], bps.xmm[1]);
			PackForIntra<F>(rp, rc, rshift, rps.xmm[0], rps.xmm[1]);
			if (A)
				PackForIntra<F>(ap, ac, ashift, aps.xmm[0], aps.xmm[1]);

			if (gshift != 0)
			{
				gshift = 0;
				bshift = 0;
				rshift = 0;

				gc += 3;
				bc += 3;
				rc += 3;

				if (A)
				{
					ashift = 0;
					ac += 3;
				}
			}
		}
	}

	for (auto p = pSrcBegin + scbStride; p != pSrcEnd; p += scbStride)
	{
		auto pp = p;

		__m128i gprev = _mm_setzero_si128();
		__m128i bprev = _mm_setzero_si128();
		__m128i rprev = _mm_setzero_si128();
		__m128i aprev;
		if (A)
			aprev = _mm_setzero_si128();

		for (; pp <= p + cbWidth - 128; pp += 128)
		{
			auto planar0 = tuned_ConvertPackedRGBXToPlanarElement<F, T, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)pp), _mm_loadu_si128((const __m128i *)(pp - scbStride))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 16)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 16))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 32)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 32))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 48)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 48)))
			);
			auto planar1 = tuned_ConvertPackedRGBXToPlanarElement<F, T, false>(
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 64)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 64))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 80)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 80))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 96)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 96))),
				_mm_sub_epi8(_mm_loadu_si128((const __m128i *)(pp + 112)), _mm_loadu_si128((const __m128i *)(pp - scbStride + 112)))
			);

			__m128i gresidual0 = tuned_PredictLeft8Element<F>(gprev, planar0.g);
			__m128i gresidual1 = tuned_PredictLeft8Element<F>(planar0.g, planar1.g);
			__m128i bresidual0 = tuned_PredictLeft8Element<F>(bprev, planar0.b);
			__m128i bresidual1 = tuned_PredictLeft8Element<F>(planar0.b, planar1.b);
			__m128i rresidual0 = tuned_PredictLeft8Element<F>(rprev, planar0.r);
			__m128i rresidual1 = tuned_PredictLeft8Element<F>(planar0.r, planar1.r);

			PackForIntra<F>(gp, gc, gshift, gresidual0, gresidual1);
			PackForIntra<F>(bp, bc, bshift, bresidual0, bresidual1);
			PackForIntra<F>(rp, rc, rshift, rresidual0, rresidual1);

			gprev = planar1.g;
			bprev = planar1.b;
			rprev = planar1.r;

			if (A)
			{
				__m128i aresidual0 = tuned_PredictLeft8Element<F>(aprev, planar0.a);
				__m128i aresidual1 = tuned_PredictLeft8Element<F>(planar0.a, planar1.a);
				PackForIntra<F>(ap, ac, ashift, aresidual0, aresidual1);
				aprev = planar1.a;
			}
		}

		if (pp != p + (cbWidth + 255) / 256 * 256)
		{
			uint8_t gprevb = _mm_cvtsi128_si32(_mm_srli_si128(gprev, 15));
			uint8_t bprevb = _mm_cvtsi128_si32(_mm_srli_si128(bprev, 15));
			uint8_t rprevb = _mm_cvtsi128_si32(_mm_srli_si128(rprev, 15));
			uint8_t aprevb;
			if (A)
				aprevb = _mm_cvtsi128_si32(_mm_srli_si128(aprev, 15));

			int n = 0;
			padsolve gps, bps, rps, aps;

			for (; pp < p + cbWidth && n < 32; pp += 4, ++n)
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
			for (; n < 32; ++n)
			{
				gps.b[n] = 0;
				bps.b[n] = 0;
				rps.b[n] = 0;
				if (A)
					aps.b[n] = 0;
			}

			PackForIntra<F>(gp, gc, gshift, gps.xmm[0], gps.xmm[1]);
			PackForIntra<F>(bp, bc, bshift, bps.xmm[0], bps.xmm[1]);
			PackForIntra<F>(rp, rc, rshift, rps.xmm[0], rps.xmm[1]);
			if (A)
				PackForIntra<F>(ap, ac, ashift, aps.xmm[0], aps.xmm[1]);

			if (gshift != 0)
			{
				gshift = 0;
				bshift = 0;
				rshift = 0;

				gc += 3;
				bc += 3;
				rc += 3;

				if (A)
				{
					ashift = 0;
					ac += 3;
				}
			}
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
	if (std::is_same<T, CBGRColorOrder>::value)
		tuned_ConvertBGRToULRG_Pack8SymAfterPredictPlanarGradient8<F>(pGPacked, cbGPacked, pGControl, pBPacked, cbBPacked, pBControl, pRPacked, cbRPacked, pRControl, pSrcBegin, pSrcEnd, cbWidth, scbStride);
	else
		tuned_ConvertRGBXToULRX_Pack8SymAfterPredictPlanarGradient8<F, T, false>(pGPacked, cbGPacked, pGControl, pBPacked, cbBPacked, pBControl, pRPacked, cbRPacked, pRControl, NULL, NULL, NULL, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

template<int F, class T>
void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride)
{
	tuned_ConvertRGBXToULRX_Pack8SymAfterPredictPlanarGradient8<F, T, true>(pGPacked, cbGPacked, pGControl, pBPacked, cbBPacked, pBControl, pRPacked, cbRPacked, pRControl, pAPacked, cbAPacked, pAControl, pSrcBegin, pSrcEnd, cbWidth, scbStride);
}

#ifdef GENERATE_SSE41
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CBGRColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_SSE41, CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif

#ifdef GENERATE_AVX1
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CBGRColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBToULRG_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CBGRAColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
template void tuned_ConvertRGBAToULRA_Pack8SymAfterPredictPlanarGradient8<CODEFEATURE_AVX1, CARGBColorOrder>(uint8_t *pGPacked, size_t *cbGPacked, uint8_t *pGControl, uint8_t *pBPacked, size_t *cbBPacked, uint8_t *pBControl, uint8_t *pRPacked, size_t *cbRPacked, uint8_t *pRControl, uint8_t *pAPacked, size_t *cbAPacked, uint8_t *pAControl, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbWidth, ssize_t scbStride);
#endif
