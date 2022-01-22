#include "../src/category/DiscreteCategoryMatcher.h"
#include "../src/category/RangeBasedCategoryMatcher.h"

#include "../src/category/CategoryFactory.cpp"
#include "test.h"

#include <limits>


#define TAG "[CategoryFactory] "

using namespace std;
using onest::category::Category;
using onest::category::CategoryFactory;
using onest::category::CategoryMatcher;
using onest::category::DiscreteCategoryMatcher;
using onest::category::RangeBasedCategoryMatcher;
using onest::Exception;

namespace
{
	unsigned char extractCategory(Category c)
	{
		return *reinterpret_cast<unsigned char*>(&c);
	}

	template<class... Args>
	CategoryFactory createCategoryFactory(Args&&... args)
	{
		vector<unique_ptr<CategoryMatcher>> v;
		v.reserve(sizeof...(args));
		int dummy[] = { (v.emplace_back(make_unique<Args>(forward<Args>(args))), 0)... };
		return v;
	}
}

CASE(TAG "Using discrete category matching works.")
{
	// Given
	CategoryFactory factory = createCategoryFactory(DiscreteCategoryMatcher("a"), DiscreteCategoryMatcher("b"));

	// When
	Category a = factory.createCategory("a");
	Category b = factory.createCategory("b");

	// Then
	EXPECT(extractCategory(a) == 0);
	EXPECT(extractCategory(b) == 1);

	// Except
	EXPECT_THROWS_AS(factory.createCategory("c"), Exception);
}

CASE(TAG "Using range-based category matching works.")
{
	// Given
	CategoryFactory factory = createCategoryFactory(RangeBasedCategoryMatcher(0.0f, 1.0f), RangeBasedCategoryMatcher(2.0f, 3.0f));

	// When
	Category a = factory.createCategory("0.3");
	Category b = factory.createCategory("2.45");
	Category c = factory.createCategory("2");

	// Then
	EXPECT(extractCategory(a) == 0);
	EXPECT(extractCategory(b) == 1);
	EXPECT(extractCategory(c) == 1);

	// Except
	EXPECT_THROWS_AS(factory.createCategory("1.3"), Exception);
}

CASE(TAG "Matching is done in order.")
{
	// Given
	CategoryFactory factory = createCategoryFactory(
		RangeBasedCategoryMatcher(0.0f, 1.0f),
		RangeBasedCategoryMatcher(1.0f, 3.0f),
		DiscreteCategoryMatcher("a"),
		DiscreteCategoryMatcher("1"),
		DiscreteCategoryMatcher("2"),
		DiscreteCategoryMatcher("4"),
		RangeBasedCategoryMatcher(4.0f, 4.5f)
	);

	// When
	Category a = factory.createCategory("0");
	Category b = factory.createCategory("1");
	Category c = factory.createCategory("1.0");
	Category d = factory.createCategory("1.3");
	Category e = factory.createCategory("2");
	Category f = factory.createCategory("4");
	Category g = factory.createCategory("4.01");
	Category h = factory.createCategory("a");

	// Then
	EXPECT(extractCategory(a) == 0);
	EXPECT(extractCategory(b) == 0);
	EXPECT(extractCategory(c) == 0);
	EXPECT(extractCategory(d) == 1);
	EXPECT(extractCategory(e) == 1);
	EXPECT(extractCategory(f) == 5);
	EXPECT(extractCategory(g) == 6);
	EXPECT(extractCategory(h) == 2);
}

CASE(TAG "Having too many matchers causes an exception.")
{
	// Given
	vector<unique_ptr<CategoryMatcher>> matchers;

	constexpr size_t intentionalOverflow = (size_t)numeric_limits<unsigned char>::max() + 2;
	generate_n(back_inserter(matchers), intentionalOverflow, [] { return make_unique<DiscreteCategoryMatcher>("a"); });

	// When, then
	EXPECT_THROWS_AS(CategoryFactory(move(matchers)), Exception);
}
