#include "Parser.h"
#include "ParserException.h"

#include <regex>


using namespace std;

namespace onest::csv
{
	Parser::Parser(const std::string& csvData, char separator, char quoteChar)
	{
		static const regex csvRegex(R"__((?:"((?:[^"]|"")*)"|([^,"]*?))(?:(,)?(\r\n|\n|\r|$)|,))__", regex_constants::optimize);
		static const regex quoteRegex("\"\"", regex_constants::optimize);

		Row row;
		bool matched = false;
		for (auto it = sregex_iterator(csvData.begin(), csvData.end(), csvRegex, regex_constants::match_not_null); it != sregex_iterator(); ++it)
		{
			const smatch& match = *it;

			if (match.prefix().matched)
				throw ParserException("Invalid CSV");

			string sm1 = regex_replace(match[1].str(), quoteRegex, "\"");
			string sm2 = match[2];
			row.push_back(move(sm1) + move(sm2));

			if (match[3].matched)
				row.push_back("");

			if (match[4].matched)
			{
				mySheet.push_back(row);
				row.clear();
				matched = !match.suffix().matched;
			}
		}

		if (!matched && !csvData.empty())
			throw ParserException("Invalid CSV");
	}
}
