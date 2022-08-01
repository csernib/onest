#include "../src/rule/LiteralRule.h"
#include "test.h"


#define TAG "[Rule] "

using namespace std;

using onest::rule::LiteralRule;

CASE(TAG "Empty literal rule matches empty string.")
{
	// Given
	LiteralRule rule("");

	// When, then
	EXPECT(rule.apply(""));
}

CASE(TAG "Unescaped literal rule matches value.")
{
	// Given
	LiteralRule rule("abc");

	// When, then
	EXPECT(rule.apply("abc"));
}

CASE(TAG "Unescaped literal rule does not match mismatching value.")
{
	// Given
	LiteralRule rule("abc");

	// When, then
	EXPECT_NOT(rule.apply("ab"));
	EXPECT_NOT(rule.apply("bc"));
	EXPECT_NOT(rule.apply("abcd"));
	EXPECT_NOT(rule.apply("abc "));
	EXPECT_NOT(rule.apply(" abc"));
}

CASE(TAG "Escaped literal rule matches value without escaping.")
{
	// Given
	LiteralRule rule("ab\\c");

	// When, then
	EXPECT(rule.apply("abc"));
}

CASE(TAG "Literal rule with an unclosed escape at the end matches value without escaping.")
{
	// Given
	LiteralRule rule("abc\\");

	// When, then
	EXPECT(rule.apply("abc"));
}

CASE(TAG "Literal rule with a single unclosed escape matches empty string.")
{
	// Given
	LiteralRule rule("\\");

	// When, then
	EXPECT(rule.apply(""));
}

CASE(TAG "Escaped literal rule does not match escaped value.")
{
	// Given
	LiteralRule rule("ab\\c");

	// When, then
	EXPECT_NOT(rule.apply("ab\\c"));
}
