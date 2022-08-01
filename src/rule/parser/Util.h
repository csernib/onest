#pragma once

#include "../Rule.h"

#include <memory>
#include <string_view>
#include <vector>


namespace onest::rule::parser
{
	typedef std::shared_ptr<Rule> (*RuleFunction)(std::string_view);

	bool isUnescaped(std::string_view string, size_t index);

	std::vector<std::string_view> splitByUnescapedChar(std::string_view string, char c);

	std::vector<std::shared_ptr<Rule>> stringsToRules(std::vector<std::string_view> ruleStrings, RuleFunction ruleFunction);
}
