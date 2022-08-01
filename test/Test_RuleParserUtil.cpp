#include "../src/rule/parser/Util.h"
#include "test.h"


#define TAG "[RuleParser] "

using namespace std;

using onest::rule::parser::isUnescaped;
using onest::rule::parser::splitByUnescapedChar;

CASE(TAG "The 'isUnescaped' function gives the correct result.")
{
	// Given
	string_view noEscape = "abc";
	string_view singleEscape = "ab\\c";
	string_view doubleEscape = "ab\\\\c";
	string_view tripleEscape = "ab\\\\\\c";
	string_view leadingEscape = "\\c";
	string_view leadingDoubleEscape = "\\\\c";
	string_view differentEscape = "a\\bc";

	auto check = [](string_view s) { return isUnescaped(s, s.find('c')); };

	// When, then
	EXPECT(check(noEscape));
	EXPECT_NOT(check(singleEscape));
	EXPECT(check(doubleEscape));
	EXPECT_NOT(check(tripleEscape));
	EXPECT_NOT(check(leadingEscape));
	EXPECT(check(leadingDoubleEscape));
	EXPECT(check(differentEscape));
}

CASE(TAG "Splitting by unescaped character works correctly.")
{
	// Given
	string_view simple = "abc,def,gh";
	string_view empty = "";
	string_view noComma = "abc";
	string_view endingWithEmpty = "abc,def,";
	string_view startingWithEmpty = ",abc,def";
	string_view containingEmpty = "abc,,def";
	string_view multipleEmpty = ",,abc,,,def,gh,";
	string_view escapedComma = "ab\\,c,def,gh";
	string_view doubleEscape = "ab\\\\,c,def,gh";

	auto compare = [](string_view s, initializer_list<string_view> values)
	{
		return splitByUnescapedChar(s, ',') == vector(values);
	};

	// When, then
	EXPECT(compare(simple, { "abc", "def", "gh" }));
	EXPECT(compare(empty, { "" }));
	EXPECT(compare(noComma, { "abc" }));
	EXPECT(compare(endingWithEmpty, { "abc", "def", "" }));
	EXPECT(compare(startingWithEmpty, { "", "abc", "def" }));
	EXPECT(compare(containingEmpty, { "abc", "", "def" }));
	EXPECT(compare(multipleEmpty, { "", "", "abc", "", "", "def", "gh", "" }));
	EXPECT(compare(escapedComma, { "ab\\,c", "def", "gh" }));
	EXPECT(compare(doubleEscape, { "ab\\\\", "c", "def", "gh" }));
}
