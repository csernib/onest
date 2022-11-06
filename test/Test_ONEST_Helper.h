#include "../src/calc/CategoryFactory.h"
#include "../src/calc/ONEST.h"

#include <algorithm>
#include <future>


namespace test
{
	template<class AsyncFunction, class... Args>
	onest::calc::ONEST synchronize(AsyncFunction asyncFunction, Args&&... args)
	{
		std::promise<onest::calc::ONEST> promise;
		std::jthread thread = asyncFunction(
			std::forward<Args>(args)...,
			[&](onest::calc::ONEST onest) { promise.set_value(move(onest)); },
			[&](std::exception_ptr e) { promise.set_exception(e); }
		);
		thread.join();
		return promise.get_future().get();
	}

	inline onest::calc::ONEST calculateAllPermutations(const onest::calc::AssessmentMatrix& matrix)
	{
		return synchronize(onest::calc::calculateAllPermutations, matrix);
	}

	inline onest::calc::ONEST calculateRandomPermutations(
		const onest::calc::AssessmentMatrix& matrix,
		unsigned numberOfPermutations,
		onest::calc::RNG rng
	)
	{
		return synchronize(onest::calc::calculateRandomPermutations, matrix, numberOfPermutations, rng);
	}
}
