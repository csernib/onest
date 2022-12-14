#pragma once

#include "../csv/Sheet.h"

#include <vector>

#include <wx/grid.h>


namespace onest::gui
{
	class Table final : public wxGrid
	{
	public:
		Table(wxWindow* parent, const csv::Sheet& sheet);

		void setFirstRowAsHeader(bool firstRowAsHeader);
		bool isFirstRowHeader() const { return myFirstRowIsHeader; }

		void setCellHue(int row, int column, unsigned hueIndex, unsigned numberOfHuesInUse);
		void resetCellHue(int row, int column);

		void changeColumnEnableStatus(int column);
		bool isColumnEnabled(int column) const { return myColumnEnabledStatuses.at((size_t)column); }
		unsigned getNumberOfEnabledColumns() const;

	private:
		void setFirstRowAsHeaderWithoutStatusRefresh(bool firstRowAsHeader);
		void refreshDisplayedColumnStatus(int column);
		void addColumnTickboxCharacters(int column);
		void removeColumnTickboxCharacters(int column);
		void refreshColumnTickboxStatus(int column);

		bool myFirstRowIsHeader = false;
		std::vector<bool> myColumnEnabledStatuses;
	};
}
