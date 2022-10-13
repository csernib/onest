#pragma once

#include "../calc/ONEST.h"

#include <wx/window.h>

#include <utility>


namespace onest::gui
{
	class Diagram final : public wxWindow
	{
	public:
		explicit Diagram(wxWindow* parent);

		void plotONEST(calc::ONEST onest);
		wxBitmap renderToBitmap();

	private:
		void clearDiagram();
		void handlePaintEvent(wxPaintEvent& event);
		void render(wxDC& dc, wxPoint topLeft, wxPoint bottomRight);

		std::pair<wxPoint, wxPoint> calculateTopLeftAndBottomRight(wxDC& dc) const;

		void drawBackgroundAndBorders(wxDC& dc, wxPoint topLeft, wxPoint bottomRight) const;
		void drawVerticalGridLines(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const;
		void drawOPAGridLinesAndText(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorY) const;
		void drawObserverIndexes(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const;
		void drawONESTPlot(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX, double scaleFactorY) const;

		calc::ONEST myONEST;
	};
}
