#include "MainFrame.h"
#include "Diagram.h"
#include "Table.h"

#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/sizer.h>

#include "../calc/CategoryFactory.h"
#include "../calc/ONEST.h"
#include "../csv/Parser.h"
#include "../io/File.h"
#include "../rule/Categorizer.h"
#include "../git.h"


using namespace onest::calc;
using namespace onest::rule;
using namespace std;

namespace onest::gui
{
	const string MainFrame::OPAN_TEXT = "OPA(N): ";
	const string MainFrame::BANDWIDTH_TEXT = "Bandwidth: ";

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "ONEST")
	{
		csv::Sheet sheet;

		wxFileDialog* fileOpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, wxEmptyString, "CSV files (*.csv)|*.csv|All files|*", wxFD_OPEN, wxDefaultPosition);
		if (fileOpenDialog->ShowModal() == wxID_OK)
		{
			sheet = csv::parseSheet(io::File::readFileAsString(fileOpenDialog->GetPath().ToStdString()), ';', '"');
		}
		fileOpenDialog->Destroy();

		SetTitle("ONEST Pre-alpha    |  " + git::getVersionInfo());

		CreateStatusBar();

		wxBoxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

		wxBoxSizer* verticalSizer = new wxBoxSizer(wxVERTICAL);
		horizontalSizer->Add(verticalSizer, wxSizerFlags(1).Expand());

		pMyTable = new Table(this, sheet);
		horizontalSizer->Add(pMyTable, wxSizerFlags(2).Expand());

		Bind(wxEVT_SIZE, [this, verticalSizer](wxSizeEvent& e)
		{
			auto size = GetClientSize();
			verticalSizer->SetMinSize(size.x / 3, size.y);
			e.Skip();
		});

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

		pMyBandwidthValue = new wxStaticText(this, -1, BANDWIDTH_TEXT + "N/A");
		verticalSizer->Add(pMyBandwidthValue);

		pMyCategorizerInputField = new wxTextCtrl(this, wxID_ANY);
		verticalSizer->Add(pMyCategorizerInputField);
		pMyCategorizerInputField->Bind(wxEVT_TEXT, [this](wxEvent&) { recalculateValues(); });

		pMyDiagram = new Diagram(this);
		verticalSizer->Add(pMyDiagram, wxSizerFlags(1).Expand());

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
			const AssessmentMatrix matrix = createAssessmentMatrixAndUpdateCellColors();
			const ONEST onest = calculateRandomPermutations(matrix, 100);

			pMyOPANValue->SetLabelText(OPAN_TEXT + to_string(calculateOPAN(onest)));
			pMyBandwidthValue->SetLabelText(BANDWIDTH_TEXT + to_string(calculateBandwidth(onest)));

			pMyDiagram->plotONEST(onest);

			SetStatusText("Ready");
		}
		catch (const exception& ex)
		{
			pMyOPANValue->SetLabelText(OPAN_TEXT + "N/A");
			pMyBandwidthValue->SetLabelText(BANDWIDTH_TEXT + "N/A");
			pMyDiagram->plotONEST(ONEST());
			SetStatusText("Error: "s + ex.what());
		}
	}

	AssessmentMatrix MainFrame::createAssessmentMatrixAndUpdateCellColors()
	{
		const int numberOfColumns = pMyTable->GetNumberCols();
		const int numberOfRows = pMyTable->GetNumberRows();

		const unsigned numberOfObservers = pMyTable->getNumberOfEnabledColumns();
		const unsigned numberOfCases = static_cast<unsigned>(numberOfRows);

		Categorizer categorizer;
		if (auto ruleString = pMyCategorizerInputField->GetValue().ToStdString(); !ruleString.empty())
			categorizer = Categorizer(ruleString);

		CategoryFactory categoryFactory;
		AssessmentMatrix matrix(numberOfObservers, numberOfCases);

		// TODO: Exception safety! (at the other places too!)
		pMyTable->BeginBatch();
		for (int i = 0; i < numberOfRows; ++i)
		{
			for (int j = 0, observerIndex = 0; j < numberOfColumns; ++j)
			{
				if (!pMyTable->isColumnEnabled(j))
					continue;

				const string cellValue = pMyTable->GetCellValue(i, j).ToStdString();
				Categorizer::Result categorization = categorizer.categorize(cellValue);
				string categoryValue = categorization.success ? string(categorization.category) : cellValue;
				matrix.set(observerIndex, i, categoryFactory.createCategory(categoryValue));
				++observerIndex;

				if (categorization.success)
					pMyTable->setCellHue(i, j, categorization.matchedRuleIndex, categorizer.getNumberOfRules());
				else
					pMyTable->resetCellHue(i, j);
			}
		}
		pMyTable->EndBatch();

		return matrix;
	}
}
