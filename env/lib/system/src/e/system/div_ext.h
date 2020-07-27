#pragma once
#include "div_ext_basic.h"
#include "any.h"
namespace e
{
    namespace system
    {
        template <class L, class R,
            std::enable_if_t<std::is_same_v<e::system::any, L> || std::is_same_v<e::system::any, R>, int> = 0>
        static auto div_float(L a, R b)
        {
            return static_cast<e::system::any>(a).div_float(static_cast<e::system::any>(b));
        }

        template <class L, class R,
            std::enable_if_t<std::is_same_v<e::system::any, L> || std::is_same_v<e::system::any, R>, int> = 0>
        static auto mod(L a, R b)
        {
            return static_cast<e::system::any>(a) % static_cast<e::system::any>(b);
        }

        template <class L, class R,
            std::enable_if_t<std::is_same_v<e::system::any, L> || std::is_same_v<e::system::any, R>, int> = 0>
        static auto div_int(L a, R b)
        {
            return static_cast<e::system::any>(a).div_int(static_cast<e::system::any>(b));
        }
    }
}
