#pragma once

#include "Category.h"

#include <vector>


namespace onest::calc
{
	class AssessmentMatrix final
	{
	private:
		typedef std::vector<Category> Matrix;

	public:
		explicit AssessmentMatrix(unsigned numberOfObservers, unsigned numberOfCases)
			: myMatrix(numberOfObservers * numberOfCases)
			, myNumberOfObservers(numberOfObservers)
		{}

		void set(unsigned observerIndex, unsigned caseIndex, Category assessment)
		{
			myMatrix[convertToLinearAddress(observerIndex, caseIndex)] = assessment;
		}

		const Category& get(unsigned observerIndex, unsigned caseIndex) const
		{
			return myMatrix[convertToLinearAddress(observerIndex, caseIndex)];
		}

		unsigned getTotalNumberOfObservers() const { return static_cast<unsigned>(myNumberOfObservers); }
		unsigned getTotalNumberOfCases() const { return static_cast<unsigned>(myMatrix.size() / myNumberOfObservers); }

	private:
		size_t convertToLinearAddress(unsigned observerIndex, unsigned caseIndex) const
		{
			return caseIndex * myNumberOfObservers + observerIndex;
		}

		Matrix myMatrix;
		size_t myNumberOfObservers;
	};
}
