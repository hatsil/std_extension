#pragma once

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ext {
namespace detail {
template <typename IsNothrowInvocable, typename IsVararg, typename IsConst, typename IsVolatile,
          typename IsRvalueReference, typename IsLvalueReference, typename Res, typename... Args>
struct function_traits_base {
    using is_invocable         = std::true_type;
    using is_nothrow_invocable = IsNothrowInvocable;
    using is_vararg            = IsVararg;
    using is_const             = IsConst;
    using is_volatile          = IsVolatile;
    using is_rvalue_reference  = IsRvalueReference;
    using is_lvalue_reference  = IsLvalueReference;
    using result_type          = Res;
    using args_type            = std::tuple<Args...>;
    using arity                = std::tuple_size<args_type>;
};

template <typename Signature> struct function_traits {
    using is_invocable = std::false_type;
};

template <typename Res, typename... Args>
struct function_traits<Res(Args...)>
    : function_traits_base<std::false_type, std::false_type, std::false_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const>
    : function_traits_base<std::false_type, std::false_type, std::true_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) volatile>
    : function_traits_base<std::false_type, std::false_type, std::false_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const volatile>
    : function_traits_base<std::false_type, std::false_type, std::true_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) &>
    : function_traits_base<std::false_type, std::false_type, std::false_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const &>
    : function_traits_base<std::false_type, std::false_type, std::true_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) volatile &>
    : function_traits_base<std::false_type, std::false_type, std::false_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const volatile &>
    : function_traits_base<std::false_type, std::false_type, std::true_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) &&>
    : function_traits_base<std::false_type, std::false_type, std::false_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const &&>
    : function_traits_base<std::false_type, std::false_type, std::true_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) volatile &&>
    : function_traits_base<std::false_type, std::false_type, std::false_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const volatile &&>
    : function_traits_base<std::false_type, std::false_type, std::true_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......)>
    : function_traits_base<std::false_type, std::true_type, std::false_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const>
    : function_traits_base<std::false_type, std::true_type, std::true_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) volatile>
    : function_traits_base<std::false_type, std::true_type, std::false_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const volatile>
    : function_traits_base<std::false_type, std::true_type, std::true_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) &>
    : function_traits_base<std::false_type, std::true_type, std::false_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const &>
    : function_traits_base<std::false_type, std::true_type, std::true_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) volatile &>
    : function_traits_base<std::false_type, std::true_type, std::false_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const volatile &>
    : function_traits_base<std::false_type, std::true_type, std::true_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) &&>
    : function_traits_base<std::false_type, std::true_type, std::false_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const &&>
    : function_traits_base<std::false_type, std::true_type, std::true_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) volatile &&>
    : function_traits_base<std::false_type, std::true_type, std::false_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const volatile &&>
    : function_traits_base<std::false_type, std::true_type, std::true_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) noexcept>
    : function_traits_base<std::true_type, std::false_type, std::false_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const noexcept>
    : function_traits_base<std::true_type, std::false_type, std::true_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) volatile noexcept>
    : function_traits_base<std::true_type, std::false_type, std::false_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const volatile noexcept>
    : function_traits_base<std::true_type, std::false_type, std::true_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) & noexcept>
    : function_traits_base<std::true_type, std::false_type, std::false_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const & noexcept>
    : function_traits_base<std::true_type, std::false_type, std::true_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) volatile & noexcept>
    : function_traits_base<std::true_type, std::false_type, std::false_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const volatile & noexcept>
    : function_traits_base<std::true_type, std::false_type, std::true_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) && noexcept>
    : function_traits_base<std::true_type, std::false_type, std::false_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const && noexcept>
    : function_traits_base<std::true_type, std::false_type, std::true_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) volatile && noexcept>
    : function_traits_base<std::true_type, std::false_type, std::false_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args...) const volatile && noexcept>
    : function_traits_base<std::true_type, std::false_type, std::true_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) noexcept>
    : function_traits_base<std::true_type, std::true_type, std::false_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const noexcept>
    : function_traits_base<std::true_type, std::true_type, std::true_type, std::false_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) volatile noexcept>
    : function_traits_base<std::true_type, std::true_type, std::false_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const volatile noexcept>
    : function_traits_base<std::true_type, std::true_type, std::true_type, std::true_type,
                           std::false_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) & noexcept>
    : function_traits_base<std::true_type, std::true_type, std::false_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const & noexcept>
    : function_traits_base<std::true_type, std::true_type, std::true_type, std::false_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) volatile & noexcept>
    : function_traits_base<std::true_type, std::true_type, std::false_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const volatile & noexcept>
    : function_traits_base<std::true_type, std::true_type, std::true_type, std::true_type,
                           std::false_type, std::true_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) && noexcept>
    : function_traits_base<std::true_type, std::true_type, std::false_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const && noexcept>
    : function_traits_base<std::true_type, std::true_type, std::true_type, std::false_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) volatile && noexcept>
    : function_traits_base<std::true_type, std::true_type, std::false_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename Res, typename... Args>
