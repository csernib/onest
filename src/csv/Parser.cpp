#include "Parser.h"
#include "ParserException.h"


using namespace std;

namespace onest::csv
{
	Parser::Parser(const std::string& csvData, char separator, char quoteChar)
	{
		Value value;
		Row row;
		enum { NO, INPROG, MATCHED } quoting = NO;
		unsigned long lineNumber = 1;

		auto moveInto = []<class Into, class From>(Into& into, From& from)
		{
			From toBeAdded;
			swap(from, toBeAdded);
			into.push_back(move(toBeAdded));
		};

		for (char c : csvData)
		{
			if (quoting == MATCHED && c != quoteChar)
			{
				quoting = NO;
			}

			if (c == '\n' && quoting == NO)
			{
				moveInto(row, value);

				if (row.size() > 1 || !row[0].empty())
					moveInto(mySheet, row);
				else
					row.clear();

				++lineNumber;
			}
			else if (c == quoteChar)
			{
				switch (quoting)
				{
				case NO:
					if (!value.empty())
						throw ParserException("line " + to_string(lineNumber) + ": quote in unquoted string");
					quoting = INPROG;
					break;

				case INPROG:
					quoting = MATCHED;
					break;

				case MATCHED:
					value.push_back(c);
					quoting = INPROG;
					break;
				}
			}
			else if (c == separator && quoting == NO)
			{
				moveInto(row, value);
			}
			else
			{
				// TODO: Text after closing quote should also be disallowed. (see failing test!)
				value.push_back(c);
			}
		}

		if (quoting == INPROG)
			throw ParserException("end of input reached without closing quote");

		row.push_back(move(value));
		if (row.size() > 1 || !row[0].empty())
			mySheet.push_back(move(row));
	}
}
