#pragma once

#include "Category.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


namespace onest::calc
{
	class CategoryFactory final
	{
	public:
		Category createCategory(const std::string& input);

		std::string findCategoryText(Category category) const;

		auto createHasher() const
		{
			return [this](Category category)
			{
				return std::hash<Category::idtype>()(category.myID);
			};
		}

		size_t getNumberOfCategories() const { return myCategories.size(); }

	private:
		bool myExhausted = false;
		Category::idtype myIdCounter = 0;
		std::unordered_map<std::string, Category> myCategories;
	};
}
