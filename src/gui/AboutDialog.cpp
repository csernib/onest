#include "AboutDialog.h"

#include "../git.h"

#include <wx/display.h>
#include <wx/html/htmlwin.h>
#include <wx/sizer.h>


#define ABOUT_DIALOG_TITLE "About " CMAKE_ONEST_APPLICATION_NAME "..."

#define ABOUT_DIALOG_PROGRAM_DESCRIPTION                                                                              \
    "Program used for ONEST (Observers Needed to Evaluate Subjective Tests) calculation.<br>"                         \
    "The algorithm used by the program is described in the supplementary material of the following paper:"            \
    "<p><cite>Cserni B, Bori R, Cs\u00f6rg\u0151 E, Ol\u00e1h-N\u00e9meth O, Pancsa T, Sejben A, "                    \
    "Sejben I, V\u00f6r\u00f6s A, Zombori T, Ny\u00e1ri T, Cserni G.<br>"                                             \
    "The additional value of ONEST (Observers Needed to Evaluate Subjective Tests) in assessing "                     \
    "reproducibility of oestrogen receptor, progesterone receptor and Ki67 classification in breast cancer.<br>"      \
    "Virchows Arch 2021;479(6):1101-1109. doi: 10.1007/s00428-021-03172-9</cite></p>"

#define ABOUT_DIALOG_HTML_TEXT                                                                                        \
    "<p><b>" CMAKE_ONEST_APPLICATION_NAME "</b></p>"                                                                  \
    "<p><b>Version: </b>" CMAKE_ONEST_APPLICATION_VERSION "<br>"                                                      \
    "<b>Commit: </b>REPLACE_WITH_GIT_COMMIT_HASH</p>"                                                                 \
    "<p><b>Developed by B\u00e1lint Cserni</b><br>"                                                                   \
    "<b>Contact: </b><i><a href=\"mailto:" CMAKE_ONEST_CONTACT_EMAIL "\">" CMAKE_ONEST_CONTACT_EMAIL "</a></i><br>"   \
    "<b>Project website: </b><i><a href=\"" CMAKE_ONEST_PROJECT_PAGE "\">" CMAKE_ONEST_PROJECT_PAGE "</a></i></p>"    \
    "<p>" ABOUT_DIALOG_PROGRAM_DESCRIPTION "</p><br>"                                                                 \
    "<hr><div align=\"justify\"><i>" "REPLACE_WITH_CMAKE_ONEST_LICENSE_TEXT" "</i></div>"

namespace
{
	class HtmlWindow final : public wxHtmlWindow
	{
	public:
		HtmlWindow(wxWindow* parent) : wxHtmlWindow(parent)
		{}

		void OnLinkClicked(const wxHtmlLinkInfo& link) override
		{
			wxLaunchDefaultBrowser(link.GetHref());
		}
	};
}

namespace onest::gui
{
	AboutDialog::AboutDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, ABOUT_DIALOG_TITLE)
	{
		static const wxString htmlAboutText = []
		{
			wxString licenseText(CMAKE_ONEST_LICENSE_TEXT);
			licenseText.Replace("\\n", "<br>");

			wxString htmlAboutText(ABOUT_DIALOG_HTML_TEXT);
			htmlAboutText.Replace("REPLACE_WITH_GIT_COMMIT_HASH", git::getCommitInfo());
			htmlAboutText.Replace("REPLACE_WITH_CMAKE_ONEST_LICENSE_TEXT", licenseText);

			return htmlAboutText;
		}();

		SetWindowStyle(wxCAPTION);

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		SetSizer(sizer);

		HtmlWindow* htmlWindow = new HtmlWindow(this);
		sizer->Add(htmlWindow, wxSizerFlags(1).Expand());
		htmlWindow->AppendToPage(htmlAboutText);

		if (wxSizer* buttonSizer = CreateButtonSizer(wxOK))
			sizer->Add(buttonSizer, wxSizerFlags().Expand());


		const wxRect displaySize = wxDisplay(wxDisplay::GetFromWindow(this)).GetClientArea();

		// First resize the width of the client area, as the HTML window inherits this.
		// The height does not matter, as the HTML window will fill it out anyway because of the second call.
		SetClientSize(displaySize.width / 5 * 2, 0);
		SetClientSize(htmlWindow->GetVirtualSize());
	}
}
