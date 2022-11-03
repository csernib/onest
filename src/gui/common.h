#include <wx/grid.h>


namespace onest::gui
{
	[[nodiscard]] auto autoCloseBatchUpdate(wxGrid* grid)
	{
		struct RAIIWrapper
		{
			RAIIWrapper(wxGrid* grid) : grid(grid) { grid->BeginBatch(); }
			RAIIWrapper(const RAIIWrapper&) = delete;
			RAIIWrapper& operator=(const RAIIWrapper&) = delete;
			~RAIIWrapper() { grid->EndBatch(); }
			wxGrid* grid;
		};

		return RAIIWrapper(grid);
	}
}
