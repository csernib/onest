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

		for (int i = 0; i < this->GetNumberCols(); ++i)
		{
			double d;
			if (!GetCellValue(0, i).ToDouble(&d))
			{
				setFirstRowAsHeader(true);
				break;
			}
		}
	}

	void Table::setFirstRowAsHeader(bool firstRowAsHeader)
	{
		if (firstRowAsHeader)
		{
			BeginBatch();
			for (int i = 0; i < this->GetNumberCols(); ++i)
				SetColLabelValue(i, GetCellValue(0, i));

			// TODO: This is problematic. If there was a header originally, then we will get a scrollbar when the row is readded.
			//       Otherwise an empty "dummy" row is left there.
			DeleteRows();
			EndBatch();
		}
		else
		{
			BeginBatch();
			InsertRows();
			for (int i = 0; i < this->GetNumberCols(); ++i)
			{
				SetCellValue(0, i, GetColLabelValue(i));

				// TODO: What if we are out of range (more than Z number of columns)?
				SetColLabelValue(i, wxString(static_cast<char>('A' + i)));
			}
			EndBatch();
		}

		myFirstRowIsHeader = firstRowAsHeader;
	}
}
