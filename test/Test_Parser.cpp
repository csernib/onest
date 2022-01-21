#include "../src/csv/Parser.cpp"
#include "test.h"

#include <string>


#define TAG "[Parser] "

using namespace std;
using onest::csv::Parser;
using onest::csv::ParserException;

CASE(TAG "Parsing empty string returns empty sheet.")
{
	// Given
	string csv = "";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.empty());
}

CASE(TAG "Parsing a single newline returns a sheet with a single empty row.")
{
	// Given
	string csv = "\n";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);
	EXPECT(parsed[0].size() == 1);
	EXPECT(parsed[0][0] == "");
}

CASE(TAG "Parsing single unquoted line without ending newline works.")
{
	// Given
	string csv = "a,bc,def";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);
	EXPECT(parsed[0].size() == 3);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "bc");
	EXPECT(parsed[0][2] == "def");
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

CASE(TAG "Parsing two unquoted lines works.")
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

CASE(TAG "Parsing new line in unquoted string is considered an empty row.")
{
	// Given
	string csv = "a,b\n\ncd";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 3);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "b");

	EXPECT(parsed[1].size() == 1);
	EXPECT(parsed[1][0] == "");

	EXPECT(parsed[2].size() == 1);
	EXPECT(parsed[2][0] == "cd");
}

CASE(TAG "Separators next to each other cause empty strings to be added.")
{
	// Given
	string csv = "ab,,,cd";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 4);
	EXPECT(parsed[0][0] == "ab");
	EXPECT(parsed[0][1] == "");
	EXPECT(parsed[0][2] == "");
	EXPECT(parsed[0][3] == "cd");
}

CASE(TAG "Lines ending with a separator cause an empty string to be added.")
{
	// Given
	string csv = "a,b,\ncd";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 2);

	EXPECT(parsed[0].size() == 3);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "b");
	EXPECT(parsed[0][2] == "");

	EXPECT(parsed[1].size() == 1);
	EXPECT(parsed[1][0] == "cd");
}

CASE(TAG "Input ending with a separator causes an empty string to be added.")
{
	// Given
	string csv = "a,b,";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 3);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "b");
	EXPECT(parsed[0][2] == "");
}

CASE(TAG "Empty quote is parsed as empty string.")
{
	// Given
	string csv = "a,\"\"";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "");
}

CASE(TAG "Quotes allow for separators within the quote.")
{
	// Given
	string csv = "a,\"bc,d,e\"";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "bc,d,e");
}

CASE(TAG "Line-breaks are preserved within quotes.")
{
	// Given
	string csv = "a,\"bc\nd\"";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "bc\nd");
}

CASE(TAG "Using two quotes within a quote results in a single quote in the output.")
{
	// Given
	string csv = "a,\"\"\"b,\"\"\"\"\"";

	// When
	auto parsed = Parser(csv, ',', '"').getSheet();

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "\"b,\"\"");
}

CASE(TAG "Having just one quote in the input causes ParserException.")
{
	// Given
	string csv = "\"";

	// When, then
	EXPECT_THROWS_AS(Parser(csv, ',', '"'), ParserException);
}

CASE(TAG "Ending the input with opening quote causes ParserException.")
{
	// Given
	string csv = "a,\"";

	// When, then
	EXPECT_THROWS_AS(Parser(csv, ',', '"'), ParserException);
}

CASE(TAG "Ending the input without a closing quote causes a ParserException.")
{
	// Given
	string csv = "\"ab";

	// When, then
	EXPECT_THROWS_AS(Parser(csv, ',', '"'), ParserException);
}

CASE(TAG "Ending the input without a closing quote on a quoted quote causes a ParserException.")
{
	// Given
	string csv = "\"ab\"\"";

	// When, then
	EXPECT_THROWS_AS(Parser(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text before quoted value is disallowed.")
{
	// Given
	string csv = "a\"bc\"";

	// When, then
	EXPECT_THROWS_AS(Parser(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text after quoted value is disallowed.")
{
	// Given
	string csv = "\"ab\"c";

	// When, then
	EXPECT_THROWS_AS(Parser(csv, ',', '"'), ParserException);
}
