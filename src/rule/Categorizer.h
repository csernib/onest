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
			bool success;
		};

	public:
		Categorizer() = default;
		explicit Categorizer(std::string ruleString);

		Result categorize(const std::string& value) const;

	private:
		std::string myRuleString;
		std::vector<std::shared_ptr<Rule>> myRules;
	};
}
