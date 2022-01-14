#include "Parser.h"

#include <regex>


using namespace std;

namespace onest::csv
{
	Parser::Parser(const std::string& csvData, char separator, char quoteChar)
	{
		static const regex csvRegex(R"__((?:^|,)(?:"((?:[^"]|"")*)"|([^,]*?))(?=,|$))__");

		for (auto it = sregex_iterator(csvData.begin(), csvData.end(), csvRegex); it != sregex_iterator(); ++it)
		{
			// TODO: Just go and parse it! (need to detect number of columns somehow, maybe put a group around $ in the regex - we don't care if new line is part of a quote!)
		}
	}
}
