#include <filesystem>
#include <string>


namespace onest::io
{
	class File final
	{
	public:
		static std::string readFileAsString(const std::filesystem::path& path);
	};
}
