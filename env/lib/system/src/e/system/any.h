#pragma once
#include <new>
#include <typeinfo>
#include <vector>
#include "marshaler.h"
#include "type_operator_traits.h"
#include "div_ext_basic.h"
namespace e
{
    namespace system
    {
        class bad_any_cast : public std::bad_cast
        {
        public:
            virtual const char *what() const noexcept override
            {
                return "Bad e::system::any::cast<T>";
            }
        };

        class any;

        template <typename T, std::enable_if_t<!std::is_same_v<T, e::system::any>, int> = 0>
        void marshalToVector(std::vector<intptr_t> &vector, T &that)
        {
            using TMarshal = valMarshaler<T>;
            constexpr size_t elemCount = (sizeof(typename TMarshal::NativeType) + sizeof(intptr_t) - 1) / sizeof(intptr_t);
            size_t p = vector.size();
            for (size_t i = 0; i < elemCount; i++)
            {
                vector.push_back(0);
            }
            typename TMarshal::NativeType &native = *reinterpret_cast<typename TMarshal::NativeType *>(&vector.at(p));
            TMarshal::marshal(native, that);
        }

        template <typename T, std::enable_if_t<!std::is_same_v<T, e::system::any>, int> = 0>
        void cleanupFromVector(std::vector<intptr_t> &vector, T &that)
        {
            using TMarshal = valMarshaler<T>;
            constexpr size_t elemCount = (sizeof(typename TMarshal::NativeType) + sizeof(intptr_t) - 1) / sizeof(intptr_t);
            size_t p = vector.size() - elemCount;
            typename TMarshal::NativeType &native = *reinterpret_cast<typename TMarshal::NativeType *>(&vector.at(p));
            TMarshal::cleanup(native, that);
            for (size_t i = 0; i < elemCount; i++)
            {
                vector.pop_back();
            }
        }

        union _Any_DataStorage {
            void *ptr;
            uint8_t data[sizeof(double)];
            template <class T>
            constexpr static bool IsInlineType = std::is_nothrow_move_constructible_v<T> && sizeof(T) <= sizeof(_Any_DataStorage::data);
            template <class T>
            T *get()
            {
                if constexpr (_Any_DataStorage::IsInlineType<T>)
                {
                    return static_cast<T *>((void *)(&this->data));
                }
                else
                {
                    return static_cast<T *>(this->ptr);
                }
            }
            template <class T>
            const T *get() const
            {
                if constexpr (_Any_DataStorage::IsInlineType<T>)
                {
                    return static_cast<const T *>((void *)(&this->data));
                }
                else
                {
                    return static_cast<const T *>(this->ptr);
                }
            }
        };

