#pragma once

#include "Rule.h"

#include <charconv>


using namespace std;

namespace onest::rule
{
	class IntervalRule final : public Rule
	{
	public:
		struct Threshold
		{
			double value;
			bool allowEqual;
			bool enabled;
		};

	public:
		explicit IntervalRule(string_view category, Threshold lowerBound, Threshold upperBound)
			: Rule(category)
			, myLowerBound(lowerBound)
			, myUpperBound(upperBound)
		{}

		bool apply(string_view value) const override
		{
			double convertedValue;
			auto r = from_chars(value.data(), value.data() + value.size(), convertedValue, chars_format::fixed);
			if (r.ptr != value.data() + value.size() || r.ec != errc())
				return false;

			bool result = true;
			if (myLowerBound.enabled)
			{
				if (myLowerBound.allowEqual)
					result = convertedValue >= myLowerBound.value;
				else
					result = convertedValue > myLowerBound.value;
			}

			if (myUpperBound.enabled)
			{
				if (myUpperBound.allowEqual)
					result = result && convertedValue <= myUpperBound.value;
				else
					result = result && convertedValue < myUpperBound.value;
			}

			return result;
		}

		const Threshold& getLowerBound() const noexcept { return myLowerBound; }
		const Threshold& getUpperBound() const noexcept { return myUpperBound; }

	private:
		const Threshold myLowerBound;
		const Threshold myUpperBound;
	};
}
