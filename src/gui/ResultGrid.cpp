#include "ResultGrid.h"

#include <string>


using namespace onest::calc;
using namespace std;


namespace
{
	const int OPAN_ROW_INDEX = 0;
	const int BANDWIDTH_ROW_INDEX = 1;
	const int OBSERVERS_NEEDED_ROW_INDEX = 2;

	const string OPAN_TEXT = "OPA(N): ";
	const string BANDWIDTH_TEXT = "Bandwidth: ";
	const string OBSERVERS_NEEDED_TEXT = "Observers needed: ";
	const string UNDEFINED_VALUE_TEXT = "N/A";
}

namespace onest::gui
{
	ResultGrid::ResultGrid(wxWindow* parent) : wxGrid(parent, wxID_ANY)
	{
		CreateGrid(3, 2, wxGrid::wxGridSelectNone);
		HideColLabels();
		HideRowLabels();
		EnableEditing(false);
		EnableDragColSize(false);
		EnableDragRowSize(false);
		SetScrollbars(0, 0, 0, 0);    // Disable scrollbars.
		SetDefaultCellBackgroundColour(GetBackgroundColour());

		SetCellValue(OPAN_ROW_INDEX, 0, OPAN_TEXT);
		SetCellValue(BANDWIDTH_ROW_INDEX, 0, BANDWIDTH_TEXT);
		SetCellValue(OBSERVERS_NEEDED_ROW_INDEX, 0, OBSERVERS_NEEDED_TEXT);

		for (int i = 0; i < 3; ++i)
			SetCellValue(i, 1, UNDEFINED_VALUE_TEXT);

		AutoSizeColumns();
	}

	void ResultGrid::updateResults(const ONEST& onest)
	{
		SetCellValue(OPAN_ROW_INDEX, 1, to_string(calculateOPAN(onest)));
		SetCellValue(BANDWIDTH_ROW_INDEX, 1, to_string(calculateBandwidth(onest)));

		const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);
		wstring observersNeededText;
		switch (observersNeeded.result)
		{
		case ObserversNeeded::CONVERGED_AND_DEFINED:
			observersNeededText = to_wstring(observersNeeded.numOfObservers);
			break;

		case ObserversNeeded::CONVERGED_BUT_UNKNOWN:
			observersNeededText = L"?";
			break;

		case ObserversNeeded::DIVERGED:
			observersNeededText = L"\u221E";    // infinity sign
			break;
		}
		SetCellValue(OBSERVERS_NEEDED_ROW_INDEX, 1, observersNeededText);

		AutoSizeColumns();
	}

	void ResultGrid::clear()
	{
		for (int i = 0; i < 3; ++i)
			SetCellValue(i, 1, UNDEFINED_VALUE_TEXT);
		AutoSizeColumns();
	}
}
