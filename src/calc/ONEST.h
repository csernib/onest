#pragma once

#include "AssessmentMatrix.h"


namespace onest::calc
{
	typedef double number_t;
	typedef std::vector<number_t> OPAC;
	typedef std::vector<OPAC> ONEST;

	ONEST calculateAllPermutations(const AssessmentMatrix& matrix);
	ONEST calculateRandomPermutations(const AssessmentMatrix& matrix, unsigned numberOfPermutations);

	number_t calculateOPAN(const ONEST& onest);
	number_t calculateBandwidth(const ONEST& onest);
	unsigned calculateNumOfObserversNeeded(const ONEST& onest);
}
