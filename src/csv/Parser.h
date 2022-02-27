#pragma once

#include "Sheet.h"


namespace onest::csv
{
	[[nodiscard]] Sheet parseSheet(const std::string& csvData, char separator, char quoteChar);
}
