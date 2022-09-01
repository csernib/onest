#pragma once

#include "../calc/ONEST.h"

#include <wx/window.h>

#include <utility>


class wxBufferedPaintDC;

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

		std::pair<wxPoint, wxPoint> calculateTopLeftAndBottomRight(wxBufferedPaintDC& dc) const;

		void drawBackgroundAndBorders(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight) const;
		void drawVerticalGridLines(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const;
		void drawObserverIndexes(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const;
		void drawOPAAxisText(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight) const;
		void drawONESTPlot(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX, double scaleFactorY) const;

		calc::ONEST myONEST;
	};
}
