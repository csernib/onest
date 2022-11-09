#include "../src/calc/CategoryFactory.h"
#include "../src/calc/ONEST.h"
#include "../src/Exception.h"
#include "Test_ONEST_Helper.h"
#include "test.h"

#include <limits>


#define TAG "[ONEST] "

using namespace std;
using namespace onest::calc;

using onest::Exception;
using test::calculateAllPermutations;
using test::calculateRandomPermutations;

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

namespace onest::test
{
	unsigned factorial(unsigned n);
	number_t median(const vector<number_t>& input);
}

using onest::test::factorial;
using onest::test::median;

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
	const ONEST onest = calculateRandomPermutations(matrix, 2, RNG());

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
	const ONEST onest = calculateRandomPermutations(matrix, 0, RNG());

	// Then
	EXPECT(onest.empty());
}

CASE(TAG "Factorial calculation works correctly for small numbers.")
{
	EXPECT(factorial(0) == 1);
	EXPECT(factorial(1) == 1);
	EXPECT(factorial(2) == 2);
	EXPECT(factorial(3) == 6);
	EXPECT(factorial(4) == 24);
	EXPECT(factorial(5) == 120);
}

CASE(TAG "Factorial of a large number (that would cause the result type to overflow) is the maximum value possible.")
{
	EXPECT(factorial(-1) == numeric_limits<decltype(factorial(0))>::max());
	EXPECT(factorial(-2) == numeric_limits<decltype(factorial(0))>::max());
	EXPECT(factorial(numeric_limits<decltype(factorial(0))>::max() / 3) == numeric_limits<decltype(factorial(0))>::max());
}

CASE(TAG "Median calculation works correctly.")
{
	// Given
	vector<number_t> oddNumberedInput = { 7.0, 1.8, 2.0, 5.0, 3.5 };
	vector<number_t> evenNumberedInput = { 7.0, 1.0, 3.0, 5.0 };

	// When, then
	EXPECT(median(oddNumberedInput) == 3.5);
	EXPECT(median(evenNumberedInput) == 4.0);
}

CASE(TAG "ONEST simplification works correctly.")
{
	// Given
	const ONEST onest =
	{
		{ 7.0, 1.8, 2.0, 5.0, 3.4 },
		{ 5.0, 3.2, 1.8, 8.0, 3.0 },
		{ 1.0, 4.0, 1.2, 3.0, 4.0 },
		{ 8.0, 1.0, 3.0, 2.0, 4.5 }
	};

	// When
	const ONEST simplifiedONEST = simplifyONEST(onest);

	// Then
	const OPAC minimumOPAC = { 1.0, 1.0, 1.2, 2.0, 3.0 };
	const OPAC maximumOPAC = { 8.0, 4.0, 3.0, 8.0, 4.5 };
	const OPAC medianOPAC  = { 6.0, 2.5, 1.9, 4.0, 3.7 };

	EXPECT(simplifiedONEST.size() == 3);
	EXPECT(simplifiedONEST[0] == minimumOPAC);
	EXPECT(simplifiedONEST[1] == medianOPAC);
	EXPECT(simplifiedONEST[2] == maximumOPAC);
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

CASE(TAG "Attempting to calculate OPA(N) from empty ONEST throws.")
{
	// Given
	const ONEST emptyONEST;

	// When, then
	EXPECT_THROWS_AS(calculateOPAN(emptyONEST), Exception);
}

CASE(TAG "Bandwidth is calculated correctly.")
{
	// Given
	const AssessmentMatrix matrix = generateTestMatrix();
	const ONEST onest = calculateAllPermutations(matrix);

	// When
	const number_t bandwidth = calculateBandwidth(onest);

	// Then
	EXPECT(bandwidth == 0.50);
}

CASE(TAG "Attempting to calculate bandwidth from empty ONEST throws.")
{
	// Given
	const ONEST emptyONEST;

	// When, then
	EXPECT_THROWS_AS(calculateBandwidth(emptyONEST), Exception);
}

CASE(TAG "If ONEST is a constant line at 1.0, the number of observers needed is one.")
{
	// Given
	const ONEST onest = { { 1.0, 1.0, 1.0, 1.0, 1.0 } };

	// When
	const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);

	// Then
	EXPECT(observersNeeded.result == ObserversNeeded::CONVERGED_AND_DEFINED);
	EXPECT(observersNeeded.numOfObservers == 1);
	EXPECT(observersNeeded.opaValue == 1.0);
}

