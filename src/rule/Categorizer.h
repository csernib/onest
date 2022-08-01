#pragma once

#include "Rule.h"

#include <memory>
#include <string>
#include <vector>


namespace onest::rule
{
	class Categorizer final
	{
	public:
		struct Result
		{
			std::string_view category;
			unsigned matchedRuleIndex;
			bool success;
		};

	public:
		Categorizer() = default;
		explicit Categorizer(std::string ruleString);

		Result categorize(const std::string& value) const;

		unsigned getNumberOfRules() const { return (unsigned)myRules.size(); }

	private:
		std::shared_ptr<char[]> pMyRuleString;
		std::vector<std::shared_ptr<Rule>> myRules;
	};
}
