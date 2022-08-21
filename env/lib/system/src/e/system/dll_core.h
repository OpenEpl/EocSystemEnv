#pragma once
#include <cstdint>
namespace e
{
    namespace system
    {
        namespace dll
        {
            // use namespace to avoid conflict
            using TModule = intptr_t;
            namespace internal
            {
                extern "C"
                {
                    __declspec(dllimport) TModule __stdcall LoadLibraryW(const wchar_t *lpLibFileName);
                    __declspec(dllimport) void *__stdcall GetProcAddress(TModule hModule, const char *lpProcName);
                }
            }
        }
    }
}

#define eoc_DefineMoudleLoader(name, dllname)                                                       \
    static ::e::system::dll::TModule InternalMoudleHandle_##name;                                   \
    static ::e::system::dll::TModule GetMoudleHandle_##name()                                       \
    {                                                                                               \
        ::e::system::dll::TModule x = InternalMoudleHandle_##name;                                  \
        if (!x)                                                                                     \
            InternalMoudleHandle_##name = x = ::e::system::dll::internal::LoadLibraryW(L##dllname); \
        return x;                                                                                   \
    }

#define eoc_DefineFuncPtrGetter(name, hmod, rname)                                                \
    static void *InternalFuncPtr_##name;                                                          \
    static void *GetFuncPtr_##name()                                                              \
    {                                                                                             \
        void *x = InternalFuncPtr_##name;                                                         \
        if (!x)                                                                                   \
            InternalFuncPtr_##name = x = ::e::system::dll::internal::GetProcAddress(hmod, rname); \
        return x;                                                                                 \
    }