#pragma once

#include "../csv/Sheet.h"

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
		bool myFirstRowIsHeader = false;
	};
}
