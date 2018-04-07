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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

extern "C"
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <libavutil/log.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

void av_log_custom_callback(void *avcl, int level, const char *fmt, va_list vl)
{
	if (level == AV_LOG_WARNING && strstr(fmt, "Could not find codec parameters") != NULL)
		return;
	av_log_default_callback(avcl, level, fmt, vl);
}

int main(int argc, char* argv[], char* envp[])
{
	av_log_set_callback(av_log_custom_callback);
	return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
