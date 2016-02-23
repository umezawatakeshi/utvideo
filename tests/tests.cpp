/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#ifdef _M_X64
#ifndef __x86_64__
#define __x86_64__ 1
#endif
#endif

#ifdef _M_IX86
#ifndef __i386__
#define __i386__ 1
#endif
#endif

#ifdef __i386__
#define BOOST_TEST_MODULE tests-x86
#endif

#ifdef __x86_64__
#define BOOST_TEST_MODULE tests-x64
#endif

#include <boost/test/unit_test.hpp>
