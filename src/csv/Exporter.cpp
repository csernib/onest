#include "Exporter.h"

#include <iomanip>


using namespace std;

namespace onest::csv
{
	stringstream exportCSV(const Sheet& sheet, char separator, char quoteChar)
	{
		stringstream ss;
		for (const Row& row : sheet)
		{
			bool first = true;
			for (const Value& value : row)
			{
				if (!first) [[likely]]
				{
					ss << separator;
				}
				else [[unlikely]]
				{
					first = false;
				}

				ss << quoted(value, quoteChar, quoteChar);
			}
			ss << '\n';
		}

		return ss;
	}
}
