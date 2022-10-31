#pragma once

#include "../calc/AssessmentMatrix.h"
#include "../calc/CategoryFactory.h"
#include "../calc/ONEST.h"
#include "../csv/Sheet.h"

#include <string>

#include <wx/checkbox.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>


namespace onest::gui
{
	class Table;
	class Diagram;

	class MainFrame final : public wxFrame
	{
	private:
		static const std::string OPAN_TEXT;
		static const std::string BANDWIDTH_TEXT;
		static const std::string OBSERVERS_NEEDED_TEXT;
		static const std::string DIAGRAM_TITLE_TEXT;
		static const std::string SIMPLIFIED_DIAGRAM_TITLE_TEXT;
		static const std::string UNDEFINED_VALUE_TEXT;

	public:
		MainFrame();

	private:
		void createToolbar();
		void createMainLayoutSizers();
		void createTable(const csv::Sheet& sheet);
		void createLayoutOnTheLeft();

		void showLoadFileDialog();
		void showSaveFileDialog();
		void showPlotSaveDialog();

		void recalculateValues();
		void refreshCategoryDistributionTable(const calc::AssessmentMatrix& matrix, const calc::CategoryFactory& categoryFactory);
		calc::AssessmentMatrix createAssessmentMatrixAndUpdateCellColors(calc::CategoryFactory& categoryFactory);

		calc::ONEST myONEST;

		wxBoxSizer* pMyMainHorizontalLayout = nullptr;
		wxBoxSizer* pMyLeftVerticalLayout = nullptr;

		wxGrid* pMyCategoryGrid = nullptr;

		Table* pMyTable = nullptr;
		Diagram* pMyDiagram = nullptr;
		Diagram* pMySimplifiedDiagram = nullptr;

		wxStaticText* pMyOPANValue = nullptr;
		wxStaticText* pMyBandwidthValue = nullptr;
		wxStaticText* pMyObserversNeededValue = nullptr;
		wxStaticText* pMyCategorizerLabel = nullptr;
		wxTextCtrl* pMyCategorizerInputField = nullptr;
	};
}
