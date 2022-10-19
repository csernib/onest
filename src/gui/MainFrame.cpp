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

#include "rsc/dice.h"
#include "rsc/header_toggle.h"
#include "rsc/plot_save.h"


using namespace onest::calc;
using namespace onest::rule;
using namespace std;

namespace onest::gui
{
	const string MainFrame::OPAN_TEXT = "OPA(N): ";
	const string MainFrame::BANDWIDTH_TEXT = "Bandwidth: ";
	const string MainFrame::OBSERVERS_NEEDED_TEXT = "Observers needed: ";
	const string MainFrame::DIAGRAM_TITLE_TEXT = "ONEST plot";
	const string MainFrame::SIMPLIFIED_DIAGRAM_TITLE_TEXT = "Simplified ONEST plot";
	const string MainFrame::UNDEFINED_VALUE_TEXT = "N/A";

	enum
	{
		TOOLBAR_PLOT_SAVE_BUTTON = wxID_HIGHEST + 1,
		TOOLBAR_DICE_BUTTON,
		TOOLBAR_HEADER_BUTTON
	};

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "ONEST")
	{
		SetTitle("ONEST Pre-alpha    |  " + git::getVersionInfo());

		CreateStatusBar();

		createToolbar();
		createMainLayoutSizers();
		createTable(csv::Sheet());
		createLayoutOnTheLeft();

		csv::Sheet emptySheet(100);
		for (csv::Row& row : emptySheet)
			row.resize(18);
		createTable(emptySheet);
		pMyTable->setFirstRowAsHeader(false);

		recalculateValues();
	}

	void MainFrame::createToolbar()
	{
		wxToolBar* toolbar = CreateToolBar();

		toolbar->AddTool(wxID_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN), "Open CSV input...");
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showLoadFileDialog(); }, wxID_OPEN);

		toolbar->AddTool(wxID_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE), "Export ONEST output...");
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showSaveFileDialog(); }, wxID_SAVE);

		toolbar->AddTool(
			TOOLBAR_PLOT_SAVE_BUTTON,
			"Save plot",
			wxBitmap::NewFromPNGData(rsc::plot_save, sizeof(rsc::plot_save)),
			"Save ONEST plot..."
		);
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showPlotSaveDialog(); }, TOOLBAR_PLOT_SAVE_BUTTON);

		auto diceButton = toolbar->AddTool(
			TOOLBAR_DICE_BUTTON,
			"Randomize",
			wxBitmap::NewFromPNGData(rsc::dice, sizeof(rsc::dice)),
			"Use non-deterministic random numbers for permutation selection"
		);
		diceButton->SetToggle(true);
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { recalculateValues(); }, TOOLBAR_DICE_BUTTON);

		auto headerButton = toolbar->AddTool(
			TOOLBAR_HEADER_BUTTON,
			"Toggle header",
			wxBitmap::NewFromPNGData(rsc::header_toggle, sizeof(rsc::header_toggle)),
			"Is the first row a header?"
		);
		headerButton->SetToggle(true);
		toolbar->Bind(
			wxEVT_MENU,
			[this, headerButton](wxEvent&) { pMyTable->setFirstRowAsHeader(headerButton->IsToggled()); recalculateValues(); },
			TOOLBAR_HEADER_BUTTON
		);

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
		pMyOPANValue = new wxStaticText(this, wxID_ANY, OPAN_TEXT + UNDEFINED_VALUE_TEXT);
		pMyLeftVerticalLayout->Add(pMyOPANValue);

		pMyBandwidthValue = new wxStaticText(this, wxID_ANY, BANDWIDTH_TEXT + UNDEFINED_VALUE_TEXT);
		pMyLeftVerticalLayout->Add(pMyBandwidthValue);

		pMyObserversNeededValue = new wxStaticText(this, wxID_ANY, OBSERVERS_NEEDED_TEXT + UNDEFINED_VALUE_TEXT);
		pMyLeftVerticalLayout->Add(pMyObserversNeededValue);

		pMyCategorizerInputField = new wxTextCtrl(this, wxID_ANY);
		pMyLeftVerticalLayout->Add(pMyCategorizerInputField);
		pMyCategorizerInputField->Bind(wxEVT_TEXT, [this](wxEvent&) { recalculateValues(); });

		pMyDiagram = new Diagram(this, DIAGRAM_TITLE_TEXT);
		pMyLeftVerticalLayout->Add(pMyDiagram, wxSizerFlags(1).Expand());

		pMySimplifiedDiagram = new Diagram(this, SIMPLIFIED_DIAGRAM_TITLE_TEXT);
		pMyLeftVerticalLayout->Add(pMySimplifiedDiagram, wxSizerFlags(1).Expand());
	}

	void MainFrame::showLoadFileDialog()
	{
		csv::Sheet sheet;
		wxFileDialog* fileOpenDialog = new wxFileDialog(
			this,
			"Choose a file to open....",
			wxEmptyString,
			wxEmptyString,
			"CSV files (semicolon-separated)|*.csv|CSV files (comma-separated)|*.csv",
			wxFD_OPEN | wxFD_FILE_MUST_EXIST,
			wxDefaultPosition
		);
		if (fileOpenDialog->ShowModal() == wxID_OK)
		{
			// TODO: Exception handling!
			sheet = csv::parseSheet(
				io::File::readFileAsString(fileOpenDialog->GetPath().ToStdString()),
				fileOpenDialog->GetFilterIndex() == 0 ? ';' : ',',
				'"'
			);

			createTable(sheet);

			GetToolBar()->ToggleTool(TOOLBAR_HEADER_BUTTON, pMyTable->isFirstRowHeader());

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

		wxFileDialog* fileSaveDialog = new wxFileDialog(
			this,
			"Save ONEST data...",
			wxEmptyString,
			wxEmptyString,
			"CSV files (semicolon-separated)|*.csv|CSV files (comma-separated)|*.csv",
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
			wxDefaultPosition
		);
		if (fileSaveDialog->ShowModal() == wxID_OK)
		{
			csv::Sheet sheet;
			for (const OPAC& opac : myONEST)
			{
				csv::Row row;
				ranges::copy(opac | views::transform([](number_t opa) { return to_string(opa); }), back_inserter(row));
				sheet.push_back(move(row));
			}

			io::File::writeFile(
				fileSaveDialog->GetPath().ToStdString(),
				csv::exportCSV(sheet, fileSaveDialog->GetFilterIndex() == 0 ? ';' : ',', '"')
			);
		}
		fileSaveDialog->Destroy();
	}

	void MainFrame::showPlotSaveDialog()
	{
		if (myONEST.empty())
		{
			wxMessageBox("Nothing is calculated yet, so there is nothing to save.", "Information", wxICON_INFORMATION | wxOK);
			return;
		}

		wxFileDialog* fileSaveDialog = new wxFileDialog(
			this,
			"Save ONEST plot...",
			wxEmptyString,
			wxEmptyString,
			"PNG files|*.png",
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
			wxDefaultPosition
		);
		if (fileSaveDialog->ShowModal() == wxID_OK)
		{
			pMyDiagram->renderToBitmap().SaveFile(fileSaveDialog->GetPath(), wxBITMAP_TYPE_PNG);
		}
		fileSaveDialog->Destroy();
	}

	void MainFrame::recalculateValues()
	{
		SetStatusText("Calculating ONEST...");
		myONEST.clear();
		try
		{
			auto randomizeSeedButton = GetToolBar()->FindById(TOOLBAR_DICE_BUTTON);
			assert(randomizeSeedButton && "This should always exist.");

			// TODO: Do it in a different thread!
			const AssessmentMatrix matrix = createAssessmentMatrixAndUpdateCellColors();
			myONEST = calculateRandomPermutations(
				matrix,
				100,
				randomizeSeedButton->IsToggled() ? mt19937_64(random_device()()) : mt19937_64()
			);

			pMyOPANValue->SetLabelText(OPAN_TEXT + to_string(calculateOPAN(myONEST)));
			pMyBandwidthValue->SetLabelText(BANDWIDTH_TEXT + to_string(calculateBandwidth(myONEST)));

			const ObserversNeeded observersNeeded = calculateObserversNeeded(myONEST);
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
			pMyObserversNeededValue->SetLabelText(OBSERVERS_NEEDED_TEXT + observersNeededText);

			pMyDiagram->plotONEST(myONEST);
			pMySimplifiedDiagram->plotONEST(simplifyONEST(myONEST));

			SetStatusText("Ready");
		}
		catch (const exception& ex)
		{
			myONEST.clear();
			pMyOPANValue->SetLabelText(OPAN_TEXT + UNDEFINED_VALUE_TEXT);
			pMyBandwidthValue->SetLabelText(BANDWIDTH_TEXT + UNDEFINED_VALUE_TEXT);
			pMyDiagram->plotONEST(ONEST());
			pMySimplifiedDiagram->plotONEST(ONEST());
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
