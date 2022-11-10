#pragma once

#include <wx/dialog.h>


namespace onest::gui
{
	class AboutDialog final : public wxDialog
	{
	public:
		AboutDialog(wxWindow* parent);
	};
}