struct function_traits<Res(Args......) const volatile && noexcept>
    : function_traits_base<std::true_type, std::true_type, std::true_type, std::true_type,
                           std::true_type, std::false_type, Res, Args...> {};

template <typename MemberPtr> struct member_traits {
    using is_invocable = std::false_type;
};

template <typename Method, typename Class>
struct member_traits<Method Class::*> : function_traits<Method> {
    using class_type = Class;
};

template <typename Signature> struct weak_function_object_traits {
    using is_invocable = std::false_type;
};

template <typename Functor>
struct weak_function_object_traits : member_traits<decltype(&Functor::operator())> {
    using functor_type = Functor;
};

enum class invocable_type {
    FUNCTION,
    MEMBER_FUNCTION,
    FUNCTION_OBJECT,
};

using function_tag        = std::integral_constant<invocable_type, invocable_type::FUNCTION>;
using member_function_tag = std::integral_constant<invocable_type, invocable_type::MEMBER_FUNCTION>;
using function_object_tag = std::integral_constant<invocable_type, invocable_type::FUNCTION_OBJECT>;

template <typename Functor> struct invocable_decay : std::remove_pointer<std::decay_t<Functor>> {};

template <typename Functor>
struct invocable_decay<std::reference_wrapper<Functor>> : invocable_decay<Functor> {};

template <typename Functor> using invocable_decay_t = typename invocable_decay<Functor>::type;

template <typename Functor, typename Weak_invocable_type> struct weak_invocable_traits_helper;

template <typename Functor>
struct weak_invocable_traits_helper<
    Functor, std::integral_constant<invocable_type, invocable_type::FUNCTION>>
    : function_traits<weak_invocable_decay_t<Functor>> {};

template <typename Signature>
struct weak_invocable_traits_helper<
    Signature, std::integral_constant<invocable_type, invocable_type::MEMBER_FUNCTION>>
    : member_traits<Signature> {};

template <typename Functor>
struct weak_invocable_traits_helper<
    Functor, std::integral_constant<invocable_type, invocable_type::FUNCTION_OBJECT>>
    : weak_function_object_traits<Functor> {};

template <typename Functor, typename = decltype(&Functor::operator())>
auto get_weak_invocable_type(int)
    -> std::integral_constant<invocable_type, invocable_type::FUNCTION_OBJECT>;

template <typename Functor>
auto get_weak_invocable_type(...)
    -> std::conditional_t<std::is_member_function_pointer_v<Functor>,
                          std::integral_constant<invocable_type, invocable_type::MEMBER_FUNCTION>,
                          std::integral_constant<invocable_type, invocable_type::FUNCTION>>;

template <typename Functor, typename F = invocable_decay_t<Functor>>
struct weak_invocable_traits
    : weak_invocable_traits_helper<F, decltype(get_weak_invocable_type<F>(0))> {};

template <bool Is_explicit_invocable, typename Functor, typename... Args>
struct explicit_invocable_traits_helper {
    using is_invocable = std::false_type;
};

template <typename Functor, typename... Args>
struct explicit_invocable_traits_helper<true, Functor, Args...>
    : function_traits_base<std::invoke_result_t<std::decay_t<Functor>, Args...>, Args...> {};

template <typename Functor, typename... Args>
struct explicit_invocable_traits
    : explicit_invocable_traits_helper<
          std::conjunction_v<std::negation<std::disjunction<
                                 std::is_member_object_pointer<invocable_decay_t<Functor>>,
                                 typename weak_invocable_traits<Functor>::is_invocable>>,
                             std::is_invocable<std::decay_t<Functor>, Args...>>,
          Functor, Args...> {};
} // namespace detail

template <typename Functor>
using is_weak_invocable = typename detail::weak_invocable_traits<Functor>::is_invocable;

template <typename Functor> constexpr bool is_weak_invocable_v = is_weak_invocable<Functor>::value;

template <typename Functor, typename... Args>
using is_explicit_invocable =
    typename detail::explicit_invocable_traits<Functor, Args...>::is_invocable;

template <typename Functor, typename... Args>
constexpr bool is_explicit_invocable_v = is_explicit_invocable<Functor, Args...>::value;

template <typename Functor, typename... Args>
using is_invocable =
    std::disjunction<is_weak_invocable<Functor>, is_explicit_invocable<Functor, Args...>>;

template <typename Functor, typename... Args>
constexpr bool is_invocable_v = ::ext::is_invocable<Functor, Args...>::value;

