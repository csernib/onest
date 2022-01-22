#pragma once

#include <string>
#include <string_view>


namespace onest::category
{
	class CategoryMatcher
	{
	public:
		virtual ~CategoryMatcher() {}

		virtual bool matchesCategory(std::string_view input) const = 0;
		virtual std::string getMatcherName() const = 0;
	};
}
