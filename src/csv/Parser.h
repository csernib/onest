#pragma once

#include <string>
#include <vector>


namespace onest::csv
{
	class Parser final
	{
	public:
		typedef std::string Value;
		typedef std::vector<Value> Row;
		typedef std::vector<Row> Sheet;

	public:
		Parser() = default;
		Parser(const std::string& csvData, char separator = ';', char quoteChar = '"');

		const Sheet& getSheet() const
		{
			return mySheet;
		}

	private:
		Sheet mySheet;
	};
}
