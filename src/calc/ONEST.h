#pragma once

#include "AssessmentMatrix.h"

#include <exception>
#include <functional>
#include <random>
#include <thread>


namespace onest::calc
{
	typedef double number_t;
	typedef std::vector<number_t> OPAC;
	typedef std::vector<OPAC> ONEST;

	typedef std::function<void(ONEST)> SuccessCallback;
	typedef std::function<void(std::exception_ptr)> ErrorCallback;
	typedef std::mt19937_64 RNG;

	struct ObserversNeeded
	{
		enum
		{
			CONVERGED_AND_DEFINED,
			CONVERGED_BUT_UNKNOWN,
			DIVERGED
		} result;

		unsigned numOfObservers;
		number_t opaValue;
	};

	std::jthread calculateAllPermutations(AssessmentMatrix matrix, SuccessCallback onSuccess, ErrorCallback onError);

	std::jthread calculateRandomPermutations(
		AssessmentMatrix matrix,
		unsigned numberOfPermutations,
		RNG rng,
		SuccessCallback onSuccess,
		ErrorCallback onError
	);

	ONEST simplifyONEST(const ONEST& onest);

	number_t calculateOPAN(const ONEST& onest);
	number_t calculateBandwidth(const ONEST& onest);
	std::pair<number_t, number_t> calculateBandwidthMinMax(const ONEST& onest);
	ObserversNeeded calculateObserversNeeded(const ONEST& onest);
}
