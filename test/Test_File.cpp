#include "../src/io/File.h"
#include "test.h"


#define TAG "[File] "

using namespace std;
namespace fs = std::filesystem;
using onest::io::File;
using test::testResourcesFolder;

namespace
{
	const string testFileContent = "Hello world!\nSpecial Hungarian characters: \xC5\xB1";
}

CASE(TAG "Reading an UTF-8 file with CRLF line endings works.")
{
	// Given
	fs::path file = testResourcesFolder / "CRLF_UTF-8.txt";

	// When
	string content = File::readFileAsString(file);

	// Then
	EXPECT(content == testFileContent);
}

CASE(TAG "Reading an UTF-8 file with LF line endings works.")
{
	// Given
	fs::path file = testResourcesFolder / "LF_UTF-8.txt";

	// When
	string content = File::readFileAsString(file);

	// Then
	EXPECT(content == testFileContent);
}

CASE(TAG "Reading an UTF-8 file with CR line endings works.")
{
	// Given
	fs::path file = testResourcesFolder / "CR_UTF-8.txt";

	// When
	string content = File::readFileAsString(file);

	// Then
	EXPECT(content == testFileContent);
}
