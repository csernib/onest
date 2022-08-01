#include "../src/rule/IntervalRule.h"
#include "test.h"


#define TAG "[Rule] "

using namespace std;

using onest::rule::IntervalRule;

CASE(TAG "Interval rule with lower-bound only works correctly.")
{
	// Given
	IntervalRule rule1("", { .value = 1.0, .allowEqual = false, .enabled = true }, {});
	IntervalRule rule2("", { .value = 1.0, .allowEqual = true,  .enabled = true }, {});

	// When, then
	EXPECT(rule1.apply("1.3"));
	EXPECT(rule2.apply("1.3"));

	EXPECT_NOT(rule1.apply("1"));
	EXPECT(rule2.apply("1"));

	EXPECT_NOT(rule1.apply("-2.3"));
	EXPECT_NOT(rule2.apply("-2.3"));
}

CASE(TAG "Interval rule with upper-bound only works correctly.")
{
	// Given
	IntervalRule rule1("", {}, { .value = 1.0, .allowEqual = false, .enabled = true });
	IntervalRule rule2("", {}, { .value = 1.0, .allowEqual = true,  .enabled = true });

	// When, then
	EXPECT(rule1.apply("0.3"));
	EXPECT(rule2.apply("0.3"));

	EXPECT_NOT(rule1.apply("1"));
	EXPECT(rule2.apply("1"));

	EXPECT(rule1.apply("-2.3"));
	EXPECT(rule2.apply("-2.3"));
}

CASE(TAG "Interval rule with both bounds works correctly.")
{
	// Given
	IntervalRule rule(
		"",
		{ .value = -1.0, .allowEqual = false, .enabled = true },
		{ .value = 1.0,  .allowEqual = true,  .enabled = true }
	);

	// When, then
	EXPECT(rule.apply("0"));
	EXPECT(rule.apply("1.0"));
	EXPECT_NOT(rule.apply("-1.0"));
	EXPECT_NOT(rule.apply("-1.3"));
	EXPECT_NOT(rule.apply("1.001"));
}

CASE(TAG "Interval rules ignore disabled thresholds.")
{
	// Given
	IntervalRule rule("", { .value = 0.0 }, { .value = 3.0, .enabled = true });

	// When, then
	EXPECT(rule.apply("2.3"));
	EXPECT(rule.apply("-2.3"));
	EXPECT_NOT(rule.apply("4"));
}

CASE(TAG "Interval rules with both threshold disabled return true if the number conversion succeeds.")
{
	// Given
	IntervalRule rule("", {}, {});

	// When, then
	EXPECT(rule.apply("1234"));
	EXPECT(rule.apply("-1234"));
	EXPECT_NOT(rule.apply("abc"));
}

CASE(TAG "Interval rules do not match non-numeric strings.")
{
	// Given
	IntervalRule rule("", { .value = -100.0, .enabled = true }, { .value = 100.0, .enabled = true });

	// When, then
	EXPECT_NOT(rule.apply(""));
	EXPECT_NOT(rule.apply("abc"));
	EXPECT_NOT(rule.apply("0x0A"));
	EXPECT_NOT(rule.apply("NaN"));
	EXPECT_NOT(rule.apply("1.4e9"));
}
