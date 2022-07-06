#include "../src/calc/CategoryFactory.h"
#include "../src/Exception.h"
#include "test.h"

#include <limits>


#define TAG "[CategoryFactory] "

using namespace std;
using onest::calc::Category;
using onest::calc::CategoryFactory;
using onest::Exception;

namespace
{
	unsigned char extractCategory(Category c)
	{
		return *reinterpret_cast<unsigned char*>(&c);
	}
}

CASE(TAG "Default-constructed category has zero ID.")
{
	// Given
	Category defaultConstructedCategory;

	// Then
	EXPECT(extractCategory(defaultConstructedCategory) == 0);
}

CASE(TAG "Using discrete category matching works.")
{
	// Given
	CategoryFactory factory;

	// When
	Category a1 = factory.createCategory("a");
	Category b1 = factory.createCategory("b");
	Category b2 = factory.createCategory("b");

	// Then
	EXPECT(a1 != b1);
	EXPECT(b1 == b2);
	EXPECT(extractCategory(a1) == 0);
	EXPECT(extractCategory(b1) == 1);
	EXPECT(extractCategory(b2) == 1);
}

CASE(TAG "Having too many categories causes an exception.")
{
	// Given
	CategoryFactory factory;

	constexpr size_t limitBeforeOverflow = (size_t)numeric_limits<unsigned char>::max() + 1;
	for (size_t i = 0; i < limitBeforeOverflow; ++i)
		factory.createCategory("not yet overflowing " + to_string(i));

	// When, then
	EXPECT_THROWS_AS(factory.createCategory("will overflow"), Exception);
}

CASE(TAG "Category text can be found from category.")
{
	// Given
	CategoryFactory factory;
	const string providedText = "abc";
	Category category = factory.createCategory(providedText);

	// When
	const string foundText = factory.findCategoryText(category);

	// Then
	EXPECT(foundText == providedText);
}
