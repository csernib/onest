#pragma once

#include "CategoryMatcher.h"


namespace onest::category
{
	class DiscreteCategoryMatcher final : public CategoryMatcher
	{
	public:
		DiscreteCategoryMatcher(std::string valueToMatch) : myValueToMatch(move(valueToMatch))
		{}

		bool matchesCategory(std::string_view input) const override
		{
			return myValueToMatch == input;
		}

		std::string getMatcherName() const override
		{
			return myValueToMatch;
		}

	private:
		std::string myValueToMatch;
	};
}
