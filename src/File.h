#include <filesystem>
#include <string>


namespace onest
{
	class File final
	{
	public:
		static std::string readFileAsString(const std::filesystem::path& path);
	};
}
