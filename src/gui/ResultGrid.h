#pragma once

#include "../calc/ONEST.h"

#include <wx/grid.h>


namespace onest::gui
{
	class ResultGrid final : public wxGrid
	{
	public:
		ResultGrid(wxWindow* parent);

		void updateResults(const calc::ONEST& onest);
		void clear();
	};
}
