#pragma once


namespace onest::category
{
	class Category final
	{
	private:
		friend class CategoryFactory;
		Category(unsigned char id) : myID(id)
		{}

	public:
		Category() : myID(0)
		{}

		bool operator==(const Category&) const = default;
		bool operator!=(const Category&) const = default;

	private:
		unsigned char myID;
	};
}
