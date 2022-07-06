#pragma once


namespace onest::calc
{
	class Category final
	{
	private:
		friend class CategoryFactory;
		typedef unsigned char idtype;
		Category(idtype id) : myID(id)
		{}

	public:
		Category() : myID(0)
		{}

		bool operator==(const Category&) const = default;
		bool operator!=(const Category&) const = default;

	private:
		idtype myID;
	};
}
