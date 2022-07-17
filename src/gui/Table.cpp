#include "Table.h"

#include <algorithm>
#include <cassert>


using std::ranges::count;

namespace onest::gui
{
	Table::Table(wxWindow* parent, const csv::Sheet& sheet) : wxGrid(parent, -1)
	{
		if (sheet.empty() || sheet[0].empty())
			return;

		myColumnEnabledStatuses.resize(sheet[0].size(), true);

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
				setFirstRowAsHeaderWithoutStatusRefresh(true);
				break;
			}
		}

		Bind(wxEVT_GRID_LABEL_LEFT_CLICK, [this](const wxGridEvent& event)
		{
			const int columnClicked = event.GetCol();
			if (columnClicked < 0)
				return;

			myColumnEnabledStatuses[columnClicked].flip();
			refreshDisplayedColumnStatus(columnClicked);

			// TODO: Recalculate values displayed on main frame!
		});
	}

	void Table::setFirstRowAsHeader(bool firstRowAsHeader)
	{
		setFirstRowAsHeaderWithoutStatusRefresh(firstRowAsHeader);
		for (int i = 0; i < this->GetNumberCols(); ++i)
			refreshDisplayedColumnStatus(i);
	}

	unsigned Table::getNumberOfEnabledColumns() const
	{
		return static_cast<unsigned>(count(myColumnEnabledStatuses, true));
	}

	void Table::setFirstRowAsHeaderWithoutStatusRefresh(bool firstRowAsHeader)
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

	void Table::refreshDisplayedColumnStatus(int column)
	{
		assert(column >= 0 && column < GetNumberCols());

		const wxColor red(255, 0, 0);
		const wxColor white(255, 255, 255);

		const bool columnIsEnabled = myColumnEnabledStatuses[column];

		BeginBatch();
		for (int i = 0; i < GetNumberRows(); ++i)
		{
			SetCellBackgroundColour(i, column, columnIsEnabled ? white : red);
		}
		EndBatch();
	}
}
