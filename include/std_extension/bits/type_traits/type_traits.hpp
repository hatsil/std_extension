#pragma once

#include <type_traits>
#include <tuple>
#include <cstddef>
#include <functional>

namespace ext {
// utility
struct failure_type {};

template <typename>
struct is_failure_type : std::false_type {};

template <>
struct is_failure_type<failure_type> : std::true_type {};

template <typename T>
constexpr bool is_failure_type_v = ie_failure_type<T>::value;

template<typename>
struct is_reference_wrapper : std::false_type {};

template<typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

template<typename T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

template<typename, typename>
struct is_instanceof : std::false_type {};

template<typename Base, typename Pointed, typename Derived>
struct is_instanceof<Pointed Base:: *, Derived> : std::is_base_of<Base, Derived> {};

template<typename T>
constexpr bool is_instanceof_v = is_instanceof<T>::value;

template <typename>
struct unwrap;

template <typename T>
struct unwrap<std::reference_wrapper<T>> : unwrap<T> {};

template <typename T>
struct unwrap<T> : std::conditional_t<is_reference_wrapper_v<std::decay_t<T>>, unwrap<std::decay_t<T>>, std::type_identity<T>> {};

template <typename T>
using unwrap_t = typename unwrap<T>::type;

template <typename T>
struct decay : std::decay<unwrap_t<T>> {};

template <typename T>
using decay_t = typename ext::decay<T>::type;

template<typename T, T...>
struct is_any_of;

template<typename T, T V>
struct is_any_of<T, V> : std::false_type {};

template<typename T, T V, T... Vs>
struct is_any_of<T, V, V, Vs...> : std::true_type {};

template<typename T, T V1, T V2, T... Vs>
struct is_any_of<T, V1, V2, Vs...> : is_any_of<T, V1, Vs...> {};

template<typename T, T V, T... Vs>
constexpr bool is_any_of_v = is_any_of<T, V, Vs...>::value;

// std::tuple type traits
template <typename>
struct is_std_tuple : std::false_type {};

template <typename... Args>
struct is_std_tuple<std::tuple<Args...>> : std::true_type {};

template <typename T>
constexpr bool is_std_tuple_v = is_std_tuple<T>::value;

template <typename T>
requires is_std_tuple_v<T>
struct latest_arg;

template <>
struct latest_arg<std::tuple<>> : std::type_identity<void> {};

template <typename Arg>
struct latest_arg<std::tuple<Arg>> : std::type_identity<Arg> {};

template <typename Fist, typename Second, typename... Rest>
struct latest_arg<std::tuple<Fist, Second, Rest...>> : latest_arg<std::tuple<Second, Rest...>> {};

template<typename T>
requires is_std_tuple_v<T>
using latest_arg_t = typename latest_arg<T>::type;

template <typename>
struct dispose_latest_arg;

template <>
struct dispose_latest_arg<std::tuple<>> : std::type_identity<std::tuple<>> {};

template <typename Arg>
struct dispose_latest_arg<std::tuple<Arg>> : dispose_latest_arg<std::tuple<>> {};

template <typename First, typename Second, typename... Rest, typename Suffix = typename dispose_latest_arg<std::tuple<Second, Rest...>>::type>
struct dispose_latest_arg<std::tuple<Fist, Second, Rest...>> : std::invoke_result<&std::tuple_cat<std::tuple<First>, Suffix>, std::tuple<First>, Suffix> {};

template <typename T>
requires is_std_tuple_v<T>
using dispose_latest_arg_t = typename dispose_latest_arg<T>::type;

template <typename T>
requires is_std_tuple_v<T>
struct dispose_first_arg;

template <>
struct dispose_first_arg<std::tuple<>> : std::type_identity<std::tuple<>> {};

template <typename First, typename... Rest>
struct dispose_first_arg<std::tuple<First, Rest...>> : std::type_identity<std::tuple<Rest...>> {};

template <typename T>
requires is_std_tuple_v<T>
using dispose_first_arg_t = typename dispose_first_arg<T>::type;

// invocable type traits
enum class invocable_type {
    NOT_INVOCABLE,
    FUNCTION_PTR,
    FUNCTION_OBJECT,
    MEMBER_FUNCTION_PTR,
    MEMBER_OBJECT_PTR,
};

enum class cv_ref_qualification {
    UNQUALIFIED,
    CONST,
    VOLATILE,
    CONST_VOLATILE,
    LVALUE_REF,
    LVALUE_REF_CONST,
    LVALUE_REF_VOLATILE,
    LVALUE_REF_CONST_VOLATILE,
    RVALUE_REF,
    RVALUE_REF_CONST,
    RVALUE_REF_VOLATILE,
    RVALUE_REF_CONST_VOLATILE,
    UNDETERMINED,
};

template <typename T>
struct is_pointer : std::is_pointer<std::remove_cvref_t<unwrap_t<T>>> {};

template <typename T>
struct is_function_ptr : std::conjunction<ext::is_pointer<T>, std::is_function<std::remove_pointer_t<ext::decay_t<T>>>> {};

template <typename T>
constexpr bool is_function_ptr_v = is_function_ptr<T>::value;

template <typename Ret, typename DstArgs, cv_ref_qualification Qual = cv_ref_qualification::UNDETERMINED, bool IsNoexcept = false, bool IsVariadic = false>
requires is_std_tuple_v<std::decay_t<DstArgs>>
struct invocable_essentials {
    using result_t = Ret;
    using dst_args_t = std::decay_t<DstArgs>;
    using dst_args_count = std::tuple_size<dst_args_t>;
    using is_noexcept = std::bool_constant<IsNoexcept>;
    using is_variadic = std::bool_constant<IsVariadic>;

