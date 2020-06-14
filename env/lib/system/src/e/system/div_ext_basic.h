#pragma once
#include <type_traits>
#include <cmath>
namespace e
{
    namespace system
    {
        template <class L, class R,
            std::enable_if_t<std::is_integral_v<L> && std::is_integral_v<R>, int> = 0>
        static auto div_float(L a, R b)
        {
            return (double)a / b;
        }
        template <class L, class R,
            std::enable_if_t<std::is_arithmetic_v<L> && std::is_arithmetic_v<R> && (std::is_floating_point_v<L> || std::is_floating_point_v<R>), int> = 0>
        static auto div_float(L a, R b)
        {
            return a / b;
        }

        template <class L, class R,
            std::enable_if_t<std::is_integral_v<L> && std::is_integral_v<R>, int> = 0>
        static auto mod(L a, R b)
        {
            return a % b;
        }
        template <class L, class R,
            std::enable_if_t<std::is_arithmetic_v<L> && std::is_arithmetic_v<R> && (std::is_floating_point_v<L> || std::is_floating_point_v<R>), int> = 0>
        static auto mod(L a, R b)
        {
            return std::fmod(a, b);
        }

        template <class L, class R,
            std::enable_if_t<std::is_integral_v<L> && std::is_integral_v<R>, int> = 0>
        static auto div_int(L a, R b)
        {
            return a / b;
        }
        template <class L, class R,
            std::enable_if_t<std::is_arithmetic_v<L> && std::is_arithmetic_v<R> && (std::is_floating_point_v<L> || std::is_floating_point_v<R>), int> = 0>
        static auto div_int(L a, R b)
        {
            auto r = a / b;
            decltype(r) intpart;
            std::modf(r, &intpart);
            return intpart;
        }
    }
}
