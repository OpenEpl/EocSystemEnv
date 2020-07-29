#include <catch2/catch.hpp>
#include <e/system/datetime.h>
inline const e::system::datetime D_18990101_111213(-363.46681712962965);
inline const e::system::datetime D_20191231_050607(43830.21258101852);
inline const e::system::datetime D_20200101_050607(43831.21258101852);
inline const e::system::datetime D_20200102_050607(43832.21258101852);

TEST_CASE("build datetime with human-readable info", "[datetime]")
{
    CHECK(e::system::datetime(1899, 1, 1, 11, 12, 13) == D_18990101_111213);
    CHECK(e::system::datetime(2019, 12, 31, 5, 6, 7) == D_20191231_050607);
    CHECK(e::system::datetime(2020, 1, 1, 5, 6, 7) == D_20200101_050607);
    CHECK(e::system::datetime(2020, 1, 2, 5, 6, 7) == D_20200102_050607);
}

TEST_CASE("get date part of datetime", "[datetime]")
{
    int32_t year, month, day;
    D_18990101_111213.get_date_part(&year, &month, &day);
    CHECK(year == 1899);
    CHECK(month == 1);
    CHECK(day == 1);
    D_20191231_050607.get_date_part(&year, &month, &day);
    CHECK(year == 2019);
    CHECK(month == 12);
    CHECK(day == 31);
}

TEST_CASE("get time part of datetime", "[datetime]")
{
    int32_t hour, minute, second;
    D_18990101_111213.get_time_part(&hour, &minute, &second);
    CHECK(hour == 11);
    CHECK(minute == 12);
    CHECK(second == 13);
    D_20191231_050607.get_time_part(&hour, &minute, &second);
    CHECK(hour == 5);
    CHECK(minute == 6);
    CHECK(second == 7);
}

TEST_CASE("compare two datetimes", "[datetime]")
{
    CHECK(e::system::datetime(2020, 1, 2, 3, 4, 5) == e::system::datetime(2020, 1, 2, 3, 4, 5));
    CHECK(e::system::datetime(2020, 1, 2, 3, 4, 5) >= e::system::datetime(2020, 1, 2, 3, 4, 5));
    CHECK(e::system::datetime(2020, 1, 2, 3, 4, 5) <= e::system::datetime(2020, 1, 2, 3, 4, 5));

    CHECK(e::system::datetime(2020, 1, 2, 3, 4, 6) > e::system::datetime(2020, 1, 2, 3, 4, 5));
    CHECK(e::system::datetime(2020, 1, 2, 3, 4, 6) >= e::system::datetime(2020, 1, 2, 3, 4, 5));

    CHECK(e::system::datetime(2020, 1, 1, 3, 4, 6) < e::system::datetime(2020, 1, 2, 3, 4, 5));
    CHECK(e::system::datetime(2020, 1, 1, 3, 4, 6) <= e::system::datetime(2020, 1, 2, 3, 4, 5));

    CHECK(e::system::datetime(1899, 1, 2, 3, 4, 6) > e::system::datetime(1899, 1, 2, 3, 4, 5));
    CHECK(e::system::datetime(1899, 1, 2, 3, 4, 6) >= e::system::datetime(1899, 1, 2, 3, 4, 5));

    CHECK(e::system::datetime(1899, 1, 1, 3, 4, 6) < e::system::datetime(1899, 1, 2, 3, 4, 5));
    CHECK(e::system::datetime(1899, 1, 1, 3, 4, 6) <= e::system::datetime(1899, 1, 2, 3, 4, 5));

    CHECK(e::system::datetime(1899, 1, 1, 3, 4, 6) < e::system::datetime(2020, 1, 1, 3, 4, 6));
    CHECK(e::system::datetime(1899, 1, 1, 3, 4, 6) <= e::system::datetime(2020, 1, 1, 3, 4, 6));
}

TEST_CASE("calc day of year of datetime", "[datetime]")
{
    CHECK(D_18990101_111213.day_of_year() == 1);
    CHECK(D_20191231_050607.day_of_year() == 365);
}