    // cv ref qualification
    using is_unqualified = is_any_of<cv_ref_qualification, Qual, cv_ref_qualification::UNQUALIFIED>;
    using is_const = is_any_of<cv_ref_qualification, Qual, cv_ref_qualification::CONST, cv_ref_qualification::CONST_VOLATILE, cv_ref_qualification::LVALUE_REF_CONST, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, cv_ref_qualification::RVALUE_REF_CONST, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE>;
    using is_volatile = is_any_of<cv_ref_qualification, Qual, cv_ref_qualification::VOLATILE, cv_ref_qualification::CONST_VOLATILE, cv_ref_qualification::LVALUE_REF_VOLATILE, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, cv_ref_qualification::RVALUE_REF_VOLATILE, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE>;
    using is_lvalue_reference = is_any_of<cv_ref_qualification, Qual, cv_ref_qualification::LVALUE_REF, cv_ref_qualification::LVALUE_REF_CONST, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE>;
    using is_rvalue_reference = is_any_of<cv_ref_qualification, Qual, cv_ref_qualification::RVALUE_REF, cv_ref_qualification::RVALUE_REF_CONST, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE>;
};

template <typename>
struct is_invocable_essentials : std::false_type {};

template <typename Ret, typename DstArgs, cv_ref_qualification Qual, bool IsNoexcept, bool IsVariadic>
requires is_std_tuple_v<std::decay_t<DstArgs>>
struct is_invocable_essentials<invocable_essentials<Ret, DstArgs, Qual, IsNoexcept, IsVariadic>> : std::true_type {};

template <typename Essentials, typename SrcArgs, invocable_type InvocableType, bool IsUnwrapped>
requires std::conjunction_v<is_invocable_essentials<Essentials>, is_std_tuple<std::decay_t<SrcArgs>>>
struct invocable_info_meta {
    using result_t = typename Essentials::result_t;
    using dst_args_t = typename Essentials::dst_args_t;
    using dst_args_count = typename Essentials::dst_args_count;
    using first_args_t = dispose_latest_arg_t<dst_args_t>;
    using last_arg_t = latest_arg_t<dst_args_t>;
    using src_args_t = std::decay_t<SrcArgs>;
    using src_args_count = std::tuple_size<src_args_t>;
    using is_noexcept = typename Essentials::is_noexcept;
    using is_variadic = typename Essentials::is_variadic;

    // invocable type
    using is_invocable = std::negation<is_any_of<invocable_type, InvocableType, invocable_type::NOT_INVOCABLE>>;
    using is_function_ptr = is_any_of<invocable_type, InvocableType, invocable_type::FUNCTION_PTR>;
    using is_function_alike = is_any_of<invocable_type, InvocableType, invocable_type::FUNCTION_OBJECT>;
    using is_member_function_ptr = is_any_of<invocable_type, InvocableType, invocable_type::MEMBER_FUNCTION_PTR>;
    using is_member_object_ptr = is_any_of<invocable_type, InvocableType, invocable_type::MEMBER_OBJECT_PTR>;

    // cv ref qualification
    using is_unqualified = typename Essentials::is_unqualified;
    using is_const = typename Essentials::is_const;
    using is_volatile = typename Essentials::is_volatile;
    using is_lvalue_reference = typename Essentials::is_lvalue_reference;
    using is_rvalue_reference = typename Essentials::is_rvalue_reference;

    using is_unwrapped = std::bool_constant<IsUnwrapped>;

    // static constexpr values:
    static constexpr std::size_t dst_args_count_v = dst_args_count::value;
    static constexpr std::size_t src_args_count_v = src_args_count::value;
    static constexpr bool is_noexcept_v = is_noexcept::value;
    static constexpr bool is_variadic_v = is_variadic::value;

    // invocable type
    static constexpr bool is_invocable_v = is_invocable::value;
    static constexpr bool is_function_ptr_v = is_function_ptr::value;
    static constexpr bool is_function_alike_v = is_function_alike::value;
    static constexpr bool is_member_function_ptr_v = is_member_function_ptr::value;
    static constexpr bool is_member_object_ptr_v = is_member_object_ptr::value;

