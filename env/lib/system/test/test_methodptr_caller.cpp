#include <catch2/catch.hpp>
#include <e/system/any.h>
#include <e/system/methodptr_caller.h>

void __stdcall simpleMethodWithAStringParameter(const char * x)
{
    CHECK(strcmp(x, "test") == 0);
}

TEST_CASE("call methodptr with a string argument", "[methodptr_caller]")
{
    auto target = reinterpret_cast<void *>(&simpleMethodWithAStringParameter);
    e::system::MethodPtrCaller<void(e::system::string)> caller;
    caller.call(target, EOC_STR_CONST("test"));
}

TEST_CASE("call methodptr with a string argument (using any)", "[methodptr_caller]")
{
    auto target = reinterpret_cast<void *>(&simpleMethodWithAStringParameter);
    e::system::MethodPtrCaller<void(e::system::any)> caller;
    caller.call(target, e::system::any(EOC_STR_CONST("test")));
}