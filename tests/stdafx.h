/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#include <windows.h>
#include <vfw.h>
#include <aviriff.h>

#include <dmo.h>
#include <uuids.h>
#include <dshow.h>

#include <stdio.h>
#include <tchar.h>

#include <vector>
#include <string>

#include <boost/test/unit_test.hpp> 
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

using std::vector;
using std::wstring;
using std::ostream;

namespace data = boost::unit_test::data;
using boost::unit_test::depends_on;
