#include "ONEST.h"

#include "../Exception.h"

#include <algorithm>
#include <random>
#include <set>


using namespace std;

namespace
{
	using namespace onest::calc;

	typedef vector<unsigned> ObserverPermutation;

	ObserverPermutation generateFirstPermutation(unsigned numOfObservers)
	{
		ObserverPermutation permutation(numOfObservers);
		generate(permutation.begin(), permutation.end(), [i = 0]() mutable { return i++; });
		return permutation;
	}

	number_t calculateOPA(const AssessmentMatrix& matrix, const ObserverPermutation& permutation, size_t numOfObservers)
	{
		unsigned totalEqual = 0;
		for (unsigned i = 0; i < matrix.getTotalNumberOfCases(); ++i)
		{
			for (unsigned j = 0; j < numOfObservers - 1; ++j)
			{
				// No need to compare every category to every other category in this permutation.
				// Example: If the categories are "AAAA", it is impossible to change any <4 number of
				// "A"s to "B"s and not have it detected by this loop. If all four are changed to "B"
				// then there is nolonger a difference.
				if (matrix.get(permutation[j], i) != matrix.get(permutation[j + 1], i))
					goto continue_outer_loop;
			}

			++totalEqual;

		continue_outer_loop:
			;
		}

		return static_cast<double>(totalEqual) / static_cast<double>(matrix.getTotalNumberOfCases());
	}

	OPAC calculateOPAC(const AssessmentMatrix& matrix, const ObserverPermutation& permutation)
	{
		const size_t totalNumberOfObservers = permutation.size();

		OPAC opac;
		opac.reserve(totalNumberOfObservers - 1);
		for (size_t i = 2; i <= totalNumberOfObservers; ++i)
			opac.emplace_back(calculateOPA(matrix, permutation, i));

		return opac;

	}

	ONEST calculateONEST(const AssessmentMatrix& matrix, set<ObserverPermutation> observerPermutations)
	{
		ONEST onest;
		for (const ObserverPermutation& permutation : observerPermutations)
			onest.emplace_back(calculateOPAC(matrix, permutation));

		return onest;
	}

	constexpr unsigned factorial(unsigned n)
	{
		return n <= 1 ? 1 : n * factorial(n - 1);
	}
}

namespace onest::calc
{
	ONEST calculateAllPermutations(const AssessmentMatrix& matrix)
	{
		if (matrix.getTotalNumberOfObservers() == 1)
			return ONEST{ { number_t(1.0) } };

		set<ObserverPermutation> allPermutations;

		ObserverPermutation permutation = generateFirstPermutation(matrix.getTotalNumberOfObservers());
		do
		{
			allPermutations.insert(permutation);
		}
		while (next_permutation(permutation.begin(), permutation.end()));

		return calculateONEST(matrix, move(allPermutations));
	}

	ONEST calculateRandomPermutations(const AssessmentMatrix& matrix, unsigned numberOfPermutations)
	{
		if (numberOfPermutations == 0)
			return ONEST();

		if (numberOfPermutations >= factorial(matrix.getTotalNumberOfObservers()))
			return calculateAllPermutations(matrix);

		set<ObserverPermutation> randomPermutations;

		mt19937_64 rng;
		ObserverPermutation permutation = generateFirstPermutation(matrix.getTotalNumberOfObservers());
		for (unsigned i = 0; i < numberOfPermutations; ++i)
		{
			do
			{
				shuffle(permutation.begin(), permutation.end(), rng);
			}
			while (randomPermutations.find(permutation) != randomPermutations.end());

			randomPermutations.emplace(permutation);
		}

		return calculateONEST(matrix, move(randomPermutations));
	}

	number_t calculateOPAN(const ONEST& onest)
	{
		const OPAC& anyOPAC = onest[0];
		return anyOPAC[anyOPAC.size() - 1];
	}
}
