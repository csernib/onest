#include "../src/csv/Exporter.h"
#include "test.h"


#define TAG "[CSV] "

using namespace std;
using onest::csv::exportCSV;
using onest::csv::Row;
using onest::csv::Sheet;
using onest::csv::Value;

CASE(TAG "Exporter creates the expected output.")
{
	// Given
	const Sheet sheet =
	{
		{ "ab", "cd", "ef" },
		{ "\"gh\"", "i\nj" },
		{ "k", "l,m,", "n" }
	};

	const string expectedOutput =
		R"("ab","cd","ef")" "\n"
		R"("""gh""","i)" "\nj\"\n"
		R"("k","l,m,","n")" "\n";

	// When
	const string output = exportCSV(sheet, ',', '"').str();

	// Then
	EXPECT(output == expectedOutput);
}
