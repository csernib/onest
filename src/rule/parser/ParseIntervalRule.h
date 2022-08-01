#pragma once

#include "../Rule.h"

#include <memory>


namespace onest::rule::parser
{
	std::shared_ptr<Rule> parseIntervalRule(std::string_view category);
}
