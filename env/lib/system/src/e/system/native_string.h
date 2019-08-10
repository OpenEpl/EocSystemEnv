#pragma once
#include "basic_type.h"
#include <memory>
namespace e
{
	namespace system
	{
        std::unique_ptr<wchar_t[]> ToNativeWideString(const e::system::string &x);
        std::unique_ptr<char[]> ToNativeUtf8String(const e::system::string &x);
    }
}