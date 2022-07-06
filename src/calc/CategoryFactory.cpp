#include "CategoryFactory.h"
#include "../Exception.h"

#include <cassert>
#include <format>
#include <limits>
#include <string>


using namespace std;

namespace onest::calc
{
	Category CategoryFactory::createCategory(const string& input)
	{
		constexpr auto maxId = numeric_limits<Category::idtype>::max();

		auto it = myCategories.find(input);
		if (it != myCategories.end())
			return it->second;

		if (myExhausted)
		{
			string exceptionMessage = format("Category limit exhausted. Please reduce the number of categories to {}.", maxId);
			throw Exception(move(exceptionMessage));
		}

		if (myIdCounter == maxId)
			myExhausted = true;

		return myCategories.emplace(input, Category(myIdCounter++)).first->second;
	}

	string CategoryFactory::findCategoryText(Category category) const
	{
		for (const auto& [text, mappedCategory] : myCategories)
		{
			if (category == mappedCategory)
				return text;
		}

		assert(false && "This codepath should never execute, unless the category is from a different factory.");
		return "<INVALID>";
	}
}
