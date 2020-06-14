#include <catch2/catch.hpp>
#include <e/system/native_string.h>
TEST_CASE("native string", "[native_string]")
{
    auto normal = "HelloWorld";
    auto wide = L"HelloWorld";
    auto utf8 = u8"HelloWorld";
    CHECK(wcscmp(e::system::ToNativeWideString(normal).get(), wide) == 0);
    CHECK(strcmp(e::system::ToNativeUtf8String(normal).get(), utf8) == 0);
    CHECK(e::system::ReceiveNativeWideString(wide) == normal);
    CHECK(e::system::ReceiveNativeUtf8String(utf8) == normal);
}