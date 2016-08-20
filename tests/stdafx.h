/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#define NOMINMAX 1

#include <windows.h>
#include <vfw.h>
#include <aviriff.h>

#include <dmo.h>
#include <uuids.h>
#include <dshow.h>

#include <stdio.h>
#include <tchar.h>

#include <utility>
#include <tuple>
#include <vector>
#include <string>
#include <iostream>

#include <boost/test/unit_test.hpp> 
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

using std::pair;
using std::tuple;
using std::make_tuple;
using std::vector;
using std::string;
using std::wstring;
using std::ostream;
using std::cerr;
using std::endl;

namespace data = boost::unit_test::data;
using boost::unit_test::depends_on;
