#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

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

template <typename T, typename Class> struct member_traits<T Class::*> : function_traits<T> {
    using class_type = Class;
};

template <typename Signature> struct weak_function_object_traits {
    using is_invocable = std::false_type;
};

template <typename Functor>
struct weak_function_object_traits : member_traits<decltype(&Functor::operator())> {
    using functor_type = Functor;
};

enum class weak_invocable {
    FUNCTION,
    MEMBER_FUNCTION,
    FUNCTION_OBJECT,
};

template <typename Functor>
struct weak_invocable_decay : std::remove_pointer<std::decay_t<Functor>> {};

template <typename Functor>
using weak_invocable_decay_t = typename weak_invocable_decay<Functor>::type;

template <typename Functor, typename Weak_invocable_type> struct weak_invocable_traits_helper;

template <typename Functor>
struct weak_invocable_traits_helper<
    Functor, std::integral_constant<weak_invocable, weak_invocable::FUNCTION>>
    : function_traits<weak_invocable_decay_t<Functor>> {};

template <typename Signature>
struct weak_invocable_traits_helper<
    Signature, std::integral_constant<weak_invocable, weak_invocable::MEMBER_FUNCTION>>
    : member_traits<Signature> {};

template <typename Functor>
struct weak_invocable_traits_helper<
    Functor, std::integral_constant<weak_invocable, weak_invocable::FUNCTION_OBJECT>>
    : weak_function_object_traits<Functor> {};

template <typename Functor, typename = decltype(&Functor::operator())>
auto get_weak_invocable_type(int)
    -> std::integral_constant<weak_invocable, weak_invocable::FUNCTION_OBJECT>;

template <typename Functor>
auto get_weak_invocable_type(...)
    -> std::conditional_t<std::is_member_function_pointer_v<Functor>,
                          std::integral_constant<weak_invocable, weak_invocable::MEMBER_FUNCTION>,
                          std::integral_constant<weak_invocable, weak_invocable::FUNCTION>>;

template <typename Functor, typename Weak_functor = weak_invocable_decay_t<Functor>>
struct weak_invocable_traits
    : weak_invocable_traits_helper<Weak_functor,
                                   decltype(get_weak_invocable_type<Weak_functor>(0))> {};

template <bool Is_explicit_invocable, typename Functor, typename... Args>
struct explicit_invocable_traits_helper {
    using is_invocable = std::false_type;
};

template <typename Functor, typename... Args>
struct explicit_invocable_traits_helper<true, Functor, Args...>
    : function_traits_base<std::invoke_result_t<std::decay_t<Functor>, Args...>, Args...>,
      member_traits<Functor> {
    using is_invocable         = std::true_type;
    using vararg               = std::false_type;
    using is_nothrow_invocable = std::is_nothrow_invocable<std::decay_t<Functor>, Args...>;
};

template <typename Functor, typename... Args>
struct explicit_invocable_traits
    : explicit_invocable_traits_helper<
          std::conjunction_v<
              std::negation<std::disjunction<
                  std::is_member_object_pointer<std::decay_t<Functor>>,
                  typename weak_invocable_traits<weak_invocable_decay_t<Functor>>::is_invocable>>,
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
                         detail::explicit_invocable_traits<Functor, Args..>> {
    using given_args_type            = std::tuple<Args...>;
    using given_arity                = std::tuple_size<given_args_type>;
    using is_member_function_pointer = std::is_member_function_pointer<std::decay_t<Functor>>;
    using is_function                = std::is_function<detail::weak_invocable_decay_t<Functor>>;
    using is_function_object =
        std::conjunction<::ext::is_invocable<Functor, Args...>,
                         std::negation<std::disjunction<is_member_function_pointer, is_function>>>;
};
} // namespace ext
