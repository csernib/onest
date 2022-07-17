#pragma once

#include "../calc/AssessmentMatrix.h"

#include <string>

#include <wx/frame.h>
#include <wx/stattext.h>


namespace onest::gui
{
	class Table;

	class MainFrame final : public wxFrame
	{
	private:
		static const std::string OPAN_TEXT;

	public:
		MainFrame();

	private:
		void recalculateValues();
		calc::AssessmentMatrix createAssessmentMatrixFromGUI();

		Table* pMyTable;
		wxStaticText* pMyOPANValue;
	};
}
