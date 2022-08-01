#pragma once

#include "Rule.h"

#include <memory>
#include <vector>


namespace onest::rule
{
	std::vector<std::shared_ptr<Rule>> parseRuleString(std::string_view ruleString);
}