template <typename Functor, typename... Args>
struct invocable_traits
    : std::conditional_t<is_weak_invocable_v<Functor>, detail::weak_invocable_traits<Functor>,
                         detail::explicit_invocable_traits<Functor, Args...>> {
    using is_explicit_invocable = is_explicit_invocable<Functor, Args...>;
    using functor_type = Functor;
    using given_args_type       = std::tuple<Args...>;
    using given_arity           = std::tuple_size<given_args_type>;
    using is_function           = std::is_function<detail::invocable_decay_t<Functor>>;
    using is_member_function_pointer =
        std::is_member_function_pointer<detail::invocable_decay_t<Functor>>;
    using is_function_object =
        std::conjunction<std::negation<std::disjunction<is_function, is_member_function_pointer>>,
                         ::ext::is_invocable<Functor, Args...>>;
    using invocable_type = std::conditional_t<
        is_function::value, detail::function_tag,
        std::conditional_t<is_member_function_pointer::value, detail::member_function_tag,
                           detail::function_object_tag>>;
};

namespace detail {
template <std::size_t A, std::size_t B>
struct max {
    static constexpr std::size_t value = A >= B ? A : B;
};

template <typename DST_TUP, typename SRC_TUP>
struct zip {
    using dst_tuple = DST_TUP;
    using src_tuple = SRC_TUP;
};

template <std::size_t A, std::size_t B>
struct less {
    static constexpr bool value = A < B;
};

template <std::size_t A, std::size_t B>
static constexpr bool less_v = less<A, B>::value;

template <std::size_t I, typename TUPLE>
struct tuple_element : std::conditional<less_v<I, std::tuple_size_v<TUPLE>>, std::tuple_element_t<I, TUPLE>, void> {};

template <std::size_t I, typename TUPLE>
using tuple_element_t = typename tuple_element<I, TUPLE>::type;

template <std::size_t I, typename ZIP>
struct zip_element {
    using type = std::pair<tuple_element_t<I, typename ZIP::dst_tuple>, tuple_element_t<I, typename ZIP::src_tuple>>;
};

template <std::size_t I, typename ZIP>
using zip_element_t = typename zip_element<I, ZIP>::type;

template <typename DST_ARG, typename SRC_ARG>
struct is_pair_args_packable : std::false_type {};

template <typename ZIPPED_ARGS>
struct is_args_packable;

template <>
struct is_args_packable<std::tuple<>> : std::true_type {};

template <typename FIRST_DST, typename... REST>
struct is_args_packable<std::tuple<std::pair<FIRST_DST, void>, REST...>> : std::false_type {};

template <typename FIRST_SRC, typename... REST>
struct is_args_packable<std::tuple<std::pair<void, FIRST_SRC>, REST...>> : std::false_type {};

template <typename FIRST_DST, typename FIRST_SRC, typename... REST>
struct is_args_packable<std::tuple<std::pair<FIRST_DST, FIRST_SRC>, REST...>> : std::conjununction<is_pair_args_packable<FIRST_DST, FIRST_SRC>, is_args_packable<std::tuple<REST...>>> {};

template<typename FUNCTOR, typename INVOCABLE_TYPE, bool IS_VARARG, typename DST_ARGS, typename SRC_ARGS>
struct is_packable_helper : std::false_type {};
template<typename FUNCTOR, typename DST_ARGS, typename SRC_ARGS>
struct is_packable_helper<FUNCTOR, function_tag, false, DST_ARGS, SRC_ARGS> {
};

template<bool IS_INVOCABLE, typename INVOCABLE_TRAITS>
struct is_packable : std::false_type {};

template<typename INVOCABLE_TRAITS>
struct is_packable<true, INVOCABLE_TRAITS> : is_packable_helper<typename INVOCABLE_TRAITS::functor_type, typename INVOCABLE_TRAITS::invocable_type, typename INVOCABLE_TRAITS::is_vararg::value, typename INVOCABLE_TRAITS::args_type, typename INVOCABLE_TRAITS::given_args_type> {
};
}


template <typename INVOCABLE_TRAITS>
struct is_packable : detail::is_packable<typename INVOCABLE_TRAITS::is_invocable::value, INVOCABLE_TRAITS> {};


// namespace detail {
// template <typename Tuple1, typename Tuple2>
// struct is_convertible;

// template<typename Functor, typename... Args>
// struct is_explicit_executable : std::conjunction<::ext::is_explicit_invocable<Functor, Args...>, > {};
// }

template <typename Functor, typename... Args> struct is_executable : std::conjunction<::ext::is_invocable<Functor, Args...>, ::ext::is_packable<invocable_traits<Functor, Args...>>> {};

} // namespace ext