    // cv ref qualification
    static constexpr bool is_unqualified_v = is_unqualified::value;
    static constexpr bool is_const_v = is_const::value;
    static constexpr bool is_volatile_v = is_volatile::value;
    static constexpr bool is_lvalue_reference_v = is_lvalue_reference::value;
    static constexpr bool is_rvalue_reference_v = is_rvalue_reference_v::value;

    static constexpr bool is_unwrapped_v = is_unwrapped::value;
};

template<typename>
struct is_implicit_invocable : std::false_type {};

template<typename T, typename = decltype(&T::operator())>
struct is_implicit_invocable<T> : std::true_type {};

template<typename T>
struct is_implicit_invocable<T> : std::disjunction<is_function_ptr<T>, std::is_member_pointer<ext::decay_t<T>>, is_implicit_invocable<ext::decay_t<T>>> {};

template<typename T>
constexpr bool is_implicit_invocable_v = is_implicit_invocable<T>::value;

template<typename T, typename... Args>
struct is_explicit_invocable : std::conjunction<std::negation<is_implicit_invocable<T>>, std::is_invocable<ext::decay_t<T>, Args...>> {};

template<typename T, typename... Args>
constexpr bool is_explicit_invocable_v = is_explicit_invocable<T, Args...>::value;

template<typename T, typename... Args>
struct is_invocable : std::disjunction<is_implicit_invocable<T>, is_explicit_invocable<T, Args...>> {};

template<typename T, typename... Args>
constexpr bool is_invocable_v = ext::is_invocable<T, Args...>::value;

template<typename>
struct implicit_invocable_info;

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...)> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args...) const volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE, true> {};

// variadic C-style functions
template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......)> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info<Ret(Args......) const volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE, true, true> {};

template<typename Class, typename Pointed>
struct implicit_invocable_info<Pointed Class:: *> : std::conditional_t<std::is_member_function_pointer_v<Pointed Class:: *>, implicit_invocable_info<Pointed>, invocable_essentials<Pointed, std::tuple<>, cv_ref_qualification::UNDETERMINED, true>> {};

template <typename T, typename U = decltype(&T::operator())>
struct implicit_invocable_info<T> : implicit_invocable_info<U> {};

template <typename T>
struct implicit_invocable_info<T> : std::conditional_t<is_implicit_invocable_v<T>, implicit_invocable_info<ext::decay_t<T>>, invocable_essentials<void, std::tuple<>, cv_ref_qualification::UNDETERMINED, true>> {};

template <typename T, typename... Args>
struct explicit_invocable_info : std::conditional_t<is_explicit_invocable_v<T, Args...>, invocable_essentials<std::invoke_result_t<ext::decay_t<T>, Args...>, std::tuple<Args...>, cv_ref_qualification::UNDETERMINED, std::is_nothrow_invocable_v<ext::decay_t<T>, Args...>>, invocable_essentials<void, std::tuple<>, cv_ref_qualification::UNDETERMINED, true>> {};

namespace detail {
template <invocable_type Value>
using invocable_type_meta = std::integral_constant<invocable_type, Value>;
}

template <typename T>
struct implicit_invocable_type : std::conditional_t<is_implicit_invocable_v<T>, std::conditional_t<is_function_ptr_v<T>, detail::invocable_type_meta<invocable_type::FUNCTION_PTR>,
                                    std::conditional_t<std::is_member_function_pointer_v<ext::decay_t<T>>, detail::invocable_type_meta<invocable_type::MEMBER_FUNCTION_PTR>,
                                            std::conditional_t<std::is_member_object_pointer_v<ext::decay_t<T>>, detail::invocable_type_meta<invocable_type::MEMBER_OBJECT_PTR>,
                                                detail::invocable_type_meta<invocable_type::FUNCTION_OBJECT>>>>, detail::invocable_type_meta<invocable_type::NOT_INVOCABLE>> {};

template <typename T, typename... Args>
struct explicit_invocable_type<T, Args...> : std::conditional_t<is_explicit_invocable_v<T, Args...>, detail::invocable_type_meta<invocable_type::FUNCTION_OBJECT>,
                                                detail::invocable_type_meta<invocable_type::NOT_INVOCABLE>> {};

template <typename T, typename... Args>
struct to_invocable_type<T, Args...> : std::conditional_t<is_implicit_invocable_v<T>, implicit_invocable_type<T>, explicit_invocable_type<T, Args...>> {};

template <typename T, typename... Args>
constexpr invocable_type to_invocable_type_v = to_invocable_type<T, Args...>::value;

template <typename T, typename... Args>
struct invocable_info : invocable_info_meta<std::conditional_t<is_implicit_invocable_v<T>, implicit_invocable_info<T>, explicit_invocable_info<T, Args...>>, std::tuple<Args...>, to_invocable_type_v<T, Args...>, is_reference_wrapper_v<std::remove_cvref_t<T>>> {};

template <typename F, typename... Args>
struct invocable_result {
    using type = typename invocable_info<F, Args...>::result_t;
};

template <typename F, typename... Args>
using invocable_result_t = typename invocable_result<F, Args...>::type;
}
