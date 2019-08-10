#include <catch2/catch.hpp>
#include <e/system/native_string.h>
TEST_CASE("native string", "[native_string]")
{
    CHECK(wcscmp(e::system::ToNativeWideString("世界您好").get(), L"世界您好") == 0);
    CHECK(strcmp(e::system::ToNativeUtf8String("世界您好").get(), u8"世界您好") == 0);
}