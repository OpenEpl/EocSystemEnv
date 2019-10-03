#pragma once
#include <type_traits>
namespace e
{
    namespace system
    {
#pragma push_macro("__DefineBinaryOperatorChecker")
#undef __DefineBinaryOperatorChecker
#define __DefineBinaryOperatorChecker(name, op)                                \
    template <typename _Tp, typename _Up>                                      \
    class __##name##_helper                                                    \
    {                                                                          \
        template <typename _Tp1, typename _Up1,                                \
            typename = decltype(std::declval<_Tp1>() op std::declval<_Up1>())> \
        static std::true_type __test(int);                                     \
        template <typename, typename>                                          \
        static std::false_type __test(...);                                    \
                                                                               \
    public:                                                                    \
        typedef decltype(__test<_Tp, _Up>(0)) type;                            \
    };                                                                         \
                                                                               \
    template <typename _Tp, typename _Up>                                      \
    class name : public __##name##_helper<_Tp, _Up>::type                      \
    {                                                                          \
    };                                                                         \
                                                                               \
    template <typename _Tp, typename _Up>                                      \
    inline constexpr bool name##_v = name<_Tp, _Up>::value;

        __DefineBinaryOperatorChecker(has_add_operator, +);
        __DefineBinaryOperatorChecker(has_sub_operator, -);
        __DefineBinaryOperatorChecker(has_mul_operator, *);
        __DefineBinaryOperatorChecker(has_div_operator, /);
        __DefineBinaryOperatorChecker(has_mod_operator, %);

        __DefineBinaryOperatorChecker(has_eq_operator, ==);
        __DefineBinaryOperatorChecker(has_ne_operator, !=);
        __DefineBinaryOperatorChecker(has_lt_operator, <);
        __DefineBinaryOperatorChecker(has_le_operator, <=);
        __DefineBinaryOperatorChecker(has_gt_operator, >);
        __DefineBinaryOperatorChecker(has_ge_operator, >=);

        __DefineBinaryOperatorChecker(has_bit_and_operator, &);
        __DefineBinaryOperatorChecker(has_bit_or_operator, |);
        __DefineBinaryOperatorChecker(has_bit_xor_operator, ^);
        __DefineBinaryOperatorChecker(has_shift_left_operator, <<);
        __DefineBinaryOperatorChecker(has_shift_right_operator, >>);
#pragma pop_macro("__DefineBinaryOperatorChecker")

#pragma push_macro("__DefineUnaryOperatorChecker")
#undef __DefineUnaryOperatorChecker
#define __DefineUnaryOperatorChecker(name, op)            \
    template <typename _Tp>                               \
    class __##name##_helper                               \
    {                                                     \
        template <typename _Tp1,                          \
            typename = decltype(op std::declval<_Tp1>())> \
        static std::true_type __test(int);                \
        template <typename>                               \
        static std::false_type __test(...);               \
                                                          \
    public:                                               \
        typedef decltype(__test<_Tp>(0)) type;            \
    };                                                    \
                                                          \
    template <typename _Tp>                               \
    class name : public __##name##_helper<_Tp>::type      \
    {                                                     \
    };                                                    \
                                                          \
    template <typename _Tp>                               \
    inline constexpr bool name##_v = name<_Tp>::value;

        __DefineUnaryOperatorChecker(has_neg_operator, -);
        __DefineUnaryOperatorChecker(has_bit_not_operator, ~);
#pragma pop_macro("__DefineUnaryOperatorChecker")
    }
}
