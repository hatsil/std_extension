#pragma once

#include <type_traits>
#include <tuple>
#include <cstddef>
#include <functional>
#include <utility>

namespace ext {
// utility
template<typename>
struct is_reference_wrapper : std::false_type {};

template<typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

template<typename T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

namespace detail {
template <typename, bool>
struct decay_helper;

template <typename T>
struct decay_helper<T, false> : std::type_identity<T> {};

template <typename T>
struct decay_helper<std::reference_wrapper<T>, true> : decay_helper<std::decay_t<T>, is_reference_wrapper_v<std::decay_t<T>>> {};
}

template <typename T>
struct decay : detail::decay_helper<std::decay_t<T>, is_reference_wrapper_v<std::decay_t<T>>> {};

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

template <typename...>
struct disjunction;

template <typename T>
struct disjunction<T> : std::disjunction<T> {};

template <typename First, typename Second, typename... Rest>
struct disjunction<First, Second, Rest...> : std::conditional_t<First::value, First, ext::disjunction<Second, Rest...>> {};

template <typename... Ts>
using disjunction_t = typename ext::disjunction<Ts...>::type;

template <typename BoolConstant, typename T>
struct predicate : BoolConstant , T {};

template <typename, typename>
struct add_first;

template <typename First, typename... Rest>
struct add_first<First, std::tuple<Rest...>> : std::type_identity<std::tuple<Fist, Rest...>> {};

template <typename First, typename Tuple>
using add_first_t = typename add_first<First, Tuple>::type;

namespace detail {
// invocable type traits
enum class invocable_type {
    FUNCTION_PTR,
    FUNCTION_OBJECT,
    MEMBER_FUNCTION_PTR,
    MEMBER_OBJECT_PTR,
};

template <invocable_type I>
struct invocable_type_int : std::integral_constant<invocable_type, I> {};

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

template <typename Ret, typename DstArgs, cv_ref_qualification Qual = cv_ref_qualification::UNDETERMINED, bool IsNoexcept = false, bool IsVariadic = false, bool IsInvocable = true>
struct invocable_essentials {
    using is_invocable = std::bool_constant<IsInvocable>;
    using result_t = Ret;
    using dst_args_t = DstArgs;
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

template <typename InvEssentials, typename SrcArgs, invocable_type InvocableType, bool IsUnwrapped>
struct invocable_info_meta {
    using is_invocable = typename InvEssentials::is_invocable;
    using result_t = typename InvEssentials::result_t;
    using dst_args_t = typename InvEssentials::dst_args_t;
    using dst_args_count = typename InvEssentials::dst_args_count;
    using src_args_t = SrcArgs;
    using src_args_count = std::tuple_size<src_args_t>;
    using is_noexcept = typename InvEssentials::is_noexcept;
    using is_variadic = typename InvEssentials::is_variadic;

    // invocable type
    using is_function_ptr = is_any_of<invocable_type, InvocableType, invocable_type::FUNCTION_PTR>;
    using is_function_alike = is_any_of<invocable_type, InvocableType, invocable_type::FUNCTION_OBJECT>;
    using is_member_function_ptr = is_any_of<invocable_type, InvocableType, invocable_type::MEMBER_FUNCTION_PTR>;
    using is_member_object_ptr = is_any_of<invocable_type, InvocableType, invocable_type::MEMBER_OBJECT_PTR>;

    // cv ref qualification
    using is_unqualified = typename InvEssentials::is_unqualified;
    using is_const = typename InvEssentials::is_const;
    using is_volatile = typename InvEssentials::is_volatile;
    using is_lvalue_reference = typename InvEssentials::is_lvalue_reference;
    using is_rvalue_reference = typename InvEssentials::is_rvalue_reference;

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
    static constexpr invocable_type invocable_type_v = InvocableType;

    // cv ref qualification
    static constexpr bool is_unqualified_v = is_unqualified::value;
    static constexpr bool is_const_v = is_const::value;
    static constexpr bool is_volatile_v = is_volatile::value;
    static constexpr bool is_lvalue_reference_v = is_lvalue_reference::value;
    static constexpr bool is_rvalue_reference_v = is_rvalue_reference_v::value;

    static constexpr bool is_unwrapped_v = is_unwrapped::value;
};

template<typename>
struct implicit_invocable_info_helper : invocable_essentials<void, std::tuple<>, cv_ref_qualification::UNDETERMINED, true, false, false> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...)> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args...) const volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE, true> {};

// variadic C-style functions
template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......)> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const volatile> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const volatile &> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const volatile &&> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE, false, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::UNQUALIFIED, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const volatile noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::CONST_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const volatile & noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::LVALUE_REF_CONST_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_VOLATILE, true, true> {};

template<typename Ret, typename... Args>
struct implicit_invocable_info_helper<Ret(Args......) const volatile && noexcept> : invocable_essentials<Ret, std::tuple<Args...>, cv_ref_qualification::RVALUE_REF_CONST_VOLATILE, true, true> {};

