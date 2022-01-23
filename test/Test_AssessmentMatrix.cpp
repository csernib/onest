#include "../src/AssessmentMatrix.h"
#include "../src/category/CategoryFactory.h"
#include "../src/category/DiscreteCategoryMatcher.h"
#include "test.h"


#define TAG "[AssessmentMatrix] "

using namespace std;
using onest::AssessmentMatrix;
using onest::category::Category;
using onest::category::CategoryFactory;
using onest::category::CategoryMatcher;
using onest::category::DiscreteCategoryMatcher;

CASE(TAG "Getting and setting matrix elements works.")
{
	// Given
	const unsigned numOfObservers = 3;
	const unsigned numOfCases = 4;

	AssessmentMatrix matrix(numOfObservers, numOfCases);

	vector<unique_ptr<CategoryMatcher>> matchers;
	matchers.emplace_back(make_unique<DiscreteCategoryMatcher>("a"));
	matchers.emplace_back(make_unique<DiscreteCategoryMatcher>("b"));
	matchers.emplace_back(make_unique<DiscreteCategoryMatcher>("c"));
	CategoryFactory factory(move(matchers));

	const char* matrixValues[numOfCases][numOfObservers] =
	{
		"b", "c", "a",
		"a", "b", "a",
		"a", "a", "b",
		"b", "b", "b"
	};

	// When
	for (unsigned i = 0; i < numOfCases; ++i)
	{
		for (unsigned j = 0; j < numOfObservers; ++j)
			matrix.set(j, i, factory.createCategory(matrixValues[i][j]));
	}

	// Then
	EXPECT(matrix.getTotalNumberOfCases() == numOfCases);
	EXPECT(matrix.getTotalNumberOfObservers() == numOfObservers);
	EXPECT(matrix.get(0, 0) == factory.createCategory("b"));
	EXPECT(matrix.get(0, 1) == factory.createCategory("a"));
	EXPECT(matrix.get(0, 2) == factory.createCategory("a"));
	EXPECT(matrix.get(0, 3) == factory.createCategory("b"));
	EXPECT(matrix.get(1, 0) == factory.createCategory("c"));
	EXPECT(matrix.get(1, 1) == factory.createCategory("b"));
	EXPECT(matrix.get(1, 2) == factory.createCategory("a"));
	EXPECT(matrix.get(1, 3) == factory.createCategory("b"));
	EXPECT(matrix.get(2, 0) == factory.createCategory("a"));
	EXPECT(matrix.get(2, 1) == factory.createCategory("a"));
	EXPECT(matrix.get(2, 2) == factory.createCategory("b"));
	EXPECT(matrix.get(2, 3) == factory.createCategory("b"));
}
