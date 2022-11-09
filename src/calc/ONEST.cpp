#include "ONEST.h"

#include "../Exception.h"

#include <algorithm>
#include <cassert>
#include <optional>
#include <ranges>
#include <set>


using namespace std;

namespace
{
	using namespace onest::calc;

	class ThreadInterrupted {};

	typedef vector<unsigned> ObserverPermutation;

	ObserverPermutation generateFirstPermutation(unsigned numOfObservers)
	{
		ObserverPermutation permutation(numOfObservers);
		generate(permutation.begin(), permutation.end(), [i = 0]() mutable { return i++; });
		return permutation;
	}

	number_t calculateOPA(const stop_token& stoken, const AssessmentMatrix& matrix, const ObserverPermutation& permutation, size_t numOfObservers)
	{
		unsigned totalEqual = 0;
		for (unsigned i = 0; i < matrix.getTotalNumberOfCases(); ++i)
		{
			if (stoken.stop_requested())
				throw ThreadInterrupted();

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

		return static_cast<number_t>(totalEqual) / static_cast<number_t>(matrix.getTotalNumberOfCases());
	}

	OPAC calculateOPAC(const stop_token& stoken, const AssessmentMatrix& matrix, const ObserverPermutation& permutation)
	{
		const size_t totalNumberOfObservers = permutation.size();

		OPAC opac;
		opac.reserve(totalNumberOfObservers - 1);
		for (size_t i = 2; i <= totalNumberOfObservers; ++i)
			opac.emplace_back(calculateOPA(stoken, matrix, permutation, i));

		return opac;

	}

	template<class Permutator>
	ONEST calculateONEST(const stop_token& stoken, const AssessmentMatrix& matrix, Permutator&& permutator)
	{
		ONEST onest;
		while (optional<ObserverPermutation> permutation = permutator())
			onest.emplace_back(calculateOPAC(stoken, matrix, *permutation));

		return onest;
	}

	constexpr unsigned factorial(unsigned n)
	{
		if (n <= 1)
			return 1;

		if (n == numeric_limits<unsigned>::max())
			return numeric_limits<unsigned>::max();

		const unsigned previousFactorial = factorial(n - 1);
		const unsigned currentFactorial = n * previousFactorial;
		if (currentFactorial / n != previousFactorial)
			return numeric_limits<unsigned>::max();

		return currentFactorial;
	}

	auto mapToOPAsAtObserverCountIndex(const ONEST& onest, size_t observerCountIndex)
	{
		return views::transform(onest, [=](const OPAC& opac) { return opac[observerCountIndex]; });
	}

	template<ranges::range R>
	number_t median(R&& range)
	{
		assert(!range.empty());

		vector opas(range.begin(), range.end());
		ranges::sort(opas);
		const size_t s = opas.size();
		return s % 2 == 1 ? opas[s / 2] : (opas[s / 2 - 1] + opas[s / 2]) / 2.0;
	}

	OPAC calculateMinOPAC(const ONEST& onest)
	{
		assert(!onest.empty());

		OPAC minOPAC;
		minOPAC.reserve(onest[0].size());
		for (size_t i = 0; i < onest[0].size(); ++i)
			minOPAC.push_back(ranges::min(mapToOPAsAtObserverCountIndex(onest, i)));

		return minOPAC;
	}

	ONEST calculateAllPermutationsInThread(const stop_token& stoken, const AssessmentMatrix& matrix)
	{
		if (matrix.getTotalNumberOfObservers() == 1)
			return ONEST{ { number_t(1.0) } };

		ObserverPermutation permutation = generateFirstPermutation(matrix.getTotalNumberOfObservers());
		return calculateONEST(stoken, matrix, [&, hasNext = true]() mutable -> optional<ObserverPermutation>
		{
			if (!hasNext)
				return {};

			hasNext = next_permutation(permutation.begin(), permutation.end());
			return permutation;
		});
	}

	ONEST calculateRandomPermutationsInThread(const stop_token& stoken, const AssessmentMatrix& matrix, unsigned numberOfPermutations, RNG rng)
	{
		if (numberOfPermutations == 0)
			return ONEST();

		if (numberOfPermutations >= factorial(matrix.getTotalNumberOfObservers()))
			return calculateAllPermutationsInThread(stoken, matrix);

		set<ObserverPermutation> randomPermutations;
		ObserverPermutation permutation = generateFirstPermutation(matrix.getTotalNumberOfObservers());
		return calculateONEST(stoken, matrix, [&, i = 0u]() mutable -> optional<ObserverPermutation>
		{
			if (i++ < numberOfPermutations)
			{
				do
				{
					shuffle(permutation.begin(), permutation.end(), rng);
				}
				while (randomPermutations.find(permutation) != randomPermutations.end());

				randomPermutations.emplace(permutation);
				return permutation;
			}

			return {};
		});
	}
}

namespace onest::test
{
	number_t median(const vector<number_t>& input)
	{
		return ::median(input);
	}
}

namespace onest::calc
{
	jthread calculateAllPermutations(AssessmentMatrix matrix, SuccessCallback onSuccess, ErrorCallback onError)
	{
		return jthread([
			matrix = move(matrix),
			onSuccess = move(onSuccess),
			onError = move(onError)
		](stop_token stoken)
		{
			ONEST result;
			try
			{
				result = calculateAllPermutationsInThread(stoken, matrix);
			}
			catch (const ThreadInterrupted&)
			{
				return;
			}
			catch (...)
			{
				onError(current_exception());
				return;
			}

			onSuccess(move(result));
		});
	}

