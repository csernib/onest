#pragma once

#include "../calc/ONEST.h"

#include <wx/window.h>


namespace onest::gui
{
	class Diagram final : public wxWindow
	{
	public:
		explicit Diagram(wxWindow* parent);

		void plotONEST(calc::ONEST onest);

	private:
		void clearDiagram();
		void render(wxPaintEvent& event);

		calc::ONEST myONEST;
	};
}
