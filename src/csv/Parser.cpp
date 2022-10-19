#include "Parser.h"
#include "ParserException.h"

#include <format>
#include <regex>


using namespace std;

namespace onest::csv
{
	Sheet parseSheet(const std::string& csvData, char separator, char quoteChar) try
	{
		// TODO: It is possible to get a stack overflow on Windows (VS 2022) if the input is long enough and a quote
		//       is left open. Fix this!
		const regex csvRegex(
			format(R"__((?:{1}((?:[^{1}]|{1}{1})*){1}|([^{0}{1}]*?))(?:({0})?(\r\n|\n|\r|$)|{0}))__", separator, quoteChar),
			regex_constants::optimize
		);
		const regex quoteRegex(format("{0}{0}", quoteChar), regex_constants::optimize);
		const string quoteString(&quoteChar, &quoteChar + 1);

		Row row;
		Sheet sheet;
		bool matched = false;
		for (auto it = sregex_iterator(csvData.begin(), csvData.end(), csvRegex, regex_constants::match_not_null); it != sregex_iterator(); ++it)
		{
			const smatch& match = *it;

			if (match.prefix().matched)
				throw ParserException("Could not load CSV: invalid syntax");

			string sm1 = regex_replace(match[1].str(), quoteRegex, quoteString);
			string sm2 = match[2];
			row.push_back(move(sm1) + move(sm2));

			if (match[3].matched)
				row.push_back("");

			if (match[4].matched)
			{
				sheet.push_back(row);
				row.clear();
				matched = !match.suffix().matched;
			}
		}

		if (!matched && !csvData.empty())
			throw ParserException("Could not load CSV: invalid syntax");

		return sheet;
	}
	catch (const ParserException&)
	{
		throw;
	}
	catch (const exception& ex)
	{
		throw ParserException("Could not load CSV: "s + ex.what());
	}
	catch (...)
	{
		throw ParserException("Could not load CSV.");
	}
}
