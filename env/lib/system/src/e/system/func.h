﻿#pragma once
#include "basic_type.h"
#include "methodptr_packager.h"
#include "methodptr_caller.h"
#include "temp_reference.h"
#include "div_ext.h"
#include "native_string.h"
namespace e
{
    namespace system
    {
        template <typename T>
        temp_reference<T> forceRef(const T &a)
        {
            return temp_reference<T>(a);
        }

        template <typename T>
        std::reference_wrapper<T> forceRef(T &a)
        {
            return std::reference_wrapper<T>(a);
        }
        template <typename T>
        std::remove_reference_t<T> noRef(T &&a)
        {
            return a;
        }
        template <typename T>
        std::remove_reference_t<T> noRef(T &a)
        {
            return a;
        }

#define BYREF(t, x) e::system::forceRef<t>(x).get()
#define BYREF_AUTO(x) e::system::forceRef(x).get()

        template <typename TTarget, typename TSrc>
        struct casting
        {
            static auto cast(TSrc &&src)
            {
                return static_cast<TTarget>(src);
            }
            static auto cast(TSrc &src)
            {
                return static_cast<TTarget>(src);
            }
            static auto cast(const TSrc &src)
            {
                return static_cast<TTarget>(src);
            }
        };

        template <>
        struct casting<e::system::any, e::system::any>
        {
            static e::system::any cast(e::system::any &&src)
            {
                return std::move(src);
            }
            static e::system::any cast(e::system::any &src)
            {
                return src;
            }
            static e::system::any cast(const e::system::any &src)
            {
                return src;
            }
        };

        template <typename TTarget>
        struct casting<TTarget, e::system::any>
        {
            static TTarget cast(const e::system::any &src)
            {
                if constexpr (std::is_arithmetic_v<TTarget>)
                {
                    if (src.type() == typeid(uint8_t))
                        return casting<TTarget, uint8_t>::cast(src.cast<uint8_t>());
                    if (src.type() == typeid(int16_t))
                        return casting<TTarget, int16_t>::cast(src.cast<int16_t>());
                    if (src.type() == typeid(int32_t))
                        return casting<TTarget, int32_t>::cast(src.cast<int32_t>());
                    if (src.type() == typeid(int64_t))
                        return casting<TTarget, int64_t>::cast(src.cast<int64_t>());
                    if (src.type() == typeid(double))
                        return casting<TTarget, double>::cast(src.cast<double>());
                    return casting<TTarget, float>::cast(src.cast<float>());
                }
                else
                {
                    return src.cast<TTarget>();
                }
            }
        };

        template <typename TTarget, typename TSrc>
        auto cast(TSrc &&src)
        {
            return casting<TTarget, std::remove_const_t<std::remove_reference_t<TSrc>>>::cast(std::forward<TSrc>(src));
        }

        e::system::bin make_bin(void *data, size_t size);

        template <class T, std::enable_if_t<std::is_same_v<T, e::system::any>, int> = 0>
        inline const std::type_info &GetTypeId(const T &value)
        {
            return value.type();
        }
        template <class T, std::enable_if_t<!std::is_same_v<T, e::system::any>, int> = 0>
        inline const std::type_info &GetTypeId(const T &value)
        {
            return typeid(T);
        }
    }
}