#include "Table.h"

#include <cassert>


namespace onest::gui
{
	Table::Table(wxWindow* parent, const csv::Sheet& sheet) : wxGrid(parent, -1)
	{
		if (sheet.empty() || sheet[0].empty())
			return;

		CreateGrid(sheet.size(), sheet[0].size());
		for (size_t i = 0; i < sheet.size(); ++i)
		{
			assert(sheet[0].size() == sheet[i].size());
			for (size_t j = 0; j < sheet[i].size(); ++j)
			{
				SetCellValue(i, j, sheet[i][j]);
			}
		}
	}
}
