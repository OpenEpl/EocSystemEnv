#pragma once
#include <type_traits>
namespace e
{
    namespace system
    {
        template <typename T>
        struct default_value
        {
            static T value()
            {
                return T();
            }
            static T null()
            {
                if constexpr (std::is_convertible_v<std::nullptr_t, T>)
                {
                    return T(nullptr);
                }
                else
                {
                    return T();
                }
            }
        };
    }
}