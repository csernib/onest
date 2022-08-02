#pragma once

#include "Sheet.h"

#include <sstream>


namespace onest::csv
{
	std::stringstream exportCSV(const Sheet& sheet, char separator, char quoteChar);
}
