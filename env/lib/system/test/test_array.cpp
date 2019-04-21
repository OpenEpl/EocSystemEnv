#include <catch2/catch.hpp>
#include <e/system/array.h>
#include <e/system/string.h>

TEST_CASE("1-dim array", "[array]")
{
	e::system::array<int> x(5);
	CHECK(x.GetSize() == 5);
	for (size_t i = 0; i < 5; i++)
	{
		x.At_CStyle(i) = (int)i + 1;
	}
	for (size_t i = 1; i <= 5; i++)
	{
		REQUIRE(x.At(i) == i);
	}

	SECTION("redim")
	{
		x.Redim(true, 6);
		CHECK(x.GetSize() == 6);
		for (size_t i = 0; i < 5; i++)
		{
			CHECK(x.At_CStyle(i) == i + 1);
		}

		x.Redim(false, 5);
		for (size_t i = 0; i < 5; i++)
		{
			CHECK(x.At_CStyle(i) == 0);
		}
	}

	SECTION("RedimWithDynamicRank")
	{
		{
			size_t idx[] = { 6 };
			x.RedimWithDynamicRank(true, sizeof(idx) / sizeof(idx[0]), &idx[0]);
			CHECK(x.GetSize() == 6);
			for (size_t i = 0; i < 5; i++)
			{
				CHECK(x.At_CStyle(i) == i + 1);
			}
		}

		{
			size_t idx[] = { 5 };
			x.RedimWithDynamicRank(false, sizeof(idx) / sizeof(idx[0]), &idx[0]);
			for (size_t i = 0; i < 5; i++)
			{
				CHECK(x.At_CStyle(i) == 0);
			}
		}
	}

	SECTION("invalid access")
	{
		CHECK_THROWS_AS(x.At_CStyle(0, 1), std::invalid_argument);
		CHECK_THROWS_AS(x.At_CStyle(6), std::out_of_range);
	}
}
TEST_CASE("compare array", "[array]")
{
	CHECK(e::system::array<int>{1, 2, 3, 4} == e::system::array<int>{1, 2, 3, 4});
	CHECK(e::system::array<int>{1, 2, 3, 4} != e::system::array<int>{1, 2, 3, 4, 5});
	CHECK(e::system::array<int>{1, 2, 3, 4} != e::system::array<int>{3, 4, 5, 6});

	e::system::array<int> x = { 1, 2, 3, 4 };
	x.Redim(true, 2, 2);
	CHECK(e::system::array<int>{1, 2, 3, 4} != x);
	CHECK(e::system::array<int>{1, 2, 3, 4, 5} != x);
	CHECK(x == x);

	CHECK(e::system::array<int>(0u) == e::system::array<int>(0u));
	CHECK(e::system::array<int>(nullptr) == e::system::array<int>(0u));
	CHECK(e::system::array<int>(nullptr) == e::system::array<int>(nullptr));

	CHECK(e::system::array<e::system::string>{"hello"} == e::system::array<e::system::string>{"hello"});

}
TEST_CASE("array size", "[array]")
{
	e::system::array<int> emptyArray(0u);
	e::system::array<int> nullArray(nullptr);
	CHECK(emptyArray.empty());
	CHECK(emptyArray.GetSize() == 0);
	CHECK(emptyArray.GetRank() == 1);
	CHECK(emptyArray.GetUBound(1) == 0);

	CHECK(nullArray.empty());
	CHECK(nullArray.GetSize() == 0);
	CHECK(nullArray.GetRank() == 1);
	CHECK(nullArray.GetUBound(1) == 0);

	e::system::array<int> x = { 1, 2, 3, 4 };
	CHECK_FALSE(x.empty());
	CHECK(x.GetRank() == 1);
	CHECK(x.GetSize() == 4);
	CHECK(x.GetUBound_CStyle(0) == 4);
	CHECK_THROWS_AS(x.GetUBound_CStyle(1), std::out_of_range);

	x.Redim(true, 2, 2);
	CHECK_FALSE(x.empty());
	CHECK(x.GetRank() == 2);
	CHECK(x.GetSize() == 4);
	CHECK_THROWS_AS(x.GetUBound(0), std::out_of_range);
	CHECK(x.GetUBound(1) == 2);
	CHECK(x.GetUBound(2) == 2);
	CHECK_THROWS_AS(x.GetUBound(3), std::out_of_range);
}
