#include "Categorizer.h"
#include "Parser.h"


namespace onest::rule
{
	Categorizer::Categorizer(std::string ruleString)
		: myRuleString(move(ruleString))
		, myRules(parseRuleString(myRuleString))
	{}

	Categorizer::Result Categorizer::categorize(const std::string& value) const
	{
		for (const auto& rule : myRules)
		{
			if (rule->apply(value))
				return { rule->getCategory(), true };
		}

		return { "", false };
	}
}
