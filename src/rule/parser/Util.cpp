#include "Util.h"

#include <cassert>


using namespace std;

namespace onest::rule::parser
{
	bool isUnescaped(string_view string, size_t index)
	{
		assert(index >= 0 && index < string.size());

		int counter = 0;

		// Condition is based on unsigned integer wrap-around.
		for (size_t i = index - 1; i < string.size(); --i)
		{
			if (string[i] != '\\')
				return counter % 2 == 0;

			++counter;
		}

		return counter % 2 == 0;
	}

	vector<string_view> splitByUnescapedChar(string_view string, char c)
	{
		vector<string_view> splits;
		size_t prev = 0;
		for (size_t i = 0; i < string.size(); ++i)
		{
			if (string[i] == c && isUnescaped(string, i))
			{
				splits.emplace_back(string.begin() + prev, string.begin() + i);
				prev = i + 1;
			}
		}

		splits.emplace_back(string.begin() + prev, string.end());

		return splits;
	}

	vector<shared_ptr<Rule>> stringsToRules(const vector<string_view>& ruleStrings, RuleFunction ruleFunction)
	{
		vector<shared_ptr<Rule>> rules;
		rules.reserve(ruleStrings.size());
		for (string_view ruleString : ruleStrings)
			rules.push_back(ruleFunction(ruleString));

		return rules;
	}
}
