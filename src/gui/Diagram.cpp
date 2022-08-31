#include "Diagram.h"
#include "../Exception.h"

#include <wx/dcbuffer.h>
#include <wx/image.h>

#include <cassert>
#include <cmath>
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

		const int borderWidth = 20;
		const wxPoint topLeft = { borderWidth, borderWidth };
		const wxPoint bottomRight = { GetSize().x - borderWidth, GetSize().y / 2 - borderWidth };	// TODO: We should probably do this on a window basis via sizers from the main frame.

		// We need to add 3 instead of 2 when calculating the height due to a bug.
		// TODO: Recheck this on other platforms. On Windows, adding only 2 causes zero OPAs to overlap with the border.
		dc.DrawRectangle(topLeft.x - 1, topLeft.y - 1, bottomRight.x - topLeft.x + 2, bottomRight.y - topLeft.y + 3);

		if (myONEST.empty() || myONEST[0].empty())
			return;

		const double scaleFactorX = static_cast<double>(bottomRight.x - topLeft.x) / (myONEST[0].size() - 1);
		const double scaleFactorY = static_cast<double>(bottomRight.y - topLeft.y);

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
