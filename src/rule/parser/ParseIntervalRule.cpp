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
		R"(^(-?\d+(?:\.\d+)?)(<|>)(=)?(?:X|x)?(<|>)(=)?(-?\d+(?:\.\d+)?)$)"   // NUMBER<(=)(X)<(=)NUMBER
		                                                                      // NUMBER>(=)(X)>(=)NUMBER
		                                                                      //   Capture groups:
		                                                                      //    - 1: first number
		                                                                      //    - 2: first less-than or greater-than sign
		                                                                      //    - 3: first equal sign (optional)
		                                                                      //    - 4: second less-than or greater-than sign
		                                                                      //    - 5: second equal sign (optional)
		                                                                      //    - 6: second number
		R"(|^(-?\d+(?:\.\d+)?)(<|>)(=)?(?:X|x)?$)"                            // NUMBER<(=)(X)
		                                                                      // NUMBER>(=)(X)
		                                                                      //   Capture groups:
		                                                                      //    - 7: number
		                                                                      //    - 8: less-than or greater-than sign
		                                                                      //    - 9: equal sign (optional)
		R"(|^(?:X|x)?(<|>)(=)?(-?\d+(?:\.\d+)?)$)",                           // (X)<(=)NUMBER
		                                                                      // (X)>(=)NUMBER
		                                                                      //   Capture groups:
		                                                                      //    - 10: less-than or greater-than sign
		                                                                      //    - 11: equal sign (optional)
		                                                                      //    - 12: number
		regex_constants::optimize
	);

	shared_ptr<Rule> parseIntervalRule(string_view category)
	{
		cmatch match;
		if (regex_match(category.data(), category.data() + category.size(), match, INTERVAL_REGEX))
		{
			assert(match.size() == 13);

			if (match[1].matched)
			{
				const char firstRelationalSymbol = match[2].first[0];
				const char secondRelationalSymbol = match[4].first[0];

				if (firstRelationalSymbol != secondRelationalSymbol)
					return parseLiteralRule(category);

				const Threshold threshold1 = createThreshold(match[1], match[3]);
				const Threshold threshold2 = createThreshold(match[6], match[5]);

				if (!threshold1.enabled || !threshold2.enabled)
					return parseLiteralRule(category);

				const Threshold& lowerBound = firstRelationalSymbol == '<' ? threshold1 : threshold2;
				const Threshold& upperBound = firstRelationalSymbol == '<' ? threshold2 : threshold1;

				return make_shared<IntervalRule>(category, lowerBound, upperBound);
			}

			if (match[7].matched)
			{
				const char relationalSymbol = match[8].first[0];

				const Threshold threshold = createThreshold(match[7], match[9]);

				if (!threshold.enabled)
					return parseLiteralRule(category);

				const Threshold& lowerBound = relationalSymbol == '<' ? threshold : Threshold();
				const Threshold& upperBound = relationalSymbol == '<' ? Threshold() : threshold;

				return make_shared<IntervalRule>(category, lowerBound, upperBound);
			}

			if (match[12].matched)
			{
				const char relationalSymbol = match[10].first[0];

				const Threshold threshold = createThreshold(match[12], match[11]);

				if (!threshold.enabled)
					return parseLiteralRule(category);

				const Threshold& lowerBound = relationalSymbol == '<' ? Threshold() : threshold;
				const Threshold& upperBound = relationalSymbol == '<' ? threshold : Threshold();

				return make_shared<IntervalRule>(category, lowerBound, upperBound);
			}
		}

		return parseLiteralRule(category);
	}
}
