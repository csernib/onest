#include "Diagram.h"
#include "../Exception.h"

#include <wx/dcbuffer.h>
#include <wx/image.h>

#include <cassert>
#include <cmath>
#include <format>
#include <vector>


namespace
{
	int roundToInt(double value)
	{
		return static_cast<int>(lround(value));
	}
}

namespace onest::gui
{
	Diagram::Diagram(wxWindow* parent) : wxWindow(parent, wxID_ANY)
	{
		Bind(wxEVT_PAINT, &Diagram::render, this);
		Bind(wxEVT_SIZE, [this](wxSizeEvent&) { Refresh(); });

		SetBackgroundStyle(wxBG_STYLE_PAINT);
	}

	void Diagram::plotONEST(calc::ONEST onest)
	{
		myONEST = move(onest);
		Refresh();
	}

	void Diagram::clearDiagram()
	{
		myONEST = calc::ONEST();
		Refresh();
	}

	void Diagram::render(wxPaintEvent& event)
	{
		wxBufferedPaintDC dc(this);
		dc.Clear();

		const auto [topLeft, bottomRight] = calculateTopLeftAndBottomRight(dc);

		if (topLeft.x >= bottomRight.x || topLeft.y >= bottomRight.y)
			return;

		drawBackgroundAndBorders(dc, topLeft, bottomRight);

		if (myONEST.empty() || myONEST[0].empty())
			return;

		const double scaleFactorX = static_cast<double>(bottomRight.x - topLeft.x) / (myONEST[0].size() - 1);
		const double scaleFactorY = static_cast<double>(bottomRight.y - topLeft.y);

		drawVerticalGridLines(dc, topLeft, bottomRight, scaleFactorX);
		drawObserverIndexes(dc, topLeft, bottomRight, scaleFactorX);
		drawOPAAxisText(dc, topLeft, bottomRight);
		drawONESTPlot(dc, topLeft, bottomRight, scaleFactorX, scaleFactorY);
	}

	std::pair<wxPoint, wxPoint> Diagram::calculateTopLeftAndBottomRight(wxBufferedPaintDC& dc) const
	{
		const int borderWidthBasis  = 20;

		// TODO: Instead get a sequence of the texts already (to be used for rendering too),
		//       and calculate max extent based on that.
		const int borderWidthLeft   = borderWidthBasis + dc.GetTextExtent("0.5").x + 2;
		const int borderWidthRight  = borderWidthBasis;
		const int borderWidthTop    = borderWidthBasis;
		const int borderWidthBottom = borderWidthBasis + dc.GetTextExtent("0").y + 2;

		const wxPoint topLeft = { borderWidthLeft, borderWidthTop };
		const wxPoint bottomRight = { GetSize().x - borderWidthRight, GetSize().y / 2 - borderWidthBottom };	// TODO: We should probably do this on a window basis via sizers from the main frame.

		return { topLeft, bottomRight };
	}

	void Diagram::drawBackgroundAndBorders(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight) const
	{
		// We need to add 3 instead of 2 when calculating the height due to a bug.
		// TODO: Recheck this on other platforms. On Windows, adding only 2 causes zero OPAs to overlap with the border.
		dc.DrawRectangle(topLeft.x - 1, topLeft.y - 1, bottomRight.x - topLeft.x + 2, bottomRight.y - topLeft.y + 3);
	}

	void Diagram::drawVerticalGridLines(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const
	{
		dc.SetPen(wxPen(wxColor(200, 200, 200), 1, wxPENSTYLE_SHORT_DASH));
		for (size_t i = 1; i < myONEST[0].size() - 1; ++i)
		{
			const int x = topLeft.x + roundToInt(i * scaleFactorX);
			dc.DrawLine(x, topLeft.y, x, bottomRight.y);
		}
	}

	void Diagram::drawObserverIndexes(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const
	{
		if (myONEST[0].size() >= 2)
		{
			// TODO: Only draw text at every Nth column if it would not fit!
			const int y = bottomRight.y + 2;
			for (size_t i = 0; i < myONEST[0].size(); ++i)
			{
				const int x = topLeft.x + roundToInt(i * scaleFactorX);

				const wxString observerCount = std::to_string(i + 2);
				const wxSize extent = dc.GetTextExtent(observerCount);
				dc.DrawText(observerCount, x - roundToInt(extent.x / 2.0), y);
			}
		}
	}

	void Diagram::drawOPAAxisText(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight) const
	{
		// TODO: Either pass this in here as well or have it calculated for the others too.
		const double scaleFactorY = static_cast<double>(bottomRight.y - topLeft.y);

		auto drawAtInterval = [&](int max, int i)
		{
			const std::string text = std::format("{:.2f}", 1.0 / (max - 1) * (max - i - 1));
			const wxSize extent = dc.GetTextExtent(text);

			const int x = topLeft.x - extent.x - 2;
			const int y = topLeft.y + roundToInt(scaleFactorY / (max - 1) * i - extent.y / 2.0);

			dc.DrawText(text, x, y);
		};

		const int max = 3;
		for (int i = 0; i < max; ++i)
			drawAtInterval(max, i);
	}

	void Diagram::drawONESTPlot(wxBufferedPaintDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX, double scaleFactorY) const
	{
		for (size_t i = 0; i < myONEST.size(); ++i)
		{
			const calc::OPAC& opac = myONEST[i];

			const double hue = 1.0 / myONEST.size() * i;
			const auto [r, g, b] = wxImage::HSVtoRGB({ hue, 1.0, 1.0 });
			dc.SetPen(wxPen(wxColor(r, g, b)));

			if (opac.size() != 1)
			{
				for (size_t j = 0; j + 1 < opac.size(); ++j)
				{
					const int x1 = topLeft.x + roundToInt(j * scaleFactorX);
					const int y1 = topLeft.y + roundToInt(scaleFactorY - opac[j] * scaleFactorY);
					const int x2 = topLeft.x + roundToInt((j + 1) * scaleFactorX);
					const int y2 = topLeft.y + roundToInt(scaleFactorY - opac[j + 1] * scaleFactorY);
					dc.DrawLine(x1, y1, x2, y2);
				}
			}
			else
			{
				const int x1 = topLeft.x;
				const int x2 = bottomRight.x;
				const int y  = topLeft.y + roundToInt(scaleFactorY - opac[0] * scaleFactorY);
				dc.DrawLine(x1, y, x2, y);
			}
		}
	}
}
