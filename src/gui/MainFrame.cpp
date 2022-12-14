#include "MainFrame.h"
#include "AboutDialog.h"
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

#include <ranges>
#include <string>

#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/toolbar.h>

#include "rsc/calculation_toggle_100.h"
#include "rsc/calculation_toggle_all.h"
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
		TOOLBAR_CALCULATION_TOGGLE_BUTTON,
		TOOLBAR_HEADER_BUTTON
	};

	wxDEFINE_EVENT(EVENT_CALCULATION_SUCCESS, wxThreadEvent);
	wxDEFINE_EVENT(EVENT_CALCULATION_FAILURE, wxThreadEvent);

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, CMAKE_ONEST_APPLICATION_NAME " " CMAKE_ONEST_APPLICATION_VERSION)
	{
		#ifdef WIN32
			SetIcon(wxICON(WIN32_ONEST_APPLICATION_ICON));
		#endif

		Maximize();

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

	void MainFrame::DoGiveHelp(const wxString& help, bool show)
	{
		// This override is empty on purpose. Normally this would update the status bar text with help text
		// when toolbar items are hovered, but that feature is not used by this program.
		// On Windows it was also causing a flaky bug, where the status text during ONEST calculation would
		// sometimes incorrectly get replaced by "Ready".
	}

	void MainFrame::createToolbar()
	{
		wxToolBar* toolbar = CreateToolBar();

		toolbar->AddTool(wxID_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN), "Open CSV input...");
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showLoadFileDialog(); }, wxID_OPEN);

		toolbar->AddTool(wxID_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE), "Export ONEST output...");
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showSaveFileDialog(); }, wxID_SAVE);

		createRandomizationToggleButton();
		createCalculationModeToggleButton();
		createHeaderToggleButton();

		toolbar->AddStretchableSpace();
		toolbar->AddTool(wxID_ABOUT, "About", wxArtProvider::GetBitmap(wxART_INFORMATION, "wxART_OTHER_C", wxSize(16, 16)), "About this program...");
		toolbar->Bind(wxEVT_MENU, [this](wxEvent&) { showAboutDialog(); }, wxID_ABOUT);

		toolbar->Realize();
	}

	void MainFrame::createRandomizationToggleButton()
	{
		auto randomizationToggleButton = GetToolBar()->AddTool(
			TOOLBAR_DICE_BUTTON,
			"Randomize",
			wxBitmap::NewFromPNGData(rsc::dice, sizeof(rsc::dice)),
			"Use non-deterministic random numbers for permutation selection"
		);
		randomizationToggleButton->SetToggle(true);
		GetToolBar()->Bind(wxEVT_MENU, [this](wxEvent&)
		{
			if (!myCalculateAllPossiblePermutations)
				recalculateValues();
		}, TOOLBAR_DICE_BUTTON);
	}

	void MainFrame::createCalculationModeToggleButton()
	{
		GetToolBar()->AddTool(
			TOOLBAR_CALCULATION_TOGGLE_BUTTON,
			"Toggle calculation mode",
			wxNullBitmap,
			"Use all possible permutations for ONEST calculation or only a random 100?"
		);
		GetToolBar()->Bind(wxEVT_MENU, [this](wxEvent&)
		{
			if (!myCalculateAllPossiblePermutations && !haveUserAcceptWarningForAllPermutationsIfNeeded())
				return;
			setCalculationModeAndToolBarState(!myCalculateAllPossiblePermutations);
			recalculateValues();
		}, TOOLBAR_CALCULATION_TOGGLE_BUTTON);
		setCalculationModeAndToolBarState(myCalculateAllPossiblePermutations);
	}

	void MainFrame::createHeaderToggleButton()
	{
		auto headerButton = GetToolBar()->AddTool(
			TOOLBAR_HEADER_BUTTON,
			"Toggle header",
			wxBitmap::NewFromPNGData(rsc::header_toggle, sizeof(rsc::header_toggle)),
			"Is the first row a header?"
		);
		headerButton->SetToggle(true);
		GetToolBar()->Bind(
			wxEVT_MENU,
			[this, headerButton](wxEvent&) { pMyTable->setFirstRowAsHeader(headerButton->IsToggled()); recalculateValues(); },
			TOOLBAR_HEADER_BUTTON
		);
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

			if (myCalculateAllPossiblePermutations && !haveUserAcceptWarningForAllPermutationsIfNeeded())
			{
				pMyTable->changeColumnEnableStatus(event.GetCol());
				return;
			}

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

			if (myCalculateAllPossiblePermutations && !haveUserAcceptWarningForAllPermutationsIfNeeded())
			{
				setCalculationModeAndToolBarState(false);
				wxMessageBox(
					"Calculation mode has been reverted to use only 100 random permutations.",
					"Information",
					wxOK | wxICON_INFORMATION,
					this
				);
			}

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
			wxMessageBox("Nothing is calculated, so there is nothing to save.", "Information", wxICON_INFORMATION | wxOK);
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

	void MainFrame::showAboutDialog()
	{
		AboutDialog aboutDialog(this);
		aboutDialog.ShowModal();
	}

	void MainFrame::setCalculationModeAndToolBarState(bool calculateAll)
	{
		static const wxBitmap toggleIcon100 = wxBitmap::NewFromPNGData(rsc::calculation_toggle_100, sizeof(rsc::calculation_toggle_100));
		static const wxBitmap toggleIconAll = wxBitmap::NewFromPNGData(rsc::calculation_toggle_all, sizeof(rsc::calculation_toggle_all));

		myCalculateAllPossiblePermutations = calculateAll;
		GetToolBar()->SetToolNormalBitmap(TOOLBAR_CALCULATION_TOGGLE_BUTTON, calculateAll ? toggleIconAll : toggleIcon100);
	}

	bool MainFrame::haveUserAcceptWarningForAllPermutationsIfNeeded()
	{
		if (myUserAlreadyWarnedForAllPermutations)
			return true;

		const unsigned numberOfObservers = pMyTable->getNumberOfEnabledColumns();
		if (numberOfObservers >= 10)
		{
			static const char* message =
				"Calculating all possible permutations may take a very long time with this many observers...\n\n"
				"The application and your system may become unresponsive, and your operating system "
				"may even shut down other applications due to memory exhaustion.\n\n"
				"Are you sure you want to continue?";
			const int userDecision = wxMessageBox(message, "Warning", wxYES | wxNO | wxICON_WARNING, this);
			if (userDecision == wxYES)
			{
				myUserAlreadyWarnedForAllPermutations = true;
				return true;
			}
			else
				return false;
		}
		return true;
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

			auto onSuccess = [this](ONEST onest)
			{
				wxThreadEvent* event = new wxThreadEvent(EVENT_CALCULATION_SUCCESS);
				event->SetPayload(onest);
				QueueEvent(event);
			};

			auto onError = [this](exception_ptr exception)
			{
				wxThreadEvent* event = new wxThreadEvent(EVENT_CALCULATION_FAILURE);
				event->SetPayload(exception);
				QueueEvent(event);
			};

			if (myCalculateAllPossiblePermutations)
			{
				myCalculationThread = calculateAllPermutations(matrix, onSuccess, onError);
			}
			else
			{
				myCalculationThread = calculateRandomPermutations(
					matrix,
					100,
					randomizeSeedButton->IsToggled() ? RNG(random_device()()) : RNG(),
					onSuccess,
					onError
				);
			}
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
				const Categorizer::Result categorization = categorizer.categorize(cellValue);
				const string categoryValue = categorization.success ? string(categorization.category) : cellValue;
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
