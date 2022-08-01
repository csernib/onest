#pragma once

#include "ParseIntervalRule.h"
#include "Util.h"

#include "../OrRule.h"

#include <memory>
#include <vector>


namespace onest::rule::parser
{
	inline std::shared_ptr<Rule> parseOrRule(std::string_view category)
	{
		std::vector<std::string_view> subRuleStrings = splitByUnescapedChar(category, ',');

		if (subRuleStrings.size() <= 1)
			return parseIntervalRule(category);

		return std::make_shared<OrRule>(category, stringsToRules(subRuleStrings, parseIntervalRule));
	}
}
