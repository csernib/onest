#include "MainFrame.h"
#include "Table.h"

#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/sizer.h>

#include "../calc/CategoryFactory.h"
#include "../calc/ONEST.h"
#include "../csv/Parser.h"
#include "../io/File.h"


using namespace onest::calc;
using namespace std;

namespace onest::gui
{
	const string MainFrame::OPAN_TEXT = "OPA(N): ";

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "ONEST")
	{
		csv::Sheet sheet;

		wxFileDialog* fileOpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, wxEmptyString, "CSV files (*.csv)|*.csv|All files|*", wxFD_OPEN, wxDefaultPosition);
		if (fileOpenDialog->ShowModal() == wxID_OK)
		{
			sheet = csv::parseSheet(io::File::readFileAsString(fileOpenDialog->GetPath().ToStdString()), ';', '"');
		}
		fileOpenDialog->Destroy();

		CreateStatusBar();

		wxBoxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

		wxBoxSizer* verticalSizer = new wxBoxSizer(wxVERTICAL);
		horizontalSizer->Add(verticalSizer);

		pMyTable = new Table(this, sheet);
		horizontalSizer->Add(pMyTable);

		wxCheckBox* headerCheckbox = new wxCheckBox(this, -1, "Header");
		verticalSizer->Add(headerCheckbox);
		headerCheckbox->SetValue(pMyTable->isFirstRowHeader());
		headerCheckbox->Bind(wxEVT_CHECKBOX, [this](const wxCommandEvent& e)
		{
			pMyTable->setFirstRowAsHeader(e.IsChecked());
			recalculateValues();
		});

		pMyOPANValue = new wxStaticText(this, -1, OPAN_TEXT + "N/A");
		verticalSizer->Add(pMyOPANValue);

		pMyTable->Bind(wxEVT_GRID_LABEL_LEFT_CLICK, [this](const wxGridEvent& event)
		{
			pMyTable->changeColumnEnableStatus(event.GetCol());
			recalculateValues();
		});

		pMyTable->Bind(wxEVT_GRID_CELL_CHANGED, [this](const wxGridEvent&)
		{
			recalculateValues();
		});

		SetSizer(horizontalSizer);

		recalculateValues();
	}

	void MainFrame::recalculateValues()
	{
		SetStatusText("Calculating ONEST...");
		try
		{
			// TODO: Do it in a different thread!
			const AssessmentMatrix matrix = createAssessmentMatrixFromGUI();
			const ONEST onest = calculateRandomPermutations(matrix, 100);

			pMyOPANValue->SetLabelText(OPAN_TEXT + to_string(calculateOPAN(onest)));
			SetStatusText("Ready");
		}
		catch (const exception& ex)
		{
			pMyOPANValue->SetLabelText(OPAN_TEXT + "N/A");
			SetStatusText("Error: "s + ex.what());
		}
	}

	AssessmentMatrix MainFrame::createAssessmentMatrixFromGUI()
	{
		const int numberOfColumns = pMyTable->GetNumberCols();
		const int numberOfRows = pMyTable->GetNumberRows();

		const unsigned numberOfObservers = pMyTable->getNumberOfEnabledColumns();
		const unsigned numberOfCases = static_cast<unsigned>(numberOfRows);

		CategoryFactory categoryFactory;

		AssessmentMatrix matrix(numberOfObservers, numberOfCases);
		for (int i = 0; i < numberOfRows; ++i)
		{
			for (int j = 0, observerIndex = 0; j < numberOfColumns; ++j)
			{
				if (!pMyTable->isColumnEnabled(j))
					continue;

				const string cellValue = pMyTable->GetCellValue(i, j).ToStdString();
				matrix.set(observerIndex, i, categoryFactory.createCategory(cellValue));
				++observerIndex;
			}
		}

		return matrix;
	}
}
