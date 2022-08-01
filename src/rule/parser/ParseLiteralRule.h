#pragma once

#include "../LiteralRule.h"

#include <memory>


namespace onest::rule::parser
{
	inline std::shared_ptr<Rule> parseLiteralRule(std::string_view category)
	{
		return std::make_shared<LiteralRule>(category);
	}
}
