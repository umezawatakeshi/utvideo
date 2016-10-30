/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "Compare.h"

int CompareFrame(const void *frame1, const void *frame2, unsigned int width, size_t size, DWORD format, int tolerance)
{
	switch (format)
	{
	case 24:
		{
			const uint8_t *begin1 = (const uint8_t *)frame1;
			const uint8_t *begin2 = (const uint8_t *)frame2;
			const uint8_t *end1 = begin1 + size;
			size_t net = width * (size_t)3;
			size_t stride = (net + 3) & ~(size_t)3;
			for (auto p = begin1, q = begin2; p < end1; p += stride, q += stride)
			{
				for (auto pp = p, qq = q; pp != p + net; ++pp, ++qq)
				{
					int r = (int)*pp - (int)*qq;
					if (abs(r) > tolerance)
						return r;
				}
			}
			return 0;
		}
	}

	{
		const uint8_t *begin1 = (const uint8_t *)frame1;
		const uint8_t *begin2 = (const uint8_t *)frame2;
		const uint8_t *end1 = begin1 + size;
		for (auto p = begin1, q = begin2; p != end1; ++p, ++q)
		{
			int r = (int)*p - (int)*q;
			if (abs(r) > tolerance)
				return r;
		}
		return 0;
	}
}
