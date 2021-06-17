/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "aligned_malloc.h"

#define ALLOCATION_ALIGNMENT 4096

void* aligned_malloc(size_t sz, size_t alignment)
{
	return (char*)_aligned_malloc(sz + alignment, ALLOCATION_ALIGNMENT) + alignment;
}

void aligned_free(void* p)
{
	_aligned_free((void*)((uintptr_t)p & ~(uintptr_t)(ALLOCATION_ALIGNMENT - 1)));
}

const std::vector<size_t> vecAlignments = { 0, 1, 2, 4, 8, 16, 32, 64 };

BOOST_DATA_TEST_CASE(aligned_malloc_test, data::make(vecAlignments), alignment)
{
	if (alignment == 0)
		return;

	void* p = aligned_malloc(967, alignment);
	BOOST_REQUIRE(p != NULL);
	BOOST_CHECK((uintptr_t)p % alignment == 0);
	BOOST_CHECK((uintptr_t)p % (alignment * 2) != 0);
	aligned_free(p);
}
