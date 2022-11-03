#include "../src/rule/parser/ParseIntervalRule.h"
#include "../src/rule/IntervalRule.h"
#include "../src/rule/LiteralRule.h"
#include "test.h"

#include <limits>


#define TAG "[RuleParser] "

using namespace std;

using onest::rule::parser::parseIntervalRule;
using onest::rule::IntervalRule;
using onest::rule::LiteralRule;

typedef IntervalRule::Threshold Threshold;

namespace
{
	bool compareThreshold(const Threshold& lhs, const Threshold& rhs)
	{
		return abs(lhs.value - rhs.value) < numeric_limits<decltype(lhs.value)>::epsilon() &&
		       lhs.allowEqual == rhs.allowEqual &&
		       lhs.enabled &&
		       rhs.enabled;
	}

	bool check(string_view s, Threshold lowerBound, Threshold upperBound)
	{
		auto rule = parseIntervalRule(s);
		if (!rule || typeid(*rule) != typeid(IntervalRule))
			return false;

		const IntervalRule& intervalRule = static_cast<const IntervalRule&>(*rule);

		if (lowerBound.enabled)
		{
			if (!compareThreshold(lowerBound, intervalRule.getLowerBound()))
				return false;
		}

		if (upperBound.enabled)
		{
			if (!compareThreshold(upperBound, intervalRule.getUpperBound()))
				return false;
		}

		return intervalRule.getCategory() == s;
	}
}

CASE(TAG "Interval rules with a single 'less-than' lower bound are parsed correctly.")
{
	// Given
	string_view lowerBoundOnly = "3<";
	string_view lowerBoundOnlyEqual = "3<=";
	string_view lowerBoundOnlyDecimal = "3.4<";
	string_view lowerBoundOnlyDecimalEqual = "3.4<=";

	// When, then
	EXPECT(check(lowerBoundOnly,             { .value = 3.0, .allowEqual = false }, {}));
	EXPECT(check(lowerBoundOnlyEqual,        { .value = 3.0, .allowEqual = true  }, {}));
	EXPECT(check(lowerBoundOnlyDecimal,      { .value = 3.4, .allowEqual = false }, {}));
	EXPECT(check(lowerBoundOnlyDecimalEqual, { .value = 3.4, .allowEqual = true  }, {}));
}

