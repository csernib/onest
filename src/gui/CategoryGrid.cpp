#include "CategoryGrid.h"

#include "../calc/AssessmentMatrix.h"
#include "../calc/Category.h"
#include "../calc/CategoryFactory.h"

#include <algorithm>
#include <format>
#include <numeric>
#include <unordered_map>


using namespace onest::calc;
using namespace std;

namespace onest::gui
{
	CategoryGrid::CategoryGrid(wxWindow* parent) : wxGrid(parent, wxID_ANY)
	{
		CreateGrid(3, 1, wxGrid::wxGridSelectNone);
		HideColLabels();
		HideRowLabels();
		EnableEditing(false);
		EnableDragColSize(false);
		EnableDragRowSize(false);
		SetScrollbars(10, 0, 10, 0);    // Disable vertical scrollbar.
		SetMargins(0, wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y));    // Scrollbar would hide last row without extra margin.
		SetDefaultCellBackgroundColour(GetBackgroundColour());
	}

	void CategoryGrid::refreshCategoryDistributionTable(const calc::AssessmentMatrix& matrix, const calc::CategoryFactory& categoryFactory)
	{
		auto categoryHasher = categoryFactory.createHasher();
		unordered_map<Category, unsigned, decltype(categoryHasher)> countByCategories(categoryFactory.getNumberOfCategories(), categoryHasher);

		for (unsigned i = 0; i < matrix.getTotalNumberOfCases(); ++i)
		{
			for (unsigned j = 0; j < matrix.getTotalNumberOfObservers(); ++j)
				++countByCategories[matrix.get(j, i)];
		}

		vector<pair<Category, unsigned>> sortedCounts(countByCategories.begin(), countByCategories.end());
		ranges::sort(sortedCounts, [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

		const unsigned total = accumulate(
			sortedCounts.begin(),
			sortedCounts.end(),
			0u,
			[](unsigned sum, const auto& categoryAndCount) { return sum + categoryAndCount.second; }
		);

		DeleteCols(0, -1, false);
		InsertCols(0, countByCategories.size() + 1, false);

		BeginBatch();

		SetCellValue(0, 0, "Category:");
		SetCellValue(1, 0, "Count:");
		SetCellValue(2, 0, "Percentage:");

		int col = 1;
		for (const auto& [category, count] : sortedCounts)
		{
			const string categoryText = categoryFactory.findCategoryText(category);
			SetCellValue(0, col, categoryText.empty() ? "<empty cell>" : categoryText);
			SetCellValue(1, col, to_string(count));
			SetCellValue(2, col, format("{:.2f}%", (double)count / total * 100.0));
			++col;
		}

		AutoSizeColumns();

		EndBatch();
	}

	void CategoryGrid::clear()
	{
		DeleteCols(1, -1, false);
		InsertCols(1);
		for (int i = 0; i < 3; ++i)
			SetCellValue(i, 1, "N/A");
	}
}
