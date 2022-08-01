#include "../src/rule/OrRule.h"
#include "../src/rule/LiteralRule.h"
#include "test.h"


#define TAG "[Rule] "

using namespace std;

using onest::rule::LiteralRule;
using onest::rule::OrRule;

CASE(TAG "An 'OR' rule matches if any of its child rules does.")
{
	// Given
	auto child1 = make_shared<LiteralRule>("abc");
	auto child2 = make_shared<LiteralRule>("def");
	OrRule rule("abc,def,ghi", { child1, child2 });

	// When, then
	EXPECT(rule.apply("abc"));
	EXPECT(rule.apply("def"));
	EXPECT_NOT(rule.apply("ghi"));
}
