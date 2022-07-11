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

	private:
		void setFirstRowAsHeaderWithoutStatusRefresh(bool firstRowAsHeader);
		void refreshDisplayedColumnStatus(int column);

		bool myFirstRowIsHeader = false;
		std::vector<bool> myColumnEnabledStatuses;
	};
}
