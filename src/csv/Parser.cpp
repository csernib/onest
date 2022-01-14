#include "Parser.h"

#include <regex>
#include <algorithm>


using namespace std;

namespace onest::csv
{
	Parser::Parser(const std::string& csvData, char separator, char quoteChar)
	{
		static const regex csvRegex(R"__((?:^|,)(?:"((?:[^"]|"")*)"|([^,]*?))(?=,|$))__", regex_constants::optimize);
		static const regex quoteRegex("\"\"");

		Row row;
		for (auto it = sregex_iterator(csvData.begin(), csvData.end(), csvRegex); it != sregex_iterator(); ++it)
		{
			// TODO: Just go and parse it! (need to detect number of columns somehow, maybe put a group around $ in the regex - we don't care if new line is part of a quote!)
			const smatch& match = *it;
			string sm1 = regex_replace(match[1].str(), quoteRegex, "\"");
			string sm2 = match[2];
			row.push_back(sm1 + sm2);
		}

		mySheet.push_back(move(row));
	}
}
