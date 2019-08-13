#include "native_string.h"
#include "Windows.h"
#include <codecvt>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4267)
#endif
std::unique_ptr<wchar_t[]> e::system::ToNativeWideString(const e::system::string &x)
{
    if (!x.data)
        return nullptr;
    auto len = x.len() + 1;
    auto wideLen = MultiByteToWideChar(CP_ACP, 0, x.c_str(), len, nullptr, 0);
    auto result = std::make_unique<wchar_t[]>(wideLen);
    MultiByteToWideChar(CP_ACP, 0, x.c_str(), len, result.get(), wideLen);
    return result;
}

std::unique_ptr<char[]> e::system::ToNativeUtf8String(const e::system::string &x)
{
    auto wideString = ToNativeWideString(x);
    if (!wideString)
        return nullptr;
    auto wideLen = wcslen(wideString.get()) + 1;
    auto len = WideCharToMultiByte(CP_UTF8, 0, wideString.get(), wideLen, nullptr, 0, nullptr, nullptr);
    auto result = std::make_unique<char[]>(len);
    WideCharToMultiByte(CP_UTF8, 0, wideString.get(), wideLen, result.get(), len, nullptr, nullptr);
    return result;
}

e::system::string e::system::ReceiveNativeWideString(const wchar_t *x)
{
    if (!x)
        return nullptr;
    auto wideLen = wcslen(x);
    auto len = WideCharToMultiByte(CP_ACP, 0, x, wideLen, nullptr, 0, nullptr, nullptr);
    e::system::string result(len);
    if (len)
    {
        WideCharToMultiByte(CP_ACP, 0, x, wideLen, result.c_str(), len, nullptr, nullptr);
        result.data[len] = '\0';
    }
    return result;
}

e::system::string e::system::ReceiveNativeUtf8String(const char *x)
{
    if (!x)
        return nullptr;
    auto len = strlen(x) + 1;
    auto wideLen = MultiByteToWideChar(CP_UTF8, 0, x, len, nullptr, 0);
    auto result = std::make_unique<wchar_t[]>(wideLen);
    MultiByteToWideChar(CP_UTF8, 0, x, len, result.get(), wideLen);
    return ReceiveNativeWideString(result.get());
}
#if defined(_MSC_VER)
#pragma warning(pop)
#endif