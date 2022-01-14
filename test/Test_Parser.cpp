#include "../src/csv/Parser.cpp"
#include "test.h"

#include <string>


#define TAG "[Parser] "

using namespace std;
using onest::csv::Parser;

CASE(TAG "Parsing single unquoted line without ending newline works.")
{
	// Given
	/*string csv = "a,bc,def";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);
	EXPECT(parsed[0].size() == 3);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "bc");
	EXPECT(parsed[0][2] == "def");*/

}

CASE(TAG "Parsing single unquoted line with ending newline works.")
{
	// Given
	string csv = "a,bc,def\n";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);
	EXPECT(parsed[0].size() == 3);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "bc");
	EXPECT(parsed[0][2] == "def");

}

CASE(TAG "Parsing two unquoted lines with empty line between them works.")
{
	// Given
	string csv = "a,bc,def\ngh,ij";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 2);

	EXPECT(parsed[0].size() == 3);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "bc");
	EXPECT(parsed[0][2] == "def");

	EXPECT(parsed[1].size() == 2);
	EXPECT(parsed[1][0] == "gh");
	EXPECT(parsed[1][1] == "ij");

}
