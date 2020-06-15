#include <catch2/catch.hpp>
#include <e/system/repr.h>
#include <vector>
TEST_CASE("Repr for int", "[repr]")
{
    CHECK(e::system::repr(123) == "123");
}

TEST_CASE("Repr for range", "[repr]")
{
    CHECK(e::system::repr(std::vector<int>{1, 2, 3}) == "{1, 2, 3}");
}

TEST_CASE("Repr for bool", "[repr]")
{
    CHECK(e::system::repr(true) == "true");
    CHECK(e::system::repr(false) == "false");
}

TEST_CASE("Repr for e::system::string", "[repr]")
{
    CHECK(e::system::repr(e::system::string("a\tb\rc\nd\fg\"")) == "\"a\\tb\\rc\\nd\\fg\\\"\"");
}

TEST_CASE("Repr for unknown types", "[repr]")
{
    struct {

    } x;
    CHECK(e::system::repr(x) == "[Unknown]");
}