#include "MainFrame.h"
#include "CategoryGrid.h"
#include "Diagram.h"
#include "ResultGrid.h"
#include "Table.h"
#include "common.h"

#include "../csv/Exporter.h"
#include "../csv/Parser.h"
#include "../csv/ParserException.h"
#include "../io/File.h"
#include "../rule/Categorizer.h"
#include "../git.h"

#include <ranges>
#include <string>

#include <wx/artprov.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/toolbar.h>

#include "rsc/dice.h"
#include "rsc/header_toggle.h"


using namespace onest::calc;
using namespace onest::rule;
using namespace std;

namespace
{
	const string DIAGRAM_TITLE_TEXT = "ONEST plot";
	const string SIMPLIFIED_DIAGRAM_TITLE_TEXT = "Simplified ONEST plot";
}

namespace onest::gui
{
	enum
	{
		TOOLBAR_DICE_BUTTON = wxID_HIGHEST + 1,
		TOOLBAR_HEADER_BUTTON
	};

	wxDEFINE_EVENT(EVENT_CALCULATION_SUCCESS, wxThreadEvent);
	wxDEFINE_EVENT(EVENT_CALCULATION_FAILURE, wxThreadEvent);

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "ONEST")
	{
		SetTitle("ONEST Pre-alpha    |  " + git::getVersionInfo());

		Bind(EVENT_CALCULATION_SUCCESS, &MainFrame::handleCalculationSuccess, this);
		Bind(EVENT_CALCULATION_FAILURE, &MainFrame::handleCalculationFailure, this);

		CreateStatusBar();

		createToolbar();
		createMainLayoutSizers();
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
			const wxSize size = GetClientSize();

			if (pMyLeftVerticalLayout)
				pMyLeftVerticalLayout->SetMinSize(size.x / 3, size.y);

			if (pMyCategoryGrid)
				pMyCategoryGrid->SetMaxSize(pMyLeftVerticalLayout->GetMinSize());

			if (pMyCategorizerInputField && pMyCategorizerLabel)
			{
				const int categorizerLabelX = pMyCategorizerLabel->GetSize().x;
				const int categorizerSizeX = size.x / 3 - categorizerLabelX - 10;
				if (categorizerSizeX > categorizerLabelX + 10)
					pMyCategorizerInputField->SetMinSize({ categorizerSizeX, pMyCategorizerInputField->GetMinSize().y });
				else
					pMyCategorizerInputField->SetMinSize({ categorizerLabelX + 10, pMyCategorizerInputField->GetMinSize().y });
			}

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
		pMyLeftVerticalLayout->AddSpacer(5);

		wxBoxSizer* categorizerLabelAndInputField = new wxBoxSizer(wxHORIZONTAL);

		pMyCategorizerLabel = new wxStaticText(this, wxID_ANY, "Categorizer: ");
		categorizerLabelAndInputField->Add(pMyCategorizerLabel, wxSizerFlags().CenterVertical());

		pMyCategorizerInputField = new wxTextCtrl(this, wxID_ANY);
		pMyCategorizerInputField->Bind(wxEVT_TEXT, [this](wxEvent&) { recalculateValues(); });
		categorizerLabelAndInputField->Add(pMyCategorizerInputField);

		pMyLeftVerticalLayout->Add(categorizerLabelAndInputField);

		pMyLeftVerticalLayout->AddSpacer(4);

		pMyCategoryGrid = new CategoryGrid(this);
		pMyLeftVerticalLayout->Add(pMyCategoryGrid, wxSizerFlags().Expand());

		pMyResultGrid = new ResultGrid(this);
		pMyLeftVerticalLayout->Add(pMyResultGrid, wxSizerFlags().Expand());

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
			try
			{
				sheet = csv::parseSheet(
					io::File::readFileAsString(fileOpenDialog->GetPath().ToStdString()),
					fileOpenDialog->GetFilterIndex() == 0 ? ';' : ',',
					'"'
				);
			}
			catch (const csv::ParserException& ex)
			{
				wxMessageBox(ex.what(), "Error", wxICON_ERROR);
				fileOpenDialog->Destroy();
				return;
			}

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

	void MainFrame::recalculateValues()
	{
		DeletePendingEvents();
		SetStatusText("Calculating ONEST...");
		myONEST.clear();
		try
		{
			auto randomizeSeedButton = GetToolBar()->FindById(TOOLBAR_DICE_BUTTON);
			assert(randomizeSeedButton && "This should always exist.");

			CategoryFactory categoryFactory;
			const AssessmentMatrix matrix = createAssessmentMatrixAndUpdateCellColors(categoryFactory);
			pMyCategoryGrid->refreshCategoryDistributionTable(matrix, categoryFactory);

			myCalculationThread = calculateRandomPermutations(
				matrix,
				100,
				randomizeSeedButton->IsToggled() ? RNG(random_device()()) : RNG(),
				[this](ONEST onest)
				{
					wxThreadEvent* event = new wxThreadEvent(EVENT_CALCULATION_SUCCESS);
					event->SetPayload(onest);
					QueueEvent(event);
				},
				[this](exception_ptr exception)
				{
					wxThreadEvent* event = new wxThreadEvent(EVENT_CALCULATION_FAILURE);
					event->SetPayload(exception);
					QueueEvent(event);
				}
			);
		}
		catch (const exception& ex)
		{
			handleError(ex.what());
		}
	}

	void MainFrame::handleCalculationSuccess(const wxThreadEvent& event)
	{
		assert(event.GetEventType() == EVENT_CALCULATION_SUCCESS);

		myONEST = event.GetPayload<ONEST>();

		pMyResultGrid->updateResults(myONEST);

		pMyDiagram->plotONEST(myONEST);
		pMySimplifiedDiagram->plotONEST(simplifyONEST(myONEST));

		SetStatusText("Ready");
	}

	void MainFrame::handleCalculationFailure(const wxThreadEvent& event)
	{
		assert(event.GetEventType() == EVENT_CALCULATION_FAILURE);

		try
		{
			rethrow_exception(event.GetPayload<exception_ptr>());
		}
		catch (const exception& ex)
		{
			handleError(ex.what());
		}
	}

	void MainFrame::handleError(const std::string& errorMessage)
	{
		myONEST.clear();

		pMyResultGrid->clear();
		pMyCategoryGrid->clear();

		pMyDiagram->plotONEST(ONEST());
		pMySimplifiedDiagram->plotONEST(ONEST());
		SetStatusText("Error: "s + errorMessage);
	}

	AssessmentMatrix MainFrame::createAssessmentMatrixAndUpdateCellColors(CategoryFactory& categoryFactory)
	{
		const int numberOfColumns = pMyTable->GetNumberCols();
		const int numberOfRows = pMyTable->GetNumberRows();

		const unsigned numberOfObservers = pMyTable->getNumberOfEnabledColumns();
		const unsigned numberOfCases = static_cast<unsigned>(numberOfRows);

		Categorizer categorizer;
		if (auto ruleString = pMyCategorizerInputField->GetValue().ToStdString(); !ruleString.empty())
			categorizer = Categorizer(ruleString);

		AssessmentMatrix matrix(numberOfObservers, numberOfCases);

		auto batchLock = autoCloseBatchUpdate(pMyTable);
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

		return matrix;
	}
}
