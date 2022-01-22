#pragma once

#include <stdexcept>


namespace onest
{
	class Exception : public std::runtime_error
	{
		using runtime_error::runtime_error;
	};
}
