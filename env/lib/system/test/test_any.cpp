#include <catch2/catch.hpp>
#include <e/system/any.h>

TEST_CASE("Runtime Any", "[any]")
{
    e::system::any x;
    CHECK_FALSE(x.has_value());
    CHECK(x.type() == typeid(void));

    x = int32_t(32);
    CHECK(x.has_value());
    CHECK(x.type() == typeid(int32_t));
    CHECK(x.cast<int32_t>() == 32);

    e::system::any y;
    y = x;
    CHECK(y.has_value());
    CHECK(y.type() == typeid(int32_t));
    CHECK(y.cast<int32_t>() == 32);

    //No implicit conversion
    CHECK_THROWS_AS(y.cast<int32_t *>(), e::system::bad_any_cast);
    CHECK_THROWS_AS(y.cast<char>(), e::system::bad_any_cast);
    CHECK_THROWS_AS(y.cast<int64_t>(), e::system::bad_any_cast);

    y.reset();
    CHECK_FALSE(y.has_value());
    CHECK(y.type() == typeid(void));
    //Not affect x
    CHECK(x.cast<int32_t>() == 32);

    x = EOC_STR_CONST("Hello World");
    CHECK(x + EOC_STR_CONST(".www") == EOC_STR_CONST("Hello World.www"));

    CHECK(((e::system::any(123) & 456 ^ 789 | 996) << 1) - 39 == 2003);
}