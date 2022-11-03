#include "Table.h"

#include "ColumnNameGenerator.h"
#include "common.h"

#include <wx/image.h>

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
	}

	void Table::setFirstRowAsHeader(bool firstRowAsHeader)
	{
		setFirstRowAsHeaderWithoutStatusRefresh(firstRowAsHeader);
		for (int i = 0; i < this->GetNumberCols(); ++i)
			refreshDisplayedColumnStatus(i);
	}

	void Table::setCellHue(int row, int column, unsigned hueIndex, unsigned numberOfHuesInUse)
	{
		if (numberOfHuesInUse != 0)
		{
			const double hue = 1.0 / numberOfHuesInUse * hueIndex;
			auto [r, g, b] = wxImage::HSVtoRGB({ hue, 0.5, 0.9 });

			SetCellBackgroundColour(row, column, { r, g, b });
		}
		else
		{
			resetCellHue(row, column);
		}
	}

	void Table::resetCellHue(int row, int column)
	{
		const wxColor white(255, 255, 255);
		SetCellBackgroundColour(row, column, white);
	}

	void Table::changeColumnEnableStatus(int column)
	{
		if (column < 0 || column >= GetNumberCols())
			return;

		myColumnEnabledStatuses[column].flip();
		refreshDisplayedColumnStatus(column);
	}

	unsigned Table::getNumberOfEnabledColumns() const
	{
		return static_cast<unsigned>(count(myColumnEnabledStatuses, true));
	}

	void Table::setFirstRowAsHeaderWithoutStatusRefresh(bool firstRowAsHeader)
	{
		if (firstRowAsHeader)
		{
			auto batchLock = autoCloseBatchUpdate(this);
			for (int i = 0; i < this->GetNumberCols(); ++i)
			{
				SetColLabelValue(i, GetCellValue(0, i));
				addColumnTickboxCharacters(i);
			}

			DeleteRows();
		}
		else
		{
			auto batchLock = autoCloseBatchUpdate(this);
			InsertRows();
			for (int i = 0; i < this->GetNumberCols(); ++i)
			{
				removeColumnTickboxCharacters(i);
				SetCellValue(0, i, GetColLabelValue(i));
				SetColLabelValue(i, ColumnNameGenerator::generateNameForColumn(i));
				addColumnTickboxCharacters(i);
			}
		}

		myFirstRowIsHeader = firstRowAsHeader;
	}

	void Table::refreshDisplayedColumnStatus(int column)
	{
		assert(column >= 0 && column < GetNumberCols());

		const wxColor black(0, 0, 0);
		const wxColor darkGrey(50, 50, 50);
		const wxColor lightGrey(190, 190, 190);
		const wxColor white(255, 255, 255);

		const bool columnIsEnabled = myColumnEnabledStatuses[column];

		auto batchLock = autoCloseBatchUpdate(this);
		refreshColumnTickboxStatus(column);
		for (int i = 0; i < GetNumberRows(); ++i)
		{
			SetCellTextColour(i, column, columnIsEnabled ? black : lightGrey);
			SetCellBackgroundColour(i, column, columnIsEnabled ? white : darkGrey);
		}
	}

	void Table::addColumnTickboxCharacters(int column)
	{
		SetColLabelValue(column, "_\n" + GetColLabelValue(column));
		refreshColumnTickboxStatus(column);
	}

	void Table::removeColumnTickboxCharacters(int column)
	{
		SetColLabelValue(column, GetColLabelValue(column).substr(2));
	}

	void Table::refreshColumnTickboxStatus(int column)
	{
		static const wchar_t ballotBoxWithCheck = L'\u2611';
		static const wchar_t ballotBox = L'\u2610';

		const bool columnIsEnabled = myColumnEnabledStatuses[column];

		wxString label = GetColLabelValue(column);
		label.SetChar(0, columnIsEnabled ? ballotBoxWithCheck : ballotBox);
		SetColLabelValue(column, label);
	}
}
