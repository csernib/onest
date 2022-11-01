#pragma once

#include <wx/grid.h>


namespace onest::calc
{
	class AssessmentMatrix;
	class CategoryFactory;
}

namespace onest::gui
{
	class CategoryGrid final : public wxGrid
	{
	public:
		CategoryGrid(wxWindow* parent);

		void refreshCategoryDistributionTable(const calc::AssessmentMatrix& matrix, const calc::CategoryFactory& categoryFactory);
		void clear();
	};
}
