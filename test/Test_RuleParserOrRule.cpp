#include "../src/rule/parser/ParseOrRule.h"
#include "../src/rule/IntervalRule.h"
#include "../src/rule/LiteralRule.h"
#include "../src/rule/OrRule.h"
#include "test.h"

#include <ranges>
#include <typeindex>


#define TAG "[RuleParser] "

using namespace std;

using onest::rule::parser::parseOrRule;
using onest::rule::IntervalRule;
using onest::rule::LiteralRule;
using onest::rule::OrRule;
using onest::rule::Rule;

namespace onest::test
{
	class Test_RuleParserOrRule
	{
	public:
		static bool checkLiteralRules(string_view s, vector<string_view> expectedStrings)
		{
			auto rule = parseOrRule(s);
			if (!rule || typeid(*rule) != typeid(OrRule) || rule->getCategory() != s)
				return false;

			const OrRule& orRule = static_cast<const OrRule&>(*rule);
			auto subRuleStrings = orRule.myRules | views::transform([](const auto& subRule) { return subRule->getCategory(); });
			auto subRuleTypes = orRule.myRules | views::transform([](const auto& subRule) -> type_index { return typeid(*subRule); });

			return ranges::none_of(orRule.myRules, [](auto& rulePtr) -> bool { return !rulePtr; }) &&
			       ranges::equal(expectedStrings, subRuleStrings) &&
			       ranges::all_of(subRuleTypes, [](type_index t) { return t == typeid(LiteralRule); });
		}

		static bool checkRuleTypes(const shared_ptr<Rule>& rule, vector<type_index> types)
		{
			if (!rule)
				return false;

			const OrRule& orRule = dynamic_cast<const OrRule&>(*rule);

			auto toType = [](auto rule) { return type_index(rule ? typeid(*rule) : typeid(nullptr)); };

			return ranges::equal(
				orRule.myRules | views::transform(toType),
				types
			);
		}
	};
}

CASE(TAG "Comma separated literal values in rule string are parsed as 'OR' rule containing literal rules.")
{
	// Given
	string_view noEscape = "a,b,c";
	string_view singleEscape = "a,b\\,c";
	string_view escapeBeforeComma = "a,b\\,,c";
	string_view startsWithEmpty = ",a";
	string_view endsWithEmpty = "a,";
	string_view twoEmpty = ",";

	auto check = [](string_view s, vector<string_view> v) { return onest::test::Test_RuleParserOrRule::checkLiteralRules(s, move(v)); };

	// When, then
	EXPECT(check(noEscape,          { "a",   "b",      "c" }));
	EXPECT(check(singleEscape,      { "a",   "b\\,c"       }));
	EXPECT(check(escapeBeforeComma, { "a",   "b\\,",   "c" }));
	EXPECT(check(startsWithEmpty,   { "",    "a"           }));
	EXPECT(check(endsWithEmpty,     { "a",   ""            }));
	EXPECT(check(twoEmpty,          { "",    ""            }));
}

CASE(TAG "An 'OR' rule will contain interval rules if a sub-rule can match one.")
{
	// Given
	string_view heterogeneousRuleString = "a,b,3<=<4.3,5";

	// When
	auto rule = parseOrRule(heterogeneousRuleString);

	// Then
	EXPECT(rule);
	EXPECT(typeid(*rule) == typeid(OrRule));
	EXPECT(onest::test::Test_RuleParserOrRule::checkRuleTypes(rule, {
		typeid(LiteralRule),
		typeid(LiteralRule),
		typeid(IntervalRule),
		typeid(LiteralRule)
	}));
}

CASE(TAG "Strings not containing commas are interpreted as literal rules.")
{
	// Given
	string_view empty = "";
	string_view noComma = "a\\,b";

	auto check = [](string_view s)
	{
		auto rule = parseOrRule(s);
		if (!rule || typeid(*rule) != typeid(LiteralRule))
			return false;

		return rule->getCategory() == s;
	};

	// When, then
	EXPECT(check(empty));
	EXPECT(check(noComma));
}
