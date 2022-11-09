#pragma once

#include "../calc/AssessmentMatrix.h"
#include "../calc/CategoryFactory.h"
#include "../calc/ONEST.h"
#include "../csv/Sheet.h"

#include <thread>

#include <wx/checkbox.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>


namespace onest::gui
{
	class CategoryGrid;
	class Diagram;
	class ResultGrid;
	class Table;

	class MainFrame final : public wxFrame
	{
	public:
		MainFrame();

	private:
		void DoGiveHelp(const wxString& help, bool show) override;

		void createToolbar();
		void createMainLayoutSizers();
		void createTable(const csv::Sheet& sheet);
		void createLayoutOnTheLeft();

		void showLoadFileDialog();
		void showSaveFileDialog();

		void setCalculationModeAndToolBarState(bool calculateAll);
		bool haveUserAcceptWarningForAllPermutationsIfNeeded();

		void recalculateValues();
		void handleCalculationSuccess(const wxThreadEvent& event);
		void handleCalculationFailure(const wxThreadEvent& event);
		void handleError(const std::string& errorMessage);
		calc::AssessmentMatrix createAssessmentMatrixAndUpdateCellColors(calc::CategoryFactory& categoryFactory);

		calc::ONEST myONEST;
		bool myCalculateAllPossiblePermutations = false;
		bool myUserAlreadyWarnedForAllPermutations = false;

		wxBoxSizer* pMyMainHorizontalLayout = nullptr;
		wxBoxSizer* pMyLeftVerticalLayout = nullptr;

		ResultGrid* pMyResultGrid = nullptr;
		CategoryGrid* pMyCategoryGrid = nullptr;

		Table* pMyTable = nullptr;
		Diagram* pMyDiagram = nullptr;
		Diagram* pMySimplifiedDiagram = nullptr;

		wxStaticText* pMyCategorizerLabel = nullptr;
		wxTextCtrl* pMyCategorizerInputField = nullptr;

		// Leave it as the last member variable to ensure that
		// the thread joins before the frame is destructed.
		std::jthread myCalculationThread;
	};
}
