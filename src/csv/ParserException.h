#pragma once

#include "../Exception.h"


namespace onest::csv
{
	class ParserException : public Exception
	{
		using Exception::Exception;
	};
}
