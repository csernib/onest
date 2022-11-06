#include "gui/MainFrame.h"

#include <wx/wxprec.h>
#ifndef WC_PRECOMP
	#include <wx/wx.h>
#endif


using onest::gui::MainFrame;

namespace
{
	class Application final : public wxApp
	{
	public:
		bool OnInit() override
		{
			wxInitAllImageHandlers();

			MainFrame* frame = new MainFrame();
			frame->Show(true);
			return true;
		}
	};
}

wxIMPLEMENT_APP(Application);
