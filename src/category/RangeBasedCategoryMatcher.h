#pragma once

#include "CategoryMatcher.h"

#include <stdexcept>
#include <type_traits>


namespace onest::category
{
	template<class T>
	class RangeBasedCategoryMatcher final : public CategoryMatcher
	{
	public:
		typedef T number_t;

	public:
		RangeBasedCategoryMatcher(number_t minValue, number_t maxValue)
			: myMinValue(minValue)
			, myMaxValue(maxValue)
		{}

		bool matchesCategory(std::string_view input) const override
		{
			try
			{
				const number_t n = convert(input);
				return myMinValue <= n && myMaxValue >= n;
			}
			catch (const std::invalid_argument&)
			{
				return false;
			}
			catch (const std::out_of_range&)
			{
				return false;
			}
		}

		std::string getMatcherName() const override
		{
			return "[" + std::to_string(myMinValue) + ", " + std::to_string(myMaxValue) + "]";
		}

	private:
		static number_t convert(std::string_view input)
		{
			if constexpr (std::is_same_v<number_t, float>)
				return std::stof(std::string(input));
			else
				static_assert(!std::is_same_v<number_t, number_t>, "Type conversion not supported!");
		}

		number_t myMinValue;
		number_t myMaxValue;
	};
}
