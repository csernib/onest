#include "File.h"

#include <fstream>
#include <sstream>


using namespace std;
namespace fs = filesystem;

namespace onest
{
	string File::readFileAsString(const fs::path& path)
	{
		ifstream file(path);
		file.exceptions(ifstream::failbit | ifstream::badbit);

		string content;
		{
			ostringstream ss;
			ss << file.rdbuf();
			content = move(ss).str();
		}

		return content;
	}
}
