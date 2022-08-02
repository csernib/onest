#pragma once

#include "../calc/AssessmentMatrix.h"
#include "../csv/Parser.h"

#include <string>

#include <wx/checkbox.h>
#include <wx/frame.h>
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

	public:
		MainFrame();

	private:
		void createMainLayoutSizers();
		void createTable(const csv::Sheet& sheet);
		void createLayoutOnTheLeft();

		void showLoadFileDialog();

		void recalculateValues();
		calc::AssessmentMatrix createAssessmentMatrixAndUpdateCellColors();

		wxBoxSizer* pMyMainHorizontalLayout = nullptr;
		wxBoxSizer* pMyLeftVerticalLayout = nullptr;

		Table* pMyTable = nullptr;
		Diagram* pMyDiagram = nullptr;

		wxCheckBox* pMyHeaderCheckbox = nullptr;
		wxStaticText* pMyOPANValue = nullptr;
		wxStaticText* pMyBandwidthValue = nullptr;
		wxTextCtrl* pMyCategorizerInputField = nullptr;
	};
}
