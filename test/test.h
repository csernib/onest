#pragma once

#define lest_FEATURE_AUTO_REGISTER 1
#include <lest/lest.hpp>


lest::tests& test_specification();

#undef CASE
#define CASE(Name) lest_CASE(test_specification(), Name)

#undef SETUP
#undef SECTION
#define SETUP lest_SETUP("")
#define SECTION lest_SECTION("")


namespace test
{
	// Write common code shared by all tests here.
}
