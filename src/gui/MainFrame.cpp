#include "MainFrame.h"
#include "Diagram.h"
#include "Table.h"

#include "../calc/CategoryFactory.h"
#include "../csv/Exporter.h"
#include "../csv/Parser.h"
#include "../io/File.h"
#include "../rule/Categorizer.h"
#include "../git.h"

#include <ranges>

#include <wx/artprov.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/toolbar.h>


using namespace onest::calc;
using namespace onest::rule;
using namespace std;

namespace onest::gui
{
	const string MainFrame::OPAN_TEXT = "OPA(N): ";
	const string MainFrame::BANDWIDTH_TEXT = "Bandwidth: ";

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "ONEST")
	{
		SetTitle("ONEST Pre-alpha    |  " + git::getVersionInfo());

		CreateStatusBar();

		createToolbar();
		createMainLayoutSizers();
		createTable(csv::Sheet());
		createLayoutOnTheLeft();

		showLoadFileDialog();

		recalculateValues();
	}

	void MainFrame::createToolbar()
	{
		wxToolBar* toolbar = CreateToolBar();

		toolbar->AddTool(wxID_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN));
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showLoadFileDialog(); }, wxID_OPEN);

		toolbar->AddTool(wxID_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE));
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showSaveFileDialog(); }, wxID_SAVE);

		toolbar->Realize();
	}

	void MainFrame::createMainLayoutSizers()
	{
		pMyMainHorizontalLayout = new wxBoxSizer(wxHORIZONTAL);
		SetSizer(pMyMainHorizontalLayout);

		pMyLeftVerticalLayout = new wxBoxSizer(wxVERTICAL);
		pMyMainHorizontalLayout->Add(pMyLeftVerticalLayout, wxSizerFlags(1).Expand());

		Bind(wxEVT_SIZE, [this](wxSizeEvent& e)
		{
			auto size = GetClientSize();
			pMyLeftVerticalLayout->SetMinSize(size.x / 3, size.y);
			e.Skip();
		});
	}

	void MainFrame::createTable(const csv::Sheet& sheet)
	{
		if (pMyTable)
		{
			pMyMainHorizontalLayout->Detach(pMyTable);
			pMyTable->Destroy();
		}

		pMyTable = new Table(this, sheet);
		pMyMainHorizontalLayout->Add(pMyTable, wxSizerFlags(2).Expand());

		pMyTable->Bind(wxEVT_GRID_LABEL_LEFT_CLICK, [this](const wxGridEvent& event)
		{
			pMyTable->changeColumnEnableStatus(event.GetCol());
			recalculateValues();
		});

		pMyTable->Bind(wxEVT_GRID_CELL_CHANGED, [this](const wxGridEvent&)
		{
			recalculateValues();
		});
	}

	void MainFrame::createLayoutOnTheLeft()
	{
		pMyHeaderCheckbox = new wxCheckBox(this, -1, "Header");
		pMyLeftVerticalLayout->Add(pMyHeaderCheckbox);
		pMyHeaderCheckbox->SetValue(pMyTable->isFirstRowHeader());
		pMyHeaderCheckbox->Bind(wxEVT_CHECKBOX, [this](const wxCommandEvent& e)
		{
			pMyTable->setFirstRowAsHeader(e.IsChecked());
			recalculateValues();
		});

		pMyOPANValue = new wxStaticText(this, -1, OPAN_TEXT + "N/A");
		pMyLeftVerticalLayout->Add(pMyOPANValue);

		pMyBandwidthValue = new wxStaticText(this, -1, BANDWIDTH_TEXT + "N/A");
		pMyLeftVerticalLayout->Add(pMyBandwidthValue);

		pMyCategorizerInputField = new wxTextCtrl(this, wxID_ANY);
		pMyLeftVerticalLayout->Add(pMyCategorizerInputField);
		pMyCategorizerInputField->Bind(wxEVT_TEXT, [this](wxEvent&) { recalculateValues(); });

		pMyDiagram = new Diagram(this);
		pMyLeftVerticalLayout->Add(pMyDiagram, wxSizerFlags(1).Expand());
	}

	void MainFrame::showLoadFileDialog()
	{
		csv::Sheet sheet;
		wxFileDialog* fileOpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, wxEmptyString, "CSV files (*.csv)|*.csv|All files|*", wxFD_OPEN, wxDefaultPosition);
		if (fileOpenDialog->ShowModal() == wxID_OK)
		{
			// TODO: Exception handling!
			sheet = csv::parseSheet(io::File::readFileAsString(fileOpenDialog->GetPath().ToStdString()), ';', '"');

			createTable(sheet);
			pMyHeaderCheckbox->SetValue(pMyTable->isFirstRowHeader());

			myONEST.clear();
			myONEST.shrink_to_fit();

			recalculateValues();
			Refresh();
			SendSizeEvent();
		}
		fileOpenDialog->Destroy();
	}

	void MainFrame::showSaveFileDialog()
	{
		if (myONEST.empty())
		{
			wxMessageBox("Nothing is calculated yet, so there is nothing to save.", "Information", wxICON_INFORMATION | wxOK);
			return;
		}

		wxFileDialog* fileSaveDialog = new wxFileDialog(this, _("Save..."), wxEmptyString, wxEmptyString, "CSV files (*.csv)|*.csv", wxFD_SAVE, wxDefaultPosition);
		if (fileSaveDialog->ShowModal() == wxID_OK)
		{
			csv::Sheet sheet;
			for (const OPAC& opac : myONEST)
			{
				csv::Row row;
				ranges::copy(opac | views::transform([](number_t opa) { return to_string(opa); }), back_inserter(row));
				sheet.push_back(move(row));
			}

			io::File::writeFile(fileSaveDialog->GetPath().ToStdString(), csv::exportCSV(sheet, ',', '"'));
			fileSaveDialog->Destroy();
		}
	}

	void MainFrame::recalculateValues()
	{
		SetStatusText("Calculating ONEST...");
		myONEST.clear();
		try
		{
			// TODO: Do it in a different thread!
			const AssessmentMatrix matrix = createAssessmentMatrixAndUpdateCellColors();
			myONEST = calculateRandomPermutations(matrix, 100);

			pMyOPANValue->SetLabelText(OPAN_TEXT + to_string(calculateOPAN(myONEST)));
			pMyBandwidthValue->SetLabelText(BANDWIDTH_TEXT + to_string(calculateBandwidth(myONEST)));

			pMyDiagram->plotONEST(myONEST);

			SetStatusText("Ready");
		}
		catch (const exception& ex)
		{
			myONEST.clear();
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
