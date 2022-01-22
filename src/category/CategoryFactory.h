#pragma once

#include "Category.h"
#include "CategoryMatcher.h"

#include <memory>
#include <string_view>
#include <vector>


namespace onest::category
{
	class CategoryFactory final
	{
	public:
		CategoryFactory(std::vector<std::unique_ptr<CategoryMatcher>> categoryMatchers);

		Category createCategory(std::string_view input) const;

	private:
		std::vector<std::pair<std::unique_ptr<CategoryMatcher>, Category>> myMatchers;
	};
}
