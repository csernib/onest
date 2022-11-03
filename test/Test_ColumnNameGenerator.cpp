#include "../src/gui/ColumnNameGenerator.h"
#include "test.h"


#define TAG "[ColumnNameGenerator] "

using namespace std;

using onest::gui::ColumnNameGenerator;

CASE(TAG "Default column names are be generated correctly for the first few columns.")
{
	EXPECT(ColumnNameGenerator::generateNameForColumn(0) == "A");
	EXPECT(ColumnNameGenerator::generateNameForColumn(1) == "B");
	EXPECT(ColumnNameGenerator::generateNameForColumn(2) == "C");
	EXPECT(ColumnNameGenerator::generateNameForColumn(3) == "D");
	EXPECT(ColumnNameGenerator::generateNameForColumn(4) == "E");
}

CASE(TAG "Default column names are be generated correctly for the last few columns before letter duplication.")
{
	EXPECT(ColumnNameGenerator::generateNameForColumn(23) == "X");
	EXPECT(ColumnNameGenerator::generateNameForColumn(24) == "Y");
	EXPECT(ColumnNameGenerator::generateNameForColumn(25) == "Z");
}

CASE(TAG "Letters are duplicated for column names after Z.")
{
	EXPECT(ColumnNameGenerator::generateNameForColumn(26) == "AA");
	EXPECT(ColumnNameGenerator::generateNameForColumn(27) == "BB");
	EXPECT(ColumnNameGenerator::generateNameForColumn(28) == "CC");

	EXPECT(ColumnNameGenerator::generateNameForColumn(49) == "XX");
	EXPECT(ColumnNameGenerator::generateNameForColumn(50) == "YY");
	EXPECT(ColumnNameGenerator::generateNameForColumn(51) == "ZZ");

	EXPECT(ColumnNameGenerator::generateNameForColumn(52) == "AAA");
	EXPECT(ColumnNameGenerator::generateNameForColumn(53) == "BBB");
	EXPECT(ColumnNameGenerator::generateNameForColumn(54) == "CCC");

	EXPECT(ColumnNameGenerator::generateNameForColumn(75) == "XXX");
	EXPECT(ColumnNameGenerator::generateNameForColumn(76) == "YYY");
	EXPECT(ColumnNameGenerator::generateNameForColumn(77) == "ZZZ");

	EXPECT(ColumnNameGenerator::generateNameForColumn(78) == "AAAA");
}
