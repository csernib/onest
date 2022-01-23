#include "CategoryFactory.h"
#include "../Exception.h"

#include <limits>
#include <string>


using namespace std;

namespace onest::category
{
	CategoryFactory::CategoryFactory(vector<unique_ptr<CategoryMatcher>> categoryMatchers)
	{
		constexpr auto maxNumberOfMatchers = numeric_limits<unsigned char>::max();
		const size_t actualNumberOfMatchers = categoryMatchers.size();

		if (maxNumberOfMatchers < actualNumberOfMatchers)
		{
			string exceptionMessage =
				"Too many matchers (" +
				to_string(actualNumberOfMatchers) +
				"). Only up to " +
				to_string(maxNumberOfMatchers) +
				" are supported.";

			throw Exception(move(exceptionMessage));
		}

		for (size_t i = 0; i < actualNumberOfMatchers; ++i)
		{
			const unsigned char c = static_cast<unsigned char>(i + 1);
			myMatchers.emplace_back(move(categoryMatchers[i]), Category(c));
		}
	}

	Category CategoryFactory::createCategory(string_view input) const
	{
		for (const auto& [matcher, category] : myMatchers)
		{
			if (matcher->matchesCategory(input))
				return category;
		}

		throw Exception("Value '" + string(input) + "' is not matched by any of the categories.");
	}
}
