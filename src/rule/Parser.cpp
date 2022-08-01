#include "Parser.h"

#include "parser/ParseOrRule.h"
#include "parser/Util.h"


using namespace std;

namespace onest::rule
{
	vector<shared_ptr<Rule>> parseRuleString(string_view ruleString)
	{
		vector<string_view> categoryStrings = parser::splitByUnescapedChar(ruleString, ';');
		return parser::stringsToRules(categoryStrings, parser::parseOrRule);
	}
}
