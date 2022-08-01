#include "ParseIntervalRule.h"
#include "ParseLiteralRule.h"

#include "../IntervalRule.h"

#include <cassert>
#include <regex>


using namespace std;

typedef onest::rule::IntervalRule::Threshold Threshold;

namespace
{
	bool convert(csub_match submatch, double& value)
	{
		auto r = from_chars(submatch.first, submatch.second, value, chars_format::fixed);
		if (r.ptr == submatch.second && r.ec == errc())
			return true;

		return false;
	}

	Threshold createThreshold(csub_match number, csub_match equalSign)
	{
		Threshold threshold;
		threshold.allowEqual = equalSign.matched;
		threshold.enabled = convert(number, threshold.value);
		return threshold;
	}
}

namespace onest::rule::parser
{
	const regex INTERVAL_REGEX = regex(
		R"(^(-?\d+(?:\.\d+)?)<(=)?<(=)?(-?\d+(?:\.\d+)?)$)"   // NUMBER<(=)<(=)NUMBER
		                                                      //   Capture groups:
		                                                      //    - 1: first number
		                                                      //    - 2: first equal sign (optional)
		                                                      //    - 3: second equal sign (optional)
		                                                      //    - 4: second number
		R"(|^(-?\d+(?:\.\d+)?)<(=)?$)"                        // NUMBER<(=)
		                                                      //   Capture groups:
		                                                      //    - 5: number
		                                                      //    - 6: equal sign (optional)
		R"(|^<(=)?(-?\d+(?:\.\d+)?)$)",                       // <(=)NUMBER
		                                                      //   Capture groups:
		                                                      //    - 7: equal sign (optional)
		                                                      //    - 8: number
		regex_constants::optimize
	);

	shared_ptr<Rule> parseIntervalRule(string_view category)
	{
		cmatch match;
		if (regex_match(category.data(), category.data() + category.size(), match, INTERVAL_REGEX))
		{
			assert(match.size() == 9);

			if (match[1].matched)
			{
				Threshold lowerBound = createThreshold(match[1], match[2]);
				Threshold upperBound = createThreshold(match[4], match[3]);

				if (!lowerBound.enabled || !upperBound.enabled)
					return parseLiteralRule(category);

				return make_shared<IntervalRule>(category, lowerBound, upperBound);
			}

			if (match[5].matched)
			{
				Threshold lowerBound = createThreshold(match[5], match[6]);
				if (!lowerBound.enabled)
					return parseLiteralRule(category);

				return make_shared<IntervalRule>(category, lowerBound, Threshold());
			}

			if (match[8].matched)
			{
				Threshold upperBound = createThreshold(match[8], match[7]);
				if (!upperBound.enabled)
					return parseLiteralRule(category);

				return make_shared<IntervalRule>(category, Threshold(), upperBound);
			}
		}

		return parseLiteralRule(category);
	}
}
