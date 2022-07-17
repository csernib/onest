#include "../src/calc/CategoryFactory.h"
#include "../src/calc/ONEST.h"
#include "test.h"

#include <algorithm>


#define TAG "[ONEST] "

using namespace std;
using namespace onest::calc;

namespace
{
	AssessmentMatrix generateTestMatrix()
	{
		static const AssessmentMatrix matrix = []
		{
			CategoryFactory categoryFactory;

			const Category a = categoryFactory.createCategory("a");
			const Category b = categoryFactory.createCategory("b");

			AssessmentMatrix matrix(3, 4);

			matrix.set(0, 0, a);	matrix.set(1, 0, a);	matrix.set(2, 0, b);
			matrix.set(0, 1, a);	matrix.set(1, 1, a);	matrix.set(2, 1, b);
			matrix.set(0, 2, b);	matrix.set(1, 2, a);	matrix.set(2, 2, b);
			matrix.set(0, 3, a);	matrix.set(1, 3, a);	matrix.set(2, 3, a);

			return matrix;
		}();

		return matrix;
	}

	vector<OPAC> generateExpectedOPACs()
	{
		static const vector<OPAC> opacs = []
		{
			vector<OPAC> opacs;

			// 1, 2, 3
			opacs.push_back({ 0.75, 0.25 });

			// 1, 3, 2
			opacs.push_back({ 0.50, 0.25 });

			// 2, 1, 3
			opacs.push_back({ 0.75, 0.25 });

			// 2, 3, 1
			opacs.push_back({ 0.25, 0.25 });

			// 3, 1, 2
			opacs.push_back({ 0.50, 0.25 });

			// 3, 2, 1
			opacs.push_back({ 0.25, 0.25 });

			return opacs;
		}();

		return opacs;
	}
}

CASE(TAG "ONEST for all permutations yields the correct OPACs.")
{
	// Given
	const AssessmentMatrix matrix = generateTestMatrix();
	vector<OPAC> expectedOPACs = generateExpectedOPACs();

	// When
	ONEST onest = calculateAllPermutations(matrix);
	ranges::sort(onest);
	ranges::sort(expectedOPACs);

	// Then
	EXPECT(onest == expectedOPACs);
}

CASE(TAG "ONEST for random permutations contains the correct OPACs.")
{
	// Given
	const AssessmentMatrix matrix = generateTestMatrix();
	vector<OPAC> expectedOPACs = generateExpectedOPACs();

	// When
	const ONEST onest = calculateRandomPermutations(matrix, 2);

	// Then
	EXPECT(onest.size() == 2);
	EXPECT(ranges::find(expectedOPACs, onest[0]) != expectedOPACs.end());
	EXPECT(ranges::find(expectedOPACs, onest[1]) != expectedOPACs.end());
}

CASE(TAG "ONEST for 0 random permutations returns empty container.")
{
	// Given
	const AssessmentMatrix matrix = generateTestMatrix();

	// When
	const ONEST onest = calculateRandomPermutations(matrix, 0);

	// Then
	EXPECT(onest.empty());
}

CASE(TAG "OPA(N) is calculated correctly.")
{
	// Given
	const AssessmentMatrix matrix = generateTestMatrix();
	const ONEST onest = calculateAllPermutations(matrix);

	// When
	const number_t opan = calculateOPAN(onest);

	// Then
	EXPECT(opan == 0.25);
}