	jthread calculateRandomPermutations(AssessmentMatrix matrix, unsigned numberOfPermutations, RNG rng, SuccessCallback onSuccess, ErrorCallback onError)
	{
		return jthread([
			matrix = move(matrix),
			numberOfPermutations,
			rng = move(rng),
			onSuccess = move(onSuccess),
			onError = move(onError)
		](stop_token stoken) mutable
		{
			ONEST result;
			try
			{
				result = calculateRandomPermutationsInThread(stoken, matrix, numberOfPermutations, move(rng));
			}
			catch (const ThreadInterrupted&)
			{
				return;
			}
			catch (...)
			{
				onError(current_exception());
				return;
			}

			onSuccess(move(result));
		});
	}

	ONEST simplifyONEST(const ONEST& onest)
	{
		if (onest.empty())
			return ONEST();

		const size_t opacSize = onest[0].size();

		OPAC minOPAC, maxOPAC, medianOPAC;
		minOPAC.reserve(opacSize);
		maxOPAC.reserve(opacSize);
		medianOPAC.reserve(opacSize);
		for (size_t i = 0; i < opacSize; ++i)
		{
			const auto opasAtIndex = mapToOPAsAtObserverCountIndex(onest, i);
			minOPAC.push_back(ranges::min(opasAtIndex));
			maxOPAC.push_back(ranges::max(opasAtIndex));
			medianOPAC.push_back(median(opasAtIndex));
		}

		return { minOPAC, medianOPAC, maxOPAC };
	}

	number_t calculateOPAN(const ONEST& onest)
	{
		if (onest.empty())
			throw Exception("OPA(N) is undefined for empty ONEST");

		const OPAC& anyOPAC = onest[0];
		return anyOPAC[anyOPAC.size() - 1];
	}

	number_t calculateBandwidth(const ONEST& onest)
	{
		auto [min, max] = calculateBandwidthMinMax(onest);
		return max - min;
	}

	pair<number_t, number_t> calculateBandwidthMinMax(const ONEST& onest)
	{
		if (onest.empty())
			throw Exception("Bandwidth is undefined for empty ONEST.");

		auto [min, max] = ranges::minmax(mapToOPAsAtObserverCountIndex(onest, 0));
		return { min, max };
	}

	ObserversNeeded calculateObserversNeeded(const ONEST& onest)
	{
		if (onest.empty())
			throw Exception("Number of observers needed is undefined for empty ONEST.");

		const OPAC minOPAC = calculateMinOPAC(onest);

		if (minOPAC[minOPAC.size() - 1] == 1.0)
			return { ObserversNeeded::CONVERGED_AND_DEFINED, 1, minOPAC[0] };

		if (minOPAC.size() == 1 && minOPAC[0] == 0.0)
			return { ObserversNeeded::DIVERGED, 2, minOPAC[0] };

		unsigned firstIndex = -1;
		for (unsigned i = 1; i < (unsigned)minOPAC.size(); ++i)
		{
			if (minOPAC[i] == 0.0)
				return { ObserversNeeded::DIVERGED, i + 2, minOPAC[i] };

			if (minOPAC[i] >= minOPAC[i - 1])
			{
				if (firstIndex == -1)
					firstIndex = i;
			}
			else
			{
				firstIndex = -1;
			}
		}

		if (firstIndex != -1)
			return { ObserversNeeded::CONVERGED_AND_DEFINED, firstIndex + 1, minOPAC[firstIndex - 1] };

		return { ObserversNeeded::CONVERGED_BUT_UNKNOWN, (unsigned)minOPAC.size() + 1, minOPAC[minOPAC.size() - 1] };
	}
}
