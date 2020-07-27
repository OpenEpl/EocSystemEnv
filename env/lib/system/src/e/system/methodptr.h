#pragma once
#include <cstdint>
namespace e
{
    namespace system
    {
        struct methodptr
        {
            void *data;
            constexpr methodptr() noexcept : data(nullptr)
            {
            }
            constexpr methodptr(void *init_value) noexcept : data((void *)init_value)
            {
            }
            constexpr methodptr(intptr_t init_value) noexcept : data((void *)init_value)
            {
            }
            constexpr methodptr(const methodptr &that) noexcept : data(that.data)
            {
            }
            constexpr methodptr(methodptr &&that) noexcept : data(that.data)
            {
            }
            methodptr &operator=(const methodptr &that) noexcept
            {
                this->data = that.data;
                return *this;
            }
            methodptr &operator=(methodptr &&that) noexcept
            {
                this->data = that.data;
                return *this;
            }
            constexpr operator void *() const noexcept
            {
                return (void *)data;
            }
            constexpr operator intptr_t() const noexcept
            {
                return (intptr_t)data;
            }
        };
    }
}