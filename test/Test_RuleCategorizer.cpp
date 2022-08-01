#include "../src/rule/Categorizer.h"
#include "test.h"


#define TAG "[Rule] "

using namespace std;

using onest::rule::Categorizer;

CASE(TAG "A categorizer can create the correct categories.")
{
	// Given
	Categorizer categorizer("abc,def;ghi,0<=;<1;<1");

	auto check = [&](const char* inputValue, string_view expectedCategory, unsigned expectedIndex)
	{
		auto result = categorizer.categorize(inputValue);
		return result.success && result.category == expectedCategory && result.matchedRuleIndex == expectedIndex;
	};

	// When, then
	EXPECT(check("abc", "abc,def", 0));
	EXPECT(check("def", "abc,def", 0));
	EXPECT(check("ghi", "ghi,0<=", 1));
	EXPECT(check("0",   "ghi,0<=", 1));
	EXPECT(check("0.5", "ghi,0<=", 1));
	EXPECT(check("1.0", "ghi,0<=", 1));
	EXPECT(check("-2",  "<1",      2));
}

CASE(TAG "A categorizer is able to handle empty input correctly.")
{
	// Given
	Categorizer isEmpty("");
	Categorizer hasEmptyOrClause("abc,;def");
	Categorizer hasEmptyLiteralCategoryFront(";abc");
	Categorizer hasEmptyLiteralCategoryEnd("abc;");
	Categorizer hasEmptyLiteralCategoryMid("abc;;def");
	Categorizer doesNotAllowEmptyInput("abc");

	// When, then
	EXPECT(isEmpty.categorize("").success);
	EXPECT(hasEmptyOrClause.categorize("").success);
	EXPECT(hasEmptyOrClause.categorize("").category == "abc,");
	EXPECT(hasEmptyLiteralCategoryFront.categorize("").success);
	EXPECT(hasEmptyLiteralCategoryEnd.categorize("").success);
	EXPECT(hasEmptyLiteralCategoryMid.categorize("").success);
	EXPECT_NOT(doesNotAllowEmptyInput.categorize("").success);
}

CASE(TAG "A categorizer does not match input not specified by the rule string.")
{
	// Given
	Categorizer categorizer("abc,def;ghi");

	// When, then
	EXPECT_NOT(categorizer.categorize("a").success);
	EXPECT_NOT(categorizer.categorize("abcdef").success);
	EXPECT_NOT(categorizer.categorize("abc,def").success);
	EXPECT_NOT(categorizer.categorize("def;ghi").success);
	EXPECT_NOT(categorizer.categorize("1234").success);
}

CASE(TAG "A default constructed categorizer does not match any string and returns empty result object.")
{
	// Given
	Categorizer categorizer;

	auto check = [&](const char* input)
	{
		auto result = categorizer.categorize(input);
		return !result.success && result.category == "" && result.matchedRuleIndex == 0;
	};

	// When, then
	EXPECT(check(""));
	EXPECT(check(";"));
	EXPECT(check("abc"));
}
