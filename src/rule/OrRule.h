#pragma once

#include "Rule.h"

#include <memory>
#include <vector>


namespace onest::test
{
	class Test_RuleParserOrRule;
}

namespace onest::rule
{
	class OrRule final : public Rule
	{
	public:
		explicit OrRule(std::string_view category, std::vector<std::shared_ptr<Rule>> rules)
			: Rule(category)
			, myRules(move(rules))
		{}

		bool apply(std::string_view value) const override
		{
			for (const auto& rule : myRules)
			{
				if (rule->apply(value))
					return true;
			}

			return false;
		}

	private:
		friend test::Test_RuleParserOrRule;
		std::vector<std::shared_ptr<Rule>> myRules;
	};
}
