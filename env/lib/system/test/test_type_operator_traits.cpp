#include <catch2/catch.hpp>
#include <e/system/type_operator_traits.h>
#include <iostream>
using namespace e::system;
TEST_CASE("type operator traits", "[type_operator_traits]")
{
    SECTION("add")
    {
        CHECK(has_add_operator_v<int, int>);
        CHECK(has_add_operator_v<int, char>);
        CHECK(has_add_operator_v<int, long long>);
        CHECK(has_add_operator_v<int, double>);
        CHECK_FALSE(has_add_operator_v<nullptr_t, int>);
    }

    SECTION("sub")
    {
        CHECK(has_sub_operator_v<int, double>);
        CHECK_FALSE(has_sub_operator_v<nullptr_t, int>);
    }

    SECTION("mul")
    {
        CHECK(has_mul_operator_v<int, double>);
        CHECK_FALSE(has_mul_operator_v<nullptr_t, int>);
    }

    SECTION("div")
    {
        CHECK(has_div_operator_v<int, double>);
        CHECK_FALSE(has_div_operator_v<nullptr_t, int>);
    }

    SECTION("mod")
    {
        CHECK(has_mod_operator_v<int, long long>);
        CHECK_FALSE(has_mod_operator_v<int, double>);
        CHECK_FALSE(has_mod_operator_v<nullptr_t, int>);
    }

    SECTION("eq")
    {
        CHECK(has_eq_operator_v<int, double>);
        CHECK_FALSE(has_eq_operator_v<nullptr_t, int>);
    }

    SECTION("ne")
    {
        CHECK(has_ne_operator_v<int, double>);
        CHECK_FALSE(has_ne_operator_v<nullptr_t, int>);
    }

    SECTION("lt")
    {
        CHECK(has_lt_operator_v<int, double>);
        CHECK_FALSE(has_lt_operator_v<nullptr_t, int>);
    }

    SECTION("le")
    {
        CHECK(has_le_operator_v<int, double>);
        CHECK_FALSE(has_le_operator_v<nullptr_t, int>);
    }

    SECTION("gt")
    {
        CHECK(has_gt_operator_v<int, double>);
        CHECK_FALSE(has_gt_operator_v<nullptr_t, int>);
    }

    SECTION("ge")
    {
        CHECK(has_ge_operator_v<int, double>);
        CHECK_FALSE(has_ge_operator_v<nullptr_t, int>);
    }

    SECTION("neg")
    {
        CHECK(has_neg_operator_v<int>);
        CHECK_FALSE(has_neg_operator_v<nullptr_t>);
    }

    SECTION("bit_and")
    {
        CHECK(has_bit_and_operator_v<int, int>);
        CHECK_FALSE(has_bit_and_operator_v<double, double>);
    }

    SECTION("bit_or")
    {
        CHECK(has_bit_or_operator_v<int, int>);
        CHECK_FALSE(has_bit_or_operator_v<double, double>);
    }

    SECTION("bit_xor")
    {
        CHECK(has_bit_xor_operator_v<int, int>);
        CHECK_FALSE(has_bit_xor_operator_v<double, double>);
    }

    SECTION("bit_not")
    {
        CHECK(has_bit_not_operator_v<int>);
        CHECK_FALSE(has_bit_not_operator_v<double>);
    }

    SECTION("shift_left")
    {
        CHECK(has_shift_left_operator_v<int, int>);
        CHECK(has_shift_left_operator_v<std::iostream, int>);
        CHECK_FALSE(has_shift_left_operator_v<double, int>);
    }

    SECTION("shift_right")
    {
        CHECK(has_shift_right_operator_v<int, int>);
        CHECK_FALSE(has_shift_right_operator_v<std::iostream, int>);
        CHECK_FALSE(has_shift_right_operator_v<double, int>);
    }
}