#include <filesystem>
#include <ios>
#include <string>


namespace onest::io
{
	class File final
	{
	public:
		static std::string readFileAsString(const std::filesystem::path& path);
		static void writeFile(const std::filesystem::path& path, const std::ios& stream);
	};
}
