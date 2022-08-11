#pragma once

#include "../calc/ONEST.h"

#include <memory>

#include <wx/glcanvas.h>

// Has to be included after wxWidgets headers!
#include <matplot/matplot.h>


namespace onest::gui
{
	class Diagram final : public wxGLCanvas
	{
	public:
		Diagram(wxWindow* parent);

		void plotONEST(const calc::ONEST& onest);

	private:
		void clearDiagram();
		void render(wxPaintEvent& event);

		std::unique_ptr<wxGLContext> pMyContext;
		matplot::figure_handle pMyFigure;
	};
}
