#include "Categorizer.h"
#include "Parser.h"


namespace onest::rule
{
	Categorizer::Categorizer(std::string ruleString)
	{
		const size_t sizeWithNull = ruleString.size() + 1;
		pMyRuleString.reset(new char[sizeWithNull]);
		memcpy(pMyRuleString.get(), ruleString.c_str(), sizeWithNull);

		myRules = parseRuleString(pMyRuleString.get());
	}

	Categorizer::Result Categorizer::categorize(const std::string& value) const
	{
		for (size_t i = 0; i < myRules.size(); ++i)
		{
			if (myRules[i]->apply(value))
				return { myRules[i]->getCategory(), (unsigned)i, true };
		}

		return { "", 0, false };
	}
}
