#pragma once

#include <string>


namespace onest::gui
{
	class ColumnNameGenerator final
	{
	private:
		ColumnNameGenerator() = delete;

	public:
		static std::string generateNameForColumn(int column)
		{
			constexpr int range = 'Z' - 'A';
			return std::string(column / (range + 1) + 1, static_cast<char>('A' + column % (range + 1)));
		}
	};
}
