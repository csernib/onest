#pragma once

#include <stdexcept>


namespace onest::csv
{
	class ParserException : public std::runtime_error
	{
		using runtime_error::runtime_error;
	};
}
