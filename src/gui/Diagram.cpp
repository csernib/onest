// Has to be included first!
#include <matplot/backend/opengl.h>

#include "Diagram.h"
#include "../Exception.h"

#include <cassert>
#include <vector>


namespace onest::gui
{
	Diagram::Diagram(wxWindow* parent)
		: wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
		, pMyContext(new wxGLContext(this))
	{
		[[maybe_unused]] static int instantiateOnlyOnce = 0;
		assert(instantiateOnlyOnce++ == 0 && "Only one instance of this class shall be created due to the OpenGL context!");

		wxGLCanvas::SetCurrent(*pMyContext);

		if (!gladLoadGL())
			throw Exception("Failed to load OpenGL functions.");

		pMyFigure = matplot::figure<matplot::backend::opengl_embed>(true);

		Bind(wxEVT_PAINT, &Diagram::render, this);

		SetBackgroundStyle(wxBG_STYLE_PAINT);
	}

	void Diagram::plotONEST(const calc::ONEST& onest)
	{
		// There seems to be a crash with 2 observers, as Matplot++ does not handle the case where only
		// a single point is given, so that plotting a line is not defined.
		// TODO: It would be still nice to plot the point in this case ('onest[0].size() == 1' branch).
		if (onest.empty() || onest[0].size() == 1)
		{
			clearDiagram();
			return;
		}

		std::vector<int> observerCounts(onest[0].size());
		std::ranges::generate(observerCounts, [i = 2]() mutable { return i++; });

		// TODO: Labels and title do not work.
		auto onestPlot = pMyFigure->add_subplot(2, 1, 0, true);
		onestPlot->hold(true);
		onestPlot->title("ONEST");
		onestPlot->xlabel("Observer count");
		onestPlot->ylabel("OPA");
		onestPlot->xlim({ 2.0, (double)observerCounts.back() });
		onestPlot->ylim({ 0.0, 1.0 });
		for (const calc::OPAC& opac : onest)
			onestPlot->plot(observerCounts, opac, "-o");

		// TODO: Implement boxplots.
		//       Note: this also does not work. Probably also NYI in the OpenGL backend.
		auto boxplot = pMyFigure->add_subplot(2, 1, 1, true);
		std::vector<std::vector<double>> data(10);
		for (auto& v : data)
			v = matplot::randn(100, 0.0, 1.0);
		boxplot->boxplot(data);

		Refresh();
	}

	void Diagram::clearDiagram()
	{
		pMyFigure->add_subplot(2, 1, 0, true);
		pMyFigure->add_subplot(2, 1, 1, true);
		Refresh();
	}

	void Diagram::render(wxPaintEvent& event)
	{
		glViewport(0, 0, GetSize().x, GetSize().y);
		glClear(GL_COLOR_BUFFER_BIT);
		pMyFigure->draw();
		glFlush();
		SwapBuffers();
	}
}
