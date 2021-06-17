/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
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