CASE(TAG "If there are two observers and they are in complete agreement, then the number of observers needed is just one.")
{
	// Given
	const ONEST onest = { { 1.0 } };

	// When
	const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);

	// Then
	EXPECT(observersNeeded.result == ObserversNeeded::CONVERGED_AND_DEFINED);
	EXPECT(observersNeeded.numOfObservers == 1);
	EXPECT(observersNeeded.opaValue == 1.0);
}

CASE(TAG "If there are two observers, but they are not in complete agreement, then the number of observers needed cannot be reliably calculated.")
{
	// Given
	const ONEST onest = { { 0.99 } };

	// When
	const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);

	// Then
	EXPECT(observersNeeded.result == ObserversNeeded::CONVERGED_BUT_UNKNOWN);
	EXPECT(observersNeeded.numOfObservers == 2);
	EXPECT(observersNeeded.opaValue == 0.99);
}

CASE(TAG "If there are two observers and they are in complete disagreement, then the result is divergent.")
{
	// Given
	const ONEST onest = { { 0.0 } };

	// When
	const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);

	// Then
	EXPECT(observersNeeded.result == ObserversNeeded::DIVERGED);
	EXPECT(observersNeeded.numOfObservers == 2);
	EXPECT(observersNeeded.opaValue == 0.0);
}

CASE(TAG "Even if the minimum OPAC decreases further after reaching an initial plateau, the number of observers needed is still calculated correctly.")
{
	// Given
	const ONEST onest =
	{
		{ 1.0, 0.9, 0.8, 0.7, 0.6, 0.5 },
		{ 1.0, 0.7, 0.7, 0.7, 0.5, 0.5 }
	};

	// When
	const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);

	// Then
	EXPECT(observersNeeded.result == ObserversNeeded::CONVERGED_AND_DEFINED);
	EXPECT(observersNeeded.numOfObservers == 6);
	EXPECT(observersNeeded.opaValue == 0.5);
}

CASE(TAG "If the minimum OPAC reaches 0.0, the result is divergent, thus the number of observers needed cannot be calculated.")
{
	// Given
	const ONEST onest =
	{
		{ 1.0, 0.9, 0.8, 0.7 },
		{ 0.9, 0.8, 0.0, 0.0 }
	};

	// When
	const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);

	// Then
	EXPECT(observersNeeded.result == ObserversNeeded::DIVERGED);
	EXPECT(observersNeeded.numOfObservers == 4);
	EXPECT(observersNeeded.opaValue == 0.0);
}

CASE(TAG "If the minimum OPAC does not reach a plateau, there is not enough data to reliably calculate the number of observers needed.")
{
	// Given
	const ONEST onest =
	{
		{ 1.0, 0.9, 0.8, 0.8, 0.8, 0.7 },
		{ 1.0, 0.9, 0.9, 0.9, 0.9, 0.7 }
	};

	// When
	const ObserversNeeded observersNeeded = calculateObserversNeeded(onest);

	// Then
	EXPECT(observersNeeded.result == ObserversNeeded::CONVERGED_BUT_UNKNOWN);
	EXPECT(observersNeeded.numOfObservers == 7);
	EXPECT(observersNeeded.opaValue == 0.7);
}

CASE(TAG "Attempting to calculate observers needed from empty ONEST throws.")
{
	// Given
	const ONEST emptyONEST;

	// When, then
	EXPECT_THROWS_AS(calculateObserversNeeded(emptyONEST), Exception);
}
