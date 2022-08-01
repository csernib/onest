#pragma once

#include "../calc/AssessmentMatrix.h"

#include <string>

#include <wx/frame.h>
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
		void recalculateValues();
		calc::AssessmentMatrix createAssessmentMatrixFromGUI();

		Table* pMyTable;
		Diagram* pMyDiagram;
		wxStaticText* pMyOPANValue;
		wxStaticText* pMyBandwidthValue;
		wxTextCtrl* pMyCategorizerInputField;
	};
}
