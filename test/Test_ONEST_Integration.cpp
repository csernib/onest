#include "../src/calc/CategoryFactory.h"
#include "../src/calc/ONEST.h"
#include "../src/csv/Parser.h"
#include "../src/io/File.h"
#include "../src/rule/Categorizer.h"
#include "Test_ONEST_Helper.h"
#include "test.h"

#include <algorithm>
#include <ranges>


#define TAG "[ONEST] [Integration] "

using namespace std;
using onest::calc::AssessmentMatrix;
using onest::calc::Category;
using onest::calc::CategoryFactory;
using onest::calc::ONEST;
using onest::csv::parseSheet;
using onest::csv::Row;
using onest::csv::Sheet;
using onest::io::File;
using onest::rule::Categorizer;
using std::filesystem::path;
using test::calculateAllPermutations;
using test::testResourcesFolder;

CASE(TAG "Given an input CSV file, calculating ONEST with all permutations yields the expected result.")
{
	// Given
	const path inputFilePath = testResourcesFolder / "onest_input.csv";
	const string inputCSVText = File::readFileAsString(inputFilePath);
	const Sheet inputSheet = parseSheet(inputCSVText, ',', '"');

	const path expectedResultFilePath = testResourcesFolder / "onest_output.csv";
	const string expectedCSVText = File::readFileAsString(expectedResultFilePath);
	const Sheet expectedSheet = parseSheet(expectedCSVText, ',', '"');

	ONEST expectedONEST;
	auto stringToDouble = views::transform([](const string& s) { return stod(s); });
	for (const Row& row : expectedSheet)
	{
		auto convertedRow = row | stringToDouble;
		expectedONEST.emplace_back(convertedRow.begin(), convertedRow.end());
	}

	const Categorizer categorizer("0<=X<=0.3;0.3<=X<=0.5;0.5<X");

	EXPECT(inputSheet.size() == 50);
	for (const Row& row : inputSheet)
		EXPECT(row.size() == 5);

	AssessmentMatrix matrix((unsigned)inputSheet[0].size(), (unsigned)inputSheet.size());
	CategoryFactory categoryFactory;

	for (unsigned caseIndex = 0; caseIndex < matrix.getTotalNumberOfCases(); ++caseIndex)
	{
		for (unsigned observerIndex = 0; observerIndex < matrix.getTotalNumberOfObservers(); ++observerIndex)
		{
			const Categorizer::Result categorizationResult = categorizer.categorize(inputSheet[caseIndex][observerIndex]);

			EXPECT(categorizationResult.success);

			const Category assessment = categoryFactory.createCategory(string(categorizationResult.category));
			matrix.set(observerIndex, caseIndex, assessment);
		}
	}

	// When
	ONEST onest = calculateAllPermutations(matrix);

	// Then
	EXPECT(onest.size() == expectedSheet.size());

	ranges::sort(onest);
	ranges::sort(expectedONEST);

	EXPECT(onest == expectedONEST);
}
