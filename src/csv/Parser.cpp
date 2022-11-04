#include "Parser.h"
#include "ParserException.h"

#include <sstream>


using namespace std;

namespace onest::csv
{
	Sheet parseSheet(const std::string& csvData, char separator, char quoteChar) try
	{
		if (csvData.empty())
			return Sheet();

		Row row;
		Sheet sheet;
		stringstream cell;

		auto endCell = [&]
		{
			row.push_back(cell.str());
			cell = stringstream();
		};

		auto endRow = [&]
		{
			endCell();
			sheet.push_back(move(row));
			row = Row();
		};

		bool quoteInProgress = false;

		for (size_t i = 0; i < csvData.size();)
		{
			const char currentChar = csvData[i];
			const char nextChar = csvData[i + 1];

			if (currentChar == quoteChar)
			{
				if (quoteInProgress)
				{
					if (nextChar != quoteChar)
					{
						if (nextChar != separator && i + 1 != csvData.size())
							throw ParserException("Could not load CSV: quoted and unquoted text mixed within the same cell");

						quoteInProgress = false;
						++i;
					}
					else
					{
						cell.put(currentChar);
						i += 2;
					}
				}
				else
				{
					if (cell.rdbuf()->in_avail() > 0)
						throw ParserException("Could not load CSV: quoted and unquoted text mixed within the same cell");

					quoteInProgress = true;
					++i;
				}
			}
			else if (!quoteInProgress && currentChar == '\r' && nextChar == '\n')
			{
				endRow();
				i += 2;
			}
			else if (!quoteInProgress && (currentChar == '\n' || currentChar == '\r'))
			{
				endRow();
				++i;
			}
			else if (!quoteInProgress && currentChar == separator)
			{
				endCell();
				++i;
			}
			else
			{
				cell.put(currentChar);
				++i;
			}
		}

		if (!csvData.ends_with("\r\n") && !csvData.ends_with('\n') && !csvData.ends_with('\r'))
			endRow();

		if (quoteInProgress)
			throw ParserException("Could not load CSV: missing closing quote sign");

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