template<typename Class, typename Pointed>
struct implicit_invocable_info_helper<Pointed Class:: *> : std::conditional_t<std::is_member_function_pointer_v<Pointed Class:: *>, implicit_invocable_info_helper<Pointed>, invocable_essentials<Pointed, std::tuple<>, cv_ref_qualification::UNDETERMINED, true>> {};

template <typename T, typename U = decltype(&T::operator())>
struct implicit_invocable_info_helper : implicit_invocable_info_helper<U> {};

template <typename T>
struct implicit_invocable_info : implicit_invocable_info_helper<std::remove_pointer_t<ext::decay_t<T>>> {};

template <typename T>
using is_implicit_invocable = typename implicit_invocable_info<T>::is_invocable;

template <typename T>
constexpr bool is_implicit_invocable_v = is_implicit_invocable<T>::value;

template <bool, typename, typename...>
struct explicit_invocable_info_helper;

template <typename F, typename... Args>
struct explicit_invocable_info_helper<false, F, Args...> : invocable_essentials<void, std::tuple<Args...>, cv_ref_qualification::UNDETERMINED, true, false, false> {};

template <typename F, typename... Args>
struct explicit_invocable_info_helper<true, F, Args...> : invocable_essentials<std::invoke_result_t<F, Args...>, std::tuple<Args...>, cv_ref_qualification::UNDETERMINED, std::is_nothrow_invocable_v<F, Args...>> {};

template <typename T, typename... Args>
struct explicit_invocable_info : explicit_invocable_info_helper<std::is_invocable_v<ext::decay_t<T>, Args...>, ext::decay_t<T>, Args...> {};

template <typename T, typename F = std::remove_pointer_t<ext::decay_t<T>>>
struct implicit_invocable_type : ext::disjunction_t<ext::predicate<std::is_function<F>, invocable_type_int<invocable_type::FUNCTION_PTR>>,
                                    ext::predicate<std::is_member_function_pointer<F>, invocable_type_int<invocable_type::MEMBER_FUNCTION_PTR>>,
                                    ext::predicate<std::is_member_object_pointer<F>, invocable_type_int<invocable_type::MEMBER_OBJECT_PTR>>,
                                    invocable_type_int<invocable_type::FUNCTION_OBJECT>> {};

template <typename T>
constexpr invocable_type implicit_invocable_type_v = implicit_invocable_type<T>::value;

template <bool, typename, typename...>
struct invocable_info_helper;

template <typename T, typename... Args>
struct invocable_info_helper<false, T, Args...> : invocable_info_meta<explicit_invocable_info<T, Args...>, std::tuple<Args...>, invocable_type::FUNCTION_OBJECT, is_reference_wrapper_v<std::remove_cvref_t<T>>> {};

template <typename T, typename... Args>
struct invocable_info_helper<true, T, Args...> : invocable_info_meta<implicit_invocable_info<T>, std::tuple<Args...>, implicit_invocable_type_v<T>, is_reference_wrapper_v<std::remove_cvref_t<T>>> {};
}

template <typename F, typename... Args>
struct invocable_info : detail::invocable_info_helper<detail::is_implicit_invocable_v<F>, F, Args...> {};

template <typename F, typename... Args>
struct invocable_result : std::type_identity<typename invocable_info<F, Args...>::result_t> {};

template <typename F, typename... Args>
using invocable_result_t = typename invocable_result<F, Args...>::type;