CASE(TAG "Interval rules with a single 'less-than' upper bound are parsed correctly.")
{
	// Given
	string_view upperBoundOnly = "<3";
	string_view upperBoundOnlyEqual = "<=3";
	string_view upperBoundOnlyDecimal = "<3.4";
	string_view upperBoundOnlyDecimalEqual = "<=3.4";

	// When, then
	EXPECT(check(upperBoundOnly,             {}, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(upperBoundOnlyEqual,        {}, { .value = 3.0, .allowEqual = true  }));
	EXPECT(check(upperBoundOnlyDecimal,      {}, { .value = 3.4, .allowEqual = false }));
	EXPECT(check(upperBoundOnlyDecimalEqual, {}, { .value = 3.4, .allowEqual = true  }));
}

CASE(TAG "Interval rules with two 'less-than' bounds are parsed correctly.")
{
	// Given
	string_view both = "1<<3";
	string_view bothEqual1 = "1<=<3";
	string_view bothEqual2 = "1<<=3";
	string_view bothEqual3 = "1<=<=3";
	string_view bothDecimal = "1.2<<3";
	string_view bothDecimalEqual1 = "1<=<3.4";
	string_view bothDecimalEqual2 = "1.2<<=3.4";
	string_view bothDecimalEqual3 = "1.2<=<=3.4";

	// When, then
	EXPECT(check(both,              { .value = 1.0, .allowEqual = false }, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(bothEqual1,        { .value = 1.0, .allowEqual = true  }, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(bothEqual2,        { .value = 1.0, .allowEqual = false }, { .value = 3.0, .allowEqual = true  }));
	EXPECT(check(bothEqual3,        { .value = 1.0, .allowEqual = true  }, { .value = 3.0, .allowEqual = true  }));
	EXPECT(check(bothDecimal,       { .value = 1.2, .allowEqual = false }, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(bothDecimalEqual1, { .value = 1.0, .allowEqual = true  }, { .value = 3.4, .allowEqual = false }));
	EXPECT(check(bothDecimalEqual2, { .value = 1.2, .allowEqual = false }, { .value = 3.4, .allowEqual = true  }));
	EXPECT(check(bothDecimalEqual3, { .value = 1.2, .allowEqual = true  }, { .value = 3.4, .allowEqual = true  }));
}

CASE(TAG "Interval rules with a single 'greater-than' lower bound are parsed correctly.")
{
	// Given
	string_view lowerBoundOnly = ">3";
	string_view lowerBoundOnlyEqual = ">=3";
	string_view lowerBoundOnlyDecimal = ">3.4";
	string_view lowerBoundOnlyDecimalEqual = ">=3.4";

	// When, then
	EXPECT(check(lowerBoundOnly,             { .value = 3.0, .allowEqual = false }, {}));
	EXPECT(check(lowerBoundOnlyEqual,        { .value = 3.0, .allowEqual = true  }, {}));
	EXPECT(check(lowerBoundOnlyDecimal,      { .value = 3.4, .allowEqual = false }, {}));
	EXPECT(check(lowerBoundOnlyDecimalEqual, { .value = 3.4, .allowEqual = true  }, {}));
}

CASE(TAG "Interval rules with a single 'greater-than' upper bound are parsed correctly.")
{
	// Given
	string_view upperBoundOnly = "3>";
	string_view upperBoundOnlyEqual = "3>=";
	string_view upperBoundOnlyDecimal = "3.4>";
	string_view upperBoundOnlyDecimalEqual = "3.4>=";

	// When, then
	EXPECT(check(upperBoundOnly,             {}, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(upperBoundOnlyEqual,        {}, { .value = 3.0, .allowEqual = true  }));
	EXPECT(check(upperBoundOnlyDecimal,      {}, { .value = 3.4, .allowEqual = false }));
	EXPECT(check(upperBoundOnlyDecimalEqual, {}, { .value = 3.4, .allowEqual = true  }));
}

CASE(TAG "Interval rules with two 'greater-than' bounds are parsed correctly.")
{
	// Given
	string_view both = "3>>1";
	string_view bothEqual1 = "3>=>1";
	string_view bothEqual2 = "3>>=1";
	string_view bothEqual3 = "3>=>=1";
	string_view bothDecimal = "3>>1.2";
	string_view bothDecimalEqual1 = "3.4>=>1";
	string_view bothDecimalEqual2 = "3.4>>=1.2";
	string_view bothDecimalEqual3 = "3.4>=>=1.2";

	// When, then
	EXPECT(check(both,              { .value = 1.0, .allowEqual = false }, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(bothEqual1,        { .value = 1.0, .allowEqual = true  }, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(bothEqual2,        { .value = 1.0, .allowEqual = false }, { .value = 3.0, .allowEqual = true  }));
	EXPECT(check(bothEqual3,        { .value = 1.0, .allowEqual = true  }, { .value = 3.0, .allowEqual = true  }));
	EXPECT(check(bothDecimal,       { .value = 1.2, .allowEqual = false }, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(bothDecimalEqual1, { .value = 1.0, .allowEqual = true  }, { .value = 3.4, .allowEqual = false }));
	EXPECT(check(bothDecimalEqual2, { .value = 1.2, .allowEqual = false }, { .value = 3.4, .allowEqual = true  }));
	EXPECT(check(bothDecimalEqual3, { .value = 1.2, .allowEqual = true  }, { .value = 3.4, .allowEqual = true  }));
}

CASE(TAG "Interval rules allow for optional 'X' or 'x' sign.")
{
	// Given
	string_view lowerThanLowerBoundLowercase = "3<x";
	string_view lowerThanLowerBoundUppercase = "3<X";
	string_view lowerThanUpperBoundLowercase = "x<3";
	string_view lowerThanUpperBoundUppercase = "X<3";
	string_view lowerThanMixedBoundLowercase = "3<x<4";
	string_view lowerThanMixedBoundUppercase = "3<X<4";

	string_view greaterThanLowerBoundLowercase = "x>3";
	string_view greaterThanLowerBoundUppercase = "X>3";
	string_view greaterThanUpperBoundLowercase = "3>x";
	string_view greaterThanUpperBoundUppercase = "3>X";
	string_view greaterThanMixedBoundLowercase = "4>x>3";
	string_view greaterThanMixedBoundUppercase = "4>X>3";

	// When, then
	EXPECT(check(lowerThanLowerBoundLowercase, { .value = 3.0, .allowEqual = false }, {}));
	EXPECT(check(lowerThanLowerBoundUppercase, { .value = 3.0, .allowEqual = false }, {}));
	EXPECT(check(lowerThanUpperBoundLowercase, {}, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(lowerThanUpperBoundUppercase, {}, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(lowerThanMixedBoundLowercase, { .value = 3.0, .allowEqual = false }, { .value = 4.0, .allowEqual = false }));
	EXPECT(check(lowerThanMixedBoundUppercase, { .value = 3.0, .allowEqual = false }, { .value = 4.0, .allowEqual = false }));

	EXPECT(check(greaterThanLowerBoundLowercase, { .value = 3.0, .allowEqual = false }, {}));
	EXPECT(check(greaterThanLowerBoundUppercase, { .value = 3.0, .allowEqual = false }, {}));
	EXPECT(check(greaterThanUpperBoundLowercase, {}, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(greaterThanUpperBoundUppercase, {}, { .value = 3.0, .allowEqual = false }));
	EXPECT(check(greaterThanMixedBoundLowercase, { .value = 3.0, .allowEqual = false }, { .value = 4.0, .allowEqual = false }));
	EXPECT(check(greaterThanMixedBoundUppercase, { .value = 3.0, .allowEqual = false }, { .value = 4.0, .allowEqual = false }));
}

CASE(TAG "Interval rules with negative numbers are parsed correctly.")
{
	// Given
	string_view lowerBoundOnly = "-1.2<=";
	string_view upperBoundOnly = "<=-1.2";
	string_view invalidMathStillParsed = "-1.2<=<=-3.4";

	// When, then
	EXPECT(check(lowerBoundOnly, { .value = -1.2, .allowEqual = true }, {}));
	EXPECT(check(upperBoundOnly, {}, { .value = -1.2, .allowEqual = true }));
	EXPECT(check(invalidMathStillParsed, { .value = -1.2, .allowEqual = true }, { .value = -3.4, .allowEqual = true }));
}

CASE(TAG "Numbers can start with zero, but they are not handled as octodecimal.")
{
	// Given
	string_view one = "01<";
	string_view twelve = "012<";

	// When, then
	EXPECT(check(one,    { .value = 1.0 }, {}));
	EXPECT(check(twelve, { .value = 12.0 }, {}));
}

CASE(TAG "Invalid interval rules are matched as literal rules with the full category string.")
{
	// Given
	string_view empty = "";
	string_view noNumber = "<";
	string_view noNumberEqual = "<=";
	string_view numberOnly = "123";
	string_view hex = "0xABC<";
	string_view nan = "NaN<";
	string_view spaceFront = " 1<";
	string_view spaceEnd = "1< ";
	string_view spaceMid = "1 <";
	string_view plus = "+1<";
	string_view abbrevDecimal = ".1<";
	string_view missingDecimal = "1.<";
	string_view multipleDecimal = "1.2.3<";
	string_view wrongEqualSignPlacement1 = "0=<";
	string_view wrongEqualSignPlacement2 = "0=>";
	string_view wrongEqualSignPlacement3 = "=<0";
	string_view wrongEqualSignPlacement4 = "=>0";
	string_view wrongEqualSignPlacement5 = "0=<<=1";
	string_view wrongEqualSignPlacement6 = "0<==<1";
	string_view wrongEqualSignPlacement7 = "0=>>=1";
	string_view wrongEqualSignPlacement8 = "0>==>1";
	string_view invalidDoubleBound1 = "1<>2";
	string_view invalidDoubleBound2 = "1><2";
	string_view exponent = "1.4e9<";

	auto check = [](string_view s)
	{
		auto rule = parseIntervalRule(s);
		return rule && typeid(*rule) == typeid(LiteralRule) && rule->getCategory() == s;
	};

	// When, then
	EXPECT(check(empty));
	EXPECT(check(noNumber));
	EXPECT(check(noNumberEqual));
	EXPECT(check(numberOnly));
	EXPECT(check(hex));
	EXPECT(check(nan));
	EXPECT(check(spaceFront));
	EXPECT(check(spaceEnd));
	EXPECT(check(spaceMid));
	EXPECT(check(plus));
	EXPECT(check(abbrevDecimal));
	EXPECT(check(missingDecimal));
	EXPECT(check(multipleDecimal));
	EXPECT(check(wrongEqualSignPlacement1));
	EXPECT(check(wrongEqualSignPlacement2));
	EXPECT(check(wrongEqualSignPlacement3));
	EXPECT(check(wrongEqualSignPlacement4));
	EXPECT(check(wrongEqualSignPlacement5));
	EXPECT(check(wrongEqualSignPlacement6));
	EXPECT(check(wrongEqualSignPlacement7));
	EXPECT(check(wrongEqualSignPlacement8));
	EXPECT(check(invalidDoubleBound1));
	EXPECT(check(invalidDoubleBound2));
	EXPECT(check(exponent));
}
