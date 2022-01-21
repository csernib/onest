#pragma once

#include <string>
#include <vector>


namespace onest::csv
{
	typedef std::string Value;
	typedef std::vector<Value> Row;
	typedef std::vector<Row> Sheet;

	[[nodiscard]] Sheet parseSheet(const std::string& csvData, char separator, char quoteChar);
}
