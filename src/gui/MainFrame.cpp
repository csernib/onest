#include "MainFrame.h"
#include "Table.h"

#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/sizer.h>

#include "../csv/Parser.h"
#include "../io/File.h"


namespace onest::gui
{
	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "ONEST")
	{
		csv::Sheet sheet;

		wxFileDialog* fileOpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, wxEmptyString, "CSV files (*.csv)|*.csv|All files|*", wxFD_OPEN, wxDefaultPosition);
		if (fileOpenDialog->ShowModal() == wxID_OK)
		{
			sheet = csv::parseSheet(io::File::readFileAsString(fileOpenDialog->GetPath().ToStdString()), ';', '"');
		}
		fileOpenDialog->Destroy();

		wxBoxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

		Table* table = new Table(this, sheet);

		wxCheckBox* headerCheckbox = new wxCheckBox(this, -1, "Header");
		headerCheckbox->SetValue(table->isFirstRowHeader());
		headerCheckbox->Bind(wxEVT_CHECKBOX, [table](wxCommandEvent& e)
		{
			table->setFirstRowAsHeader(e.IsChecked());
		});
		horizontalSizer->Add(headerCheckbox);
		horizontalSizer->Add(table);

		SetSizer(horizontalSizer);
	}
}
