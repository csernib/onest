#include <wx/wxprec.h>

#ifndef WC_PRECOMP
	#include <wx/wx.h>
#endif


class MainFrame : public wxFrame
{
public:
	MainFrame();
};

class Application : public wxApp
{
public:
	bool OnInit() override
	{
		MainFrame* frame = new MainFrame();
		frame->Show(true);
		return true;
	}
};

wxIMPLEMENT_APP(Application);

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Hello wxWidgets!")
{}
