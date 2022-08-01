#pragma once

#include <string_view>


namespace onest::rule
{
	class Rule
	{
	public:
		Rule(std::string_view category) : myCategory(category)
		{}
		virtual ~Rule() {}

		virtual bool apply(std::string_view value) const = 0;

		std::string_view getCategory() const { return myCategory; }

	protected:
		const std::string_view myCategory;
	};
}
