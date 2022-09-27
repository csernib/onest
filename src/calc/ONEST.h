#pragma once

#include "AssessmentMatrix.h"

#include <random>


namespace onest::calc
{
	typedef double number_t;
	typedef std::vector<number_t> OPAC;
	typedef std::vector<OPAC> ONEST;

	ONEST calculateAllPermutations(const AssessmentMatrix& matrix);
	ONEST calculateRandomPermutations(const AssessmentMatrix& matrix, unsigned numberOfPermutations, std::mt19937_64 rng);

	number_t calculateOPAN(const ONEST& onest);
	number_t calculateBandwidth(const ONEST& onest);
	std::pair<number_t, number_t> calculateBandwidthMinMax(const ONEST& onest);
	unsigned calculateNumOfObserversNeeded(const ONEST& onest);
}
