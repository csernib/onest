#include "../src/csv/Parser.h"
#include "../src/csv/ParserException.h"
#include "test.h"

#include <string>


#define TAG "[CSV] "

using namespace std;
using onest::csv::parseSheet;
using onest::csv::ParserException;

CASE(TAG "Parsing empty string returns empty sheet.")
{
	// Given
	string csv = "";

	// When
	auto parsed = parseSheet(csv, ',', '"');

	// Then
	EXPECT(parsed.empty());
}

CASE(TAG "Parsing a single newline returns a sheet with a single empty row.")
{
	// Given
	string csv = "\n";

	// When
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "");
}

CASE(TAG "Ending a quote at end of line works.")
{
	// Given
	string csv = "a,\"b\"\nc,d";

	// When
	auto parsed = parseSheet(csv, ',', '"');

	// Then
	EXPECT(parsed.size() == 2);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "b");

	EXPECT(parsed[1].size() == 2);
	EXPECT(parsed[1][0] == "c");
	EXPECT(parsed[1][1] == "d");
}

CASE(TAG "Ending a quote at end of cell works.")
{
	// Given
	string csv = "a,\"b\",c,d";

	// When
	auto parsed = parseSheet(csv, ',', '"');

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 4);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "b");
	EXPECT(parsed[0][2] == "c");
	EXPECT(parsed[0][3] == "d");
}

CASE(TAG "Quotes allow for separators within the quote.")
{
	// Given
	string csv = "a,\"bc,d,e\"";

	// When
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

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
	auto parsed = parseSheet(csv, ',', '"');

	// Then
	EXPECT(parsed.size() == 1);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "\"b,\"\"");
}

CASE(TAG "Parsing also supports CRLF line-ending.")
{
	// Given
	string csv = "a,\r\nb,c,\"d\r\nef\"";

	// When
	auto parsed = parseSheet(csv, ',', '"');

	// Then
	EXPECT(parsed.size() == 2);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "");

	EXPECT(parsed[1].size() == 3);
	EXPECT(parsed[1][0] == "b");
	EXPECT(parsed[1][1] == "c");
	EXPECT(parsed[1][2] == "d\r\nef");
}

CASE(TAG "Parsing also supports CR line-ending.")
{
	// Given
	string csv = "a,\rb,c,\"d\ref\"";

	// When
	auto parsed = parseSheet(csv, ',', '"');

	// Then
	EXPECT(parsed.size() == 2);

	EXPECT(parsed[0].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "");

	EXPECT(parsed[1].size() == 3);
	EXPECT(parsed[1][0] == "b");
	EXPECT(parsed[1][1] == "c");
	EXPECT(parsed[1][2] == "d\ref");
}

CASE(TAG "Having just one quote in the input causes ParserException.")
{
	// Given
	string csv = "\"";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Ending the input with opening quote causes ParserException.")
{
	// Given
	string csv = "a,\"";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Ending the input without a closing quote causes a ParserException.")
{
	// Given
	string csv = "\"ab";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Ending the input without a closing quote on a quoted quote causes a ParserException.")
{
	// Given
	string csv = "\"ab\"\"";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text before quoted value at start of text is disallowed.")
{
	// Given
	string csv = "a\"bc\"";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text before quoted value at start of line is disallowed.")
{
	// Given
	string csv = "x,y\na\"bc\"";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text before quoted value at start of cell is disallowed.")
{
	// Given
	string csv = "x,a\"bc\"";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text after quoted value at end of text is disallowed.")
{
	// Given
	string csv = "\"ab\"c";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text after quoted value at end of line is disallowed.")
{
	// Given
	string csv = "\"ab\"c\nx,y";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Mixing unquoted text after quoted value at end of cell is disallowed.")
{
	// Given
	string csv = "\"ab\"c,x";

	// When, then
	EXPECT_THROWS_AS(parseSheet(csv, ',', '"'), ParserException);
}

CASE(TAG "Splitting by semi-colon and different quote char also works.")
{
	// Given
	string csv = "a;b;c,d\n\'e\nf''ghi';j";

	// When
	auto parsed = parseSheet(csv, ';', '\'');

	// Then
	EXPECT(parsed.size() == 2);
	EXPECT(parsed[0].size() == 3);
	EXPECT(parsed[1].size() == 2);
	EXPECT(parsed[0][0] == "a");
	EXPECT(parsed[0][1] == "b");
	EXPECT(parsed[0][2] == "c,d");
	EXPECT(parsed[1][0] == "e\nf'ghi");
	EXPECT(parsed[1][1] == "j");
}
