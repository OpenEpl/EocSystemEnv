#pragma once
#include <functional>
#include <sstream>
#include "type_operator_traits.h"
#include "string_basic.h"
namespace e
{
    namespace system
    {
        template <class T>
        e::system::string repr(T &&x);
        template <class T>
        struct repr_maker
        {
            static e::system::string of(const T &x)
            {
                if constexpr (has_shift_left_operator_v<std::ostringstream, const T>)
                {
                    std::ostringstream stream;
                    stream << x;
                    return e::system::string(stream.str().c_str());
                }
                else if constexpr (is_range_v<const T>)
                {
                    std::ostringstream stream;
                    auto pBegin = std::begin(x);
                    auto pEnd = std::end(x);
                    bool first = true;
                    stream << "{";
                    for (auto pElem = pBegin; pElem != pEnd; ++pElem)
                    {
                        if (first)
                        {
                            first = false;
                        }
                        else
                        {
                            stream << ", ";
                        }
                        auto elem = *pElem;
                        stream << e::system::repr(elem);
                    }
                    stream << "}";
                    return e::system::string(stream.str().c_str());
                }
                else
                {
                    return EOC_STR_CONST("[Unknown]");
                }
            }
        };
        template <>
        struct repr_maker<bool>
        {
            static e::system::string of(const bool &x)
            {
                return x ? EOC_STR_CONST("true") : EOC_STR_CONST("false");
            }
        };
        template <class T>
        e::system::string repr(T &&x)
        {
            return repr_maker<std::remove_const_t<std::remove_reference_t<T>>>::of(std::forward<T>(x));
        }
// Implement dbg info for test framework via e::system::repr
// This macro must appear at top level scope ------ not inside a namespace
#ifdef CATCH_VERSION_MAJOR
#define IMPLEMENT_DBG_INFO_VIA_REPR(type)                                                       \
    template <>                                                                                 \
    struct Catch::StringMaker<typename type>                                                    \
    {                                                                                           \
        static ::std::string convert(::std::add_lvalue_reference_t<::std::add_const_t<type>> x) \
        {                                                                                       \
            return ::std::string(::e::system::repr(x).c_str());                                 \
        }                                                                                       \
    };
#else
#define IMPLEMENT_DBG_INFO_VIA_REPR(type)
#endif
    }
}