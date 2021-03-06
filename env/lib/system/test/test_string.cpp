﻿#include <catch2/catch.hpp>
#include <e/system/string.h>

e::system::string getAConstString()
{
    return EOC_STR_CONST("123");
}

TEST_CASE("string", "[string]")
{
	SECTION("isEmpty")
	{
		CHECK_FALSE(EOC_STR_CONST("123456").isEmpty());
		CHECK(e::system::string(nullptr).isEmpty());
		CHECK(EOC_STR_CONST("").isEmpty());
	}

	SECTION("compare")
	{
		CHECK(EOC_STR_CONST("abc") == EOC_STR_CONST("abc"));
		CHECK(EOC_STR_CONST("123") >= EOC_STR_CONST("123"));
		CHECK(EOC_STR_CONST("456") <= EOC_STR_CONST("456"));

		CHECK(EOC_STR_CONST("ab") != EOC_STR_CONST("abc"));
		CHECK(EOC_STR_CONST("ab") < EOC_STR_CONST("abc"));
		CHECK(EOC_STR_CONST("ab") <= EOC_STR_CONST("abc"));
		CHECK(EOC_STR_CONST("abc") >= EOC_STR_CONST("ab"));
		CHECK(EOC_STR_CONST("abc") > EOC_STR_CONST("ab"));
		CHECK(EOC_STR_CONST("c") < EOC_STR_CONST("d"));
		CHECK(EOC_STR_CONST("c") < EOC_STR_CONST("de"));

		CHECK(e::system::string(nullptr) == EOC_STR_CONST(""));
		CHECK(e::system::string(nullptr) == e::system::string(nullptr));

		CHECK(EOC_STR_CONST("") < EOC_STR_CONST("a"));
		CHECK(e::system::string(nullptr) < EOC_STR_CONST("a"));
	}

	SECTION("len")
	{
		CHECK(e::system::string(nullptr).len() == 0);
		CHECK(EOC_STR_CONST("").len() == 0);
		CHECK(EOC_STR_CONST("asdfgh").len() == 6);
	}

	SECTION("combine")
	{
		CHECK(EOC_STR_CONST("520") + EOC_STR_CONST("1314") == EOC_STR_CONST("5201314"));
		CHECK(EOC_STR_CONST("") + EOC_STR_CONST("asd") == EOC_STR_CONST("asd"));
		CHECK(e::system::string(nullptr) + EOC_STR_CONST("def") == EOC_STR_CONST("def"));


		CHECK((EOC_STR_CONST("asd") + EOC_STR_CONST("fgh")) == EOC_STR_CONST("asdfgh"));
	}

	SECTION("copy and move")
	{
		e::system::string x = EOC_STR_CONST("asd");
		{
			e::system::string y = x;
			CHECK(y == EOC_STR_CONST("asd"));
		}
		CHECK(x == EOC_STR_CONST("asd"));

		e::system::string z = std::move(x);
		x = nullptr;
		CHECK(z == EOC_STR_CONST("asd"));

		e::system::string a = getAConstString();
        CHECK(a == EOC_STR_CONST("123"));
	}
}