        class _Any_RuntimeInfo
        {
        public:
            virtual void clone(const _Any_DataStorage &that, _Any_DataStorage &target) const = 0;
            virtual void move(_Any_DataStorage &that, _Any_DataStorage &target) const noexcept = 0;
            virtual void destroy(_Any_DataStorage &that) const noexcept = 0;
            //封送
            virtual void byRef_Marshal(void *&v, _Any_DataStorage &that) const = 0;
            virtual void byRef_Cleanup(void *&v, _Any_DataStorage &that) const = 0;
            virtual void byVal_MarshalToVector(std::vector<intptr_t> &stack, _Any_DataStorage &that) const = 0;
            virtual void byVal_CleanupFromVector(std::vector<intptr_t> &stack, _Any_DataStorage &that) const = 0;
#ifdef _M_X64
            virtual void byVal_Marshal(__int64 &v, _Any_DataStorage &that) const = 0;
            virtual void byVal_Cleanup(__int64 &v, _Any_DataStorage &that) const = 0;
#endif
            //算术运算符
            virtual e::system::any add(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any sub(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any mul(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any div(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any div_float(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any div_int(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any mod(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any neg(const e::system::any &a) const = 0;

            //比较运算符
            virtual bool eq(const e::system::any &a, const e::system::any &b) const = 0;
            virtual bool ne(const e::system::any &a, const e::system::any &b) const = 0;
            virtual bool lt(const e::system::any &a, const e::system::any &b) const = 0;
            virtual bool le(const e::system::any &a, const e::system::any &b) const = 0;
            virtual bool gt(const e::system::any &a, const e::system::any &b) const = 0;
            virtual bool ge(const e::system::any &a, const e::system::any &b) const = 0;

            //位运算符
            virtual e::system::any bit_and(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any bit_or(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any bit_xor(const e::system::any &a, const e::system::any &b) const = 0;
            virtual e::system::any bit_shift_left(const e::system::any &a, int b) const = 0;
            virtual e::system::any bit_shift_right(const e::system::any &a, int b) const = 0;
            virtual e::system::any bit_not(const e::system::any &a) const = 0;
        };

        template <class T>
        constexpr static const _Any_RuntimeInfo *_Any_GetRuntimeInfoInstance();
        
        class any
        {
        private:
            const std::type_info *typeInfo;
            const _Any_RuntimeInfo *runtimeInfo;
            _Any_DataStorage storage;

        public:
            any() noexcept : typeInfo(nullptr), runtimeInfo(nullptr), storage()
            {
            }
            ~any() noexcept
            {
                if (runtimeInfo != nullptr)
                    runtimeInfo->destroy(storage);
            }
            template <class T>
            any(const T &value) : typeInfo(&typeid(T)), runtimeInfo(_Any_GetRuntimeInfoInstance<T>()), storage()
            {
                if constexpr (_Any_DataStorage::IsInlineType<T>)
                {
                    new (reinterpret_cast<T *>(&storage.data)) T(value);
                }
                else
                {
                    storage.ptr = new T(value);
                }
            }
            void reset() noexcept
            {
                if (runtimeInfo != nullptr)
                    runtimeInfo->destroy(storage);
                typeInfo = nullptr;
                runtimeInfo = nullptr;
            }
            e::system::any &operator=(const e::system::any &x)
            {
                if (&x == this)
                    return *this;
                this->~any();
                typeInfo = x.typeInfo;
                runtimeInfo = x.runtimeInfo;
                if (runtimeInfo != nullptr)
                    runtimeInfo->clone(x.storage, storage);
                return *this;
            }
            e::system::any &operator=(e::system::any &&x) noexcept
            {
                this->~any();
                typeInfo = x.typeInfo;
                runtimeInfo = x.runtimeInfo;
                if (runtimeInfo != nullptr)
                {
                    runtimeInfo->move(x.storage, storage);
                    x.reset();
                }
                return *this;
            }
            any(const e::system::any &x) : typeInfo(x.typeInfo), runtimeInfo(x.runtimeInfo), storage()
            {
                if (runtimeInfo != nullptr)
                    runtimeInfo->clone(x.storage, storage);
            }
            any(e::system::any &&x) noexcept : typeInfo(x.typeInfo), runtimeInfo(x.runtimeInfo), storage()
            {
                if (runtimeInfo != nullptr)
                {
                    runtimeInfo->move(x.storage, storage);
                    x.reset();
                }
            }
            const std::type_info &type() const noexcept
            {
                return has_value() ? *typeInfo : typeid(void);
            }
            bool has_value() const noexcept
            {
                return typeInfo != nullptr;
            }
            template <class T>
            T &cast()
            {
                if (typeid(T) != this->type())
                    throw bad_any_cast{};
                return *storage.get<T>();
            }
            template <class T>
            const T &cast() const
            {
                if (typeid(T) != this->type())
                    throw bad_any_cast{};
                return *storage.get<T>();
            }
            void byRef_Marshal(void *&v)
            {
                if (runtimeInfo != nullptr)
                {
                    runtimeInfo->byRef_Marshal(v, storage);
                }
                else
                {
                    v = nullptr;
                }
            }
            void byRef_Cleanup(void *&v)
            {
                if (runtimeInfo != nullptr)
                    runtimeInfo->byRef_Cleanup(v, storage);
            }

#ifdef _M_X64
            void byVal_Marshal(__int64 &v)
            {
                v = 0;
                if (runtimeInfo != nullptr)
                {
                    runtimeInfo->byVal_Marshal(v, storage);
                }
            }
            void byVal_Cleanup(__int64 &v)
            {
                if (runtimeInfo != nullptr)
                    runtimeInfo->byVal_Cleanup(v, storage);
            }
#endif

            void byVal_MarshalToVector(std::vector<intptr_t> &vector)
            {
                if (runtimeInfo != nullptr)
                {
                    runtimeInfo->byVal_MarshalToVector(vector, storage);
                }
                else
                {
                    vector.push_back(0);
                }
            }
            void byVal_CleanupFromVector(std::vector<intptr_t> &vector)
            {
                if (runtimeInfo != nullptr)
                {
                    runtimeInfo->byVal_CleanupFromVector(vector, storage);
                }
                else
                {
                    vector.pop_back();
                }
            }

            friend auto operator+(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->add(a, b);
            }
            friend auto operator-(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->sub(a, b);
            }
            friend auto operator*(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->mul(a, b);
            }
            friend auto operator/(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->div(a, b);
            }
            friend auto operator%(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->mod(a, b);
            }
            e::system::any operator-()
            {
                return this->runtimeInfo->neg(*this);
            }
            friend auto operator==(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->eq(a, b);
            }
            friend auto operator!=(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->ne(a, b);
            }
            friend auto operator<(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->lt(a, b);
            }
            friend auto operator<=(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->le(a, b);
            }
            friend auto operator>(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->gt(a, b);
            }
            friend auto operator>=(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->ge(a, b);
            }
            friend auto operator&(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->bit_and(a, b);
            }
            friend auto operator|(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->bit_or(a, b);
            }
            friend auto operator^(const e::system::any &a, const e::system::any &b)
            {
                return a.runtimeInfo->bit_xor(a, b);
            }
            friend auto operator<<(const e::system::any &a, int b)
            {
                return a.runtimeInfo->bit_shift_left(a, b);
            }
            friend auto operator>>(const e::system::any &a, int b)
            {
                return a.runtimeInfo->bit_shift_right(a, b);
            }
            e::system::any operator~()
            {
                return this->runtimeInfo->bit_not(*this);
            }
            auto div_int(const e::system::any &that) const
            {
                return this->runtimeInfo->div_int(*this, that);
            }
            auto div_float(const e::system::any &that) const
            {
                return this->runtimeInfo->div_float(*this, that);
            }
        };

        inline void marshalToVector(std::vector<intptr_t> &vector, e::system::any &that)
        {
            that.byVal_MarshalToVector(vector);
        }
        inline void cleanupFromVector(std::vector<intptr_t> &vector, e::system::any &that)
        {
            that.byVal_CleanupFromVector(vector);
        }

        template <typename TFirst, typename... TOthers>
        void marshalToVector(std::vector<intptr_t> &vector, TFirst &that, TOthers &... those)
        {
            marshalToVector(vector, that);
            if constexpr (sizeof...(those) != 0)
            {
                marshalToVector(vector, those...);
            }
        }

        template <typename TFirst, typename... TOthers>
        void cleanupFromVector(std::vector<intptr_t> &vector, TFirst &that, TOthers &... those)
        {
            if constexpr (sizeof...(those) != 0)
            {
                cleanupFromVector(vector, those...);
            }
            cleanupFromVector(vector, that);
        }

        //对结构体内通用型只支持传址封送
        template <>
        struct marshaler<e::system::any>
        {
            using NativeType = void *;
            static constexpr bool SameMemoryStruct = false;
            static void marshal(NativeType &v, e::system::any &r)
            {
                r.byRef_Marshal(v);
            }
            static void cleanup(NativeType &v, e::system::any &r)
            {
                r.byRef_Cleanup(v);
            }
        };
        template <>
        struct refMarshaler<e::system::any> : marshaler<e::system::any>
        {
        };
        //对通用型参数的传值封送由Caller实现
        template <>
        struct valMarshaler<e::system::any> : refMarshaler<e::system::any>
        {
        };

        template <class T>
        class _Any_RuntimeInfoImpl : public _Any_RuntimeInfo
        {
        public:
            virtual void clone(const _Any_DataStorage &that, _Any_DataStorage &target) const
            {
                if constexpr (_Any_DataStorage::IsInlineType<T>)
                {
                    new (target.get<T>()) T(*that.get<T>());
                }
                else
                {
                    target.ptr = new T(*that.get<T>());
                }
            }
            virtual void move(_Any_DataStorage &that, _Any_DataStorage &target) const noexcept
            {
                if constexpr (_Any_DataStorage::IsInlineType<T>)
                {
                    new (target.get<T>()) T(std::move(*that.get<T>()));
                }
                else
                {
                    target.ptr = that.ptr;
                    that.ptr = nullptr;
                }
            }
            virtual void destroy(_Any_DataStorage &that) const noexcept
            {
                if constexpr (_Any_DataStorage::IsInlineType<T>)
                {
                    that.get<T>()->~T();
                }
                else
                {
                    delete that.get<T>();
                }
            }
            //封送
            virtual void byRef_Marshal(void *&v, _Any_DataStorage &that) const
            {
                using TMarshal = refMarshaler<T>;
                typename TMarshal::NativeType &native = reinterpret_cast<typename TMarshal::NativeType &>(v);
                TMarshal::marshal(native, *that.get<T>());
            }
            virtual void byRef_Cleanup(void *&v, _Any_DataStorage &that) const
            {
                using TMarshal = refMarshaler<T>;
                typename TMarshal::NativeType &native = reinterpret_cast<typename TMarshal::NativeType &>(v);
                TMarshal::cleanup(native, *that.get<T>());
            }
            virtual void byVal_MarshalToVector(std::vector<intptr_t> &vector, _Any_DataStorage &that) const
            {
                marshalToVector<T>(vector, *that.get<T>());
            }
            virtual void byVal_CleanupFromVector(std::vector<intptr_t> &vector, _Any_DataStorage &that) const
            {
                cleanupFromVector<T>(vector, *that.get<T>());
            }
#ifdef _M_X64
            virtual void byVal_Marshal(__int64 &v, _Any_DataStorage &that) const
            {
                using TMarshal = valMarshaler<T>;
                typename TMarshal::NativeType &native = reinterpret_cast<typename TMarshal::NativeType &>(v);
                TMarshal::marshal(native, *that.get<T>());
            }
            virtual void byVal_Cleanup(__int64 &v, _Any_DataStorage &that) const
            {
                using TMarshal = valMarshaler<T>;
                typename TMarshal::NativeType &native = reinterpret_cast<typename TMarshal::NativeType &>(v);
                TMarshal::cleanup(native, *that.get<T>());
            }
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4804) //unsafe use of type 'bool' in operation
#pragma warning(disable : 4805) //unsafe mix of type 'type' and type 'type' in operation
#endif

#pragma push_macro("__ImplementProcessorForBinaryOperator")
#undef __ImplementProcessorForBinaryOperator
#define __ImplementProcessorForBinaryOperator(name, checker, op, resultType)        \
    virtual resultType name(const e::system::any &a, const e::system::any &b) const \
    {                                                                               \
        if constexpr (std::is_arithmetic_v<T>)                                      \
        {                                                                           \
            if constexpr (checker<std::add_const_t<T>, uint8_t>)                    \
                if (b.type() == typeid(uint8_t))                                    \
                    return a.cast<T>() op b.cast<uint8_t>();                        \
            if constexpr (checker<std::add_const_t<T>, const int16_t>)              \
                if (b.type() == typeid(int16_t))                                    \
                    return a.cast<T>() op b.cast<int16_t>();                        \
            if constexpr (checker<std::add_const_t<T>, const int32_t>)              \
                if (b.type() == typeid(int32_t))                                    \
                    return a.cast<T>() op b.cast<int32_t>();                        \
            if constexpr (checker<std::add_const_t<T>, const int64_t>)              \
                if (b.type() == typeid(int64_t))                                    \
                    return a.cast<T>() op b.cast<int64_t>();                        \
            if constexpr (checker<std::add_const_t<T>, const float>)                \
                if (b.type() == typeid(float))                                      \
                    return a.cast<T>() op b.cast<float>();                          \
            if constexpr (checker<std::add_const_t<T>, const double>)               \
                if (b.type() == typeid(double))                                     \
                    return a.cast<T>() op b.cast<double>();                         \
        }                                                                           \
        else if constexpr (checker<std::add_const_t<T>, std::add_const_t<T>>)       \
        {                                                                           \
            if (b.type() == typeid(T))                                              \
                return a.cast<T>() op b.cast<T>();                                  \
        }                                                                           \
        throw std::domain_error("Bad operator for this object");                    \
    }
            __ImplementProcessorForBinaryOperator(add, has_add_operator_v, +, e::system::any);
            __ImplementProcessorForBinaryOperator(sub, has_sub_operator_v, -, e::system::any);
            __ImplementProcessorForBinaryOperator(mul, has_mul_operator_v, *, e::system::any);
            __ImplementProcessorForBinaryOperator(div, has_div_operator_v, /, e::system::any);

            __ImplementProcessorForBinaryOperator(eq, has_eq_operator_v, ==, bool);
            __ImplementProcessorForBinaryOperator(ne, has_ne_operator_v, !=, bool);
            __ImplementProcessorForBinaryOperator(lt, has_lt_operator_v, <, bool);
            __ImplementProcessorForBinaryOperator(le, has_le_operator_v, <=, bool);
            __ImplementProcessorForBinaryOperator(gt, has_gt_operator_v, >, bool);
            __ImplementProcessorForBinaryOperator(ge, has_ge_operator_v, >=, bool);

            __ImplementProcessorForBinaryOperator(bit_and, has_bit_and_operator_v, &, e::system::any);
            __ImplementProcessorForBinaryOperator(bit_or, has_bit_or_operator_v, |, e::system::any);
            __ImplementProcessorForBinaryOperator(bit_xor, has_bit_xor_operator_v, ^, e::system::any);
#pragma pop_macro("__ImplementProcessorForBinaryOperator")

            virtual e::system::any div_float(const e::system::any &a, const e::system::any &b) const
            {
                if constexpr (std::is_arithmetic_v<T>)
                {
                    if (b.type() == typeid(uint8_t))
                        return e::system::div_float(a.cast<T>(), b.cast<uint8_t>());
                    if (b.type() == typeid(int16_t))
                        return e::system::div_float(a.cast<T>(), b.cast<int16_t>());
                    if (b.type() == typeid(int32_t))
                        return e::system::div_float(a.cast<T>(), b.cast<int32_t>());
                    if (b.type() == typeid(int64_t))
                        return e::system::div_float(a.cast<T>(), b.cast<int64_t>());
                    if (b.type() == typeid(float))
                        return e::system::div_float(a.cast<T>(), b.cast<float>());
                    if (b.type() == typeid(double))
                        return e::system::div_float(a.cast<T>(), b.cast<double>());
                }
                throw std::domain_error("Bad operator for this object");
            }

            virtual e::system::any div_int(const e::system::any &a, const e::system::any &b) const
            {
                if constexpr (std::is_arithmetic_v<T>)
                {
                    if (b.type() == typeid(uint8_t))
                        return e::system::div_int(a.cast<T>(), b.cast<uint8_t>());
                    if (b.type() == typeid(int16_t))
                        return e::system::div_int(a.cast<T>(), b.cast<int16_t>());
                    if (b.type() == typeid(int32_t))
                        return e::system::div_int(a.cast<T>(), b.cast<int32_t>());
                    if (b.type() == typeid(int64_t))
                        return e::system::div_int(a.cast<T>(), b.cast<int64_t>());
                    if (b.type() == typeid(float))
                        return e::system::div_int(a.cast<T>(), b.cast<float>());
                    if (b.type() == typeid(double))
                        return e::system::div_int(a.cast<T>(), b.cast<double>());
                }
                throw std::domain_error("Bad operator for this object");
            }

            virtual e::system::any mod(const e::system::any &a, const e::system::any &b) const
            {
                if constexpr (std::is_arithmetic_v<T>)
                {
                    if (b.type() == typeid(uint8_t))
                        return e::system::mod(a.cast<T>(), b.cast<uint8_t>());
                    if (b.type() == typeid(int16_t))
                        return e::system::mod(a.cast<T>(), b.cast<int16_t>());
                    if (b.type() == typeid(int32_t))
                        return e::system::mod(a.cast<T>(), b.cast<int32_t>());
                    if (b.type() == typeid(int64_t))
                        return e::system::mod(a.cast<T>(), b.cast<int64_t>());
                    if (b.type() == typeid(float))
                        return e::system::mod(a.cast<T>(), b.cast<float>());
                    if (b.type() == typeid(double))
                        return e::system::mod(a.cast<T>(), b.cast<double>());
                }
                else if constexpr (has_mod_operator_v<std::add_const_t<T>, std::add_const_t<T>>)
                {
                    if (b.type() == typeid(T))
                        return a.cast<T>() % b.cast<T>();
                }
                throw std::domain_error("Bad operator for this object");
            }

            virtual e::system::any bit_shift_left(const e::system::any &a, int b) const
            {
                if constexpr (has_shift_left_operator_v<std::add_const_t<T>, const int>)
                    return a.cast<T>() << b;
                throw std::domain_error("Bad operator for this object");
            }

            virtual e::system::any bit_shift_right(const e::system::any &a, int b) const
            {
                if constexpr (has_shift_right_operator_v<std::add_const_t<T>, const int>)
                    return a.cast<T>() >> b;
                throw std::domain_error("Bad operator for this object");
            }

            virtual e::system::any neg(const e::system::any &a) const
            {
                if constexpr (has_neg_operator_v<std::add_const_t<T>>)
                    return -a.cast<T>();
                throw std::domain_error("Bad operator for this object");
            }

            virtual e::system::any bit_not(const e::system::any &a) const
            {
                if constexpr (has_bit_not_operator_v<std::add_const_t<T>>)
                    return ~a.cast<T>();
                throw std::domain_error("Bad operator for this object");
            }
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
        };

        template <class T>
        constexpr static _Any_RuntimeInfoImpl<T> _Any_RuntimeInfoInstance{};

        template <class T>
        constexpr static const _Any_RuntimeInfo *_Any_GetRuntimeInfoInstance()
        {
            return std::addressof(_Any_RuntimeInfoInstance<T>);
        }
    }
}