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

	bool rectanglesIntersect(const std::vector<wxRect>& rects, const wxRect& newRect)
	{
		auto rectIntersects = [&](auto& r) { return !r.Intersect(newRect).IsEmpty(); };
		if (std::ranges::find_if(rects, rectIntersects) != rects.end())
			return true;

		return false;
	}
}

namespace onest::gui
{
	Diagram::Diagram(wxWindow* parent, std::string title)
		: wxWindow(parent, wxID_ANY)
		, myTitle(move(title))
	{
		Bind(wxEVT_PAINT, &Diagram::handlePaintEvent, this);
		Bind(wxEVT_SIZE, [this](wxSizeEvent&) { Refresh(); });

		SetBackgroundStyle(wxBG_STYLE_PAINT);
	}

	void Diagram::plotONEST(calc::ONEST onest)
	{
		myONEST = move(onest);
		Refresh();
	}

	wxBitmap Diagram::renderToBitmap()
	{
		const wxSize bitmapSize(800, 600);
		wxBitmap screenshot(bitmapSize);
		wxMemoryDC memDC;

		memDC.SelectObject(screenshot);
		memDC.SetBackground(wxBrush(wxColor(255, 255, 255)));
		memDC.Clear();

		const wxPoint topLeft = { 50, 50 };
		const wxPoint bottomRight = bitmapSize - topLeft;

		render(memDC, topLeft, bottomRight);

		memDC.SelectObject(wxNullBitmap);
		return screenshot;
	}

	void Diagram::clearDiagram()
	{
		myONEST = calc::ONEST();
		Refresh();
	}

	void Diagram::handlePaintEvent(wxPaintEvent& event)
	{
		wxBufferedPaintDC dc(this);
		dc.Clear();
		const auto [topLeft, bottomRight] = calculateTopLeftAndBottomRight(dc);
		render(dc, topLeft, bottomRight);
	}

	void Diagram::render(wxDC& dc, wxPoint topLeft, wxPoint bottomRight)
	{
		if (topLeft.x >= bottomRight.x || topLeft.y >= bottomRight.y)
			return;

		drawBackgroundAndBorders(dc, topLeft, bottomRight);

		if (myONEST.empty() || myONEST[0].empty())
			return;

		const double scaleFactorX = static_cast<double>(bottomRight.x - topLeft.x) / (myONEST[0].size() - 1);
		const double scaleFactorY = static_cast<double>(bottomRight.y - topLeft.y);

		drawVerticalGridLines(dc, topLeft, bottomRight, scaleFactorX);
		drawOPAGridLinesAndText(dc, topLeft, bottomRight, scaleFactorY);
		drawObserverIndexes(dc, topLeft, bottomRight, scaleFactorX);
		drawONESTPlot(dc, topLeft, bottomRight, scaleFactorX, scaleFactorY);
		drawTitle(dc, topLeft, bottomRight, scaleFactorX, scaleFactorY);
	}

	std::pair<wxPoint, wxPoint> Diagram::calculateTopLeftAndBottomRight(wxDC& dc) const
	{
		const int borderWidthBasis  = 20;

		// TODO: Instead get a sequence of the texts already (to be used for rendering too),
		//       and calculate max extent based on that.
		const int borderWidthLeft   = borderWidthBasis + dc.GetTextExtent("0.5").x + 2;
		const int borderWidthRight  = borderWidthBasis;
		const int borderWidthTop    = borderWidthBasis;
		const int borderWidthBottom = borderWidthBasis + dc.GetTextExtent("0").y + dc.GetTextExtent(myTitle).y + 8;

		const wxPoint topLeft = { borderWidthLeft, borderWidthTop };
		const wxPoint bottomRight = { GetSize().x - borderWidthRight, GetSize().y - borderWidthBottom };

		return { topLeft, bottomRight };
	}

	void Diagram::drawBackgroundAndBorders(wxDC& dc, wxPoint topLeft, wxPoint bottomRight) const
	{
		// We need to add 3 instead of 2 when calculating the height due to a bug.
		// TODO: Recheck this on other platforms. On Windows, adding only 2 causes zero OPAs to overlap with the border.
		dc.DrawRectangle(topLeft.x - 1, topLeft.y - 1, bottomRight.x - topLeft.x + 2, bottomRight.y - topLeft.y + 3);
	}

	void Diagram::drawVerticalGridLines(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const
	{
		dc.SetPen(wxPen(wxColor(200, 200, 200), 1, wxPENSTYLE_SHORT_DASH));
		for (size_t i = 1; i < myONEST[0].size() - 1; ++i)
		{
			const int x = topLeft.x + roundToInt(i * scaleFactorX);
			dc.DrawLine(x, topLeft.y, x, bottomRight.y);
		}
	}

	void Diagram::drawOPAGridLinesAndText(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorY) const
	{
		std::vector<wxRect> textPlacements;
		auto draw = [&](calc::number_t opaValue, bool gridLine)
		{
			const int y = topLeft.y + roundToInt(scaleFactorY - opaValue * scaleFactorY);
			if (gridLine)
				dc.DrawLine(topLeft.x, y, bottomRight.x, y);

			const std::string text = std::format("{:.2f}", opaValue);
			const wxSize extent = dc.GetTextExtent(text);
			const wxPoint pos(topLeft.x - extent.x - 2, y - roundToInt(extent.y / 2.0));
			const wxRect placement(pos, extent);
			if (rectanglesIntersect(textPlacements, placement))
				return;

			textPlacements.push_back(placement);
			dc.DrawText(text, pos);
		};

		dc.SetPen(wxPen(wxColor(200, 200, 200), 1, wxPENSTYLE_SHORT_DASH));

		const auto [bandwidthMin, bandwidthMax] = calc::calculateBandwidthMinMax(myONEST);
		const calc::ObserversNeeded observersNeeded = calc::calculateObserversNeeded(myONEST);

		draw(bandwidthMin, true);
		draw(bandwidthMax, true);

		if (observersNeeded.result != calc::ObserversNeeded::DIVERGED)
			draw(observersNeeded.opaValue, true);

		draw(1.0, false);
		draw(0.0, false);
	}

	void Diagram::drawObserverIndexes(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX) const
	{
		if (myONEST[0].size() >= 2)
		{
			std::vector<wxRect> textPlacements;
			const int y = bottomRight.y + 2;
			for (size_t i = 0; i < myONEST[0].size(); ++i)
			{
				const int x = topLeft.x + roundToInt(i * scaleFactorX);

				const wxString observerCount = std::to_string(i + 2);
				const wxSize extent = dc.GetTextExtent(observerCount);
				const wxPoint pos(x - roundToInt(extent.x / 2.0), y);
				const wxRect placement(pos, extent);
				if (rectanglesIntersect(textPlacements, placement))
					continue;

				textPlacements.push_back(placement);
				dc.DrawText(observerCount, pos);
			}
		}
	}

	void Diagram::drawONESTPlot(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX, double scaleFactorY) const
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

	void Diagram::drawTitle(wxDC& dc, wxPoint topLeft, wxPoint bottomRight, double scaleFactorX, double scaleFactorY) const
	{
		const wxSize extent = dc.GetTextExtent(myTitle);
		const int middle = topLeft.x + (bottomRight.x - topLeft.x) / 2;
		const int y = bottomRight.y + dc.GetTextExtent("0").y + 6;
		const int x = middle - extent.x / 2;

		dc.DrawText(myTitle, x, y);
	}
}
