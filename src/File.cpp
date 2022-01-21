#include "File.h"

#include <fstream>
#include <sstream>
#include <regex>


using namespace std;
namespace fs = filesystem;

namespace onest
{
	string File::readFileAsString(const fs::path& path)
	{
		static const regex lineEndingNormalizer("\r\n|\r", regex_constants::optimize);

		ifstream file(path);
		file.exceptions(ifstream::failbit | ifstream::badbit);

		string content;
		{
			ostringstream ss;
			ss << file.rdbuf();
			content = move(ss).str();
		}

		content = regex_replace(content, lineEndingNormalizer, "\n");

		return content;
	}
}
