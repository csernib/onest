#pragma once

#include "Rule.h"


namespace onest::rule
{
	class LiteralRule final : public Rule
	{
	public:
		explicit LiteralRule(std::string_view category) : Rule(category)
		{}

		bool apply(std::string_view value) const override
		{
			size_t j = 0;
			bool alreadyEscaped = false;
			for (size_t i = 0; i < myCategory.size(); ++i)
			{
				if (myCategory[i] == '\\' && !alreadyEscaped)
				{
					alreadyEscaped = true;
					continue;
				}

				if (j >= value.size())
					return false;

				alreadyEscaped = false;

				if (myCategory[i] != value[j])
					return false;

				++j;
			}

			return j == value.size();
		}
	};
}