namespace detail {
template <typename T, typename U = std::remove_reference_t<T>>
struct to_cstyle : std::conditional<std::is_array_v<U>, std::decay_t<U>, U> {};

template <typename T>
using to_cstyle_t = typename to_cstyle<T>::type;

template<bool /* Predicate */, std::size_t /* I */, typename /* DstTuple */, typename /* SrcTuple */>
struct bind_element_helper;

template <std::size_t I, typename DstTuple, typename SrcTuple>
struct bind_element_helper<true, I, DstTuple, SrcTuple> : std::type_identity<std::pair<std::tuple_element_t<I, DstTuple>, std::tuple_element_t<I, SrcTuple>>> {};

template <std::size_t I, typename DstTuple, typename SrcTuple>
struct bind_element_helper<false, I, DstTuple, SrcTuple> : std::type_identity<std::pair<to_cstyle_t<std::tuple_element_t<I, SrcTuple>>, std::tuple_element_t<I, SrcTuple>>> {};

template <std::size_t I, typename DstTuple, typename SrcTuple>
struct bind_element : bind_element_helper<(I < std::tuple_size_v<DstTuple>), I, DstTuple, SrcTuple> {};

template <std::size_t I, typename DstTuple, typename SrcTuple>
using bind_element_t = typename bind_element<I, DstTuple, SrcTuple>::type;

template <typename /* DstTuple */, typename /* SrcTuple */, typename /* IndexSequence */>
struct zip_for_binding_helper;

template <typename DstTuple, typename SrcTuple, std::size_t... Idx>
struct zip_for_binding_helper<DstTuple, SrcTuple, std::index_sequence<Idx...>> : std::type_identity<std::tuple<bind_element_t<Idx, DstTuple, SrcTuple>...>> {};

template <typename DstTuple, typename SrcTuple>
struct zip_for_binding : zip_for_binding_helper<DstTuple, SrcTuple, std::make_index_sequence<std::tuple_size_v<SrcTuple>>> {};

template <typename DstTuple, typename SrcTuple>
using zip_for_binding_t = typename zip_for_binding<DstTuple, SrcTuple>::type;

template <typename Dst, typename Src, typename T = std::remove_reference_t<Src>, typename U = std::conditional_t<std::is_array_v<T>, std::decay_t<T>, Src>>
struct is_arg_bindable : std::is_convertible<U, Dst> {};

template <typename>
struct is_bindable;

template <>
struct is_bindable<std::tuple<>> : std::true_type {};

template <typename Dst, typename Src, typename... Rest>
struct is_bindable<std::tuple<std::pair<Dst, Src>, Rest...>> : std::conjunction<is_arg_bindable<Dst, Src>, is_bindable<std::tuple<Rest...>>> {};

template <bool /* Predicate */, typename /* DstTuple */, typename /* SrcTuple */>
struct is_executable_helper : std::false_type {};

template <typename DstTuple, typename SrcTuple>
struct is_executable_helper<true, DstTuple, SrcTuple> : is_bindable<zip_for_binding_t<DstTuple, SrcTuple>> {};

template <bool /* IsVariadic */, invocable_type, typename /* F */, typename /* Dst */, typename /* Src */>
struct is_executable : std::false_type {};

template <typename F, typename DstTuple, typename SrcTuple>
struct is_executable<false, invocable_type::FUNCTION_OBJECT, F, DstTuple, SrcTuple> : is_executable_helper<std::tuple_size_v<DstTuple> == std::tuple_size_v<SrcTuple>, ext::add_first_t<F, DstTuple>, ext::add_first_t<F, SrcTuple>> {};

template <typename F, typename DstTuple, typename SrcTuple>
struct is_executable<false, invocable_type::FUNCTION_PTR, F, DstTuple, SrcTuple> : is_executable<false, invocable_type::FUNCTION_OBJECT, F, DstTuple, SrcTuple> {};

template <typename MemberFunction, typename DstTuple, typename Instance, typename... SrcRest>
struct is_executable<false, invocable_type::MEMBER_FUNCTION_PTR, MemberFunction, DstTuple, std::tuple<Instance, SrcRest...>> : std::conjunction<is_memberof<ext::decay_t<MemberFunction>, ext::decay_t<Instance>>, is_executable_helper<std::tuple_size_v<DstTuple> == sizeof...(SrcRest), ext::add_first_t<MemberFunction, ext::add_first_t<Instance, DstTuple>>, std::tuple<MemberFunction, Instance, SrcRest...>>> {};

template <typename MemberObject, typename Instance>
struct is_executable<false, invocable_type::MEMBER_OBJECT_PTR, MemberObject, std::tuple<>, std::tuple<Instance>> : detail::is_executable<false, invocable_type::MEMBER_FUCNTION_PTR, MemberObject, std::tuple<>, std::tuple<Instance>> {};

template <typename F, typename DstTuple, typename SrcTuple>
struct is_executable<true, invocable_type::FUNCTION_OBJECT, F, DstTuple, SrcTuple> : is_executable_helper<(std::tuple_size_v<DstTuple> <= std::tuple_size_v<SrcTuple>), ext::add_first_t<F, DstTuple>, ext::add_first_t<F, SrcTuple>> {};

template <typename F, typename DstTuple, typename SrcTuple>
struct is_executable<true, invocable_type::FUNCTION_PTR, F, DstTuple, SrcTuple> : is_executable<true, invocable_type::FUNCTION_OBJECT, F, DstTuple, SrcTuple> {};

template <typename MemberFunction, typename DstTuple, typename Instance, typename... SrcRest>
struct is_executable<true, invocable_type::MEMBER_FUNCTION_PTR, MemberFunction, DstTuple, std::tuple<Instance, SrcRest...>> : std::conjunction<is_memberof<ext::decay_t<MemberFunction>, ext::decay_t<Instance>>, is_executable_helper<(std::tuple_size_v<DstTuple> <= sizeof...(SrcRest)), ext::add_first_t<MemberFunction, ext::add_first_t<Instance, DstTuple>>, std::tuple<MemberFunction, Instance, SrcRest...>>> {};

template <bool IsVariadic, invocable_type InvocableType, typename F, typename Dst, typename Src>
constexpr bool is_executable_v = is_executable<IsVariadic, InvocableType, F, Dst, Src>::value;
}

template <typename F, typename... Args>
struct is_executable {
private:
    using exec_info = invocable_info<F, Args...>;

public:
    static constexpr bool value = detail::is_executable_v<exec_info::is_variadic_v, exec_info::invocable_type_v, F, typename exec_info::dst_args_t, typename exec_info::src_args_t>;
};

template <typename F, typename... Args>
constexpr bool is_executable_v = is_executable<F, Args...>::value;
}
