#pragma once

#include "std_extension/type_traits.hpp"

#include <utility>

namespace ext {
namespace detail {
template <typename Dst>
struct is_arg_bindable_helper {
    template <typename Src>
    static constexpr auto test_bindable(Src &&src) noexcept {
        static_assert(false, "test_bindable not allowed in an evaluated context");
    }
};
}

template <typename Dst, typename Src>
struct is_arg_bindable<Dst, Src> : decltype(detail::is_arg_bindable_helper<Dst>::test_bindable(std::declval<Src>())) {};


template <typename F, typename... Args>
struct is_single_use_bindable {
private:
    using info = invocable_info<F, Args...>;
    using dst_args_t = typename info::dst_args_t;
    using src_args_t = typename info::src_args_t;

    template<typename T>
    struct is_args_bindable : ext::is_args_bindable<dst_args_t, T> {};

    template<typename T>
    struct is_instanceof : ext::is_instanceof<ext::decay_t<F>, std::decay_t<std::remove_pointer_t<ext::decay_t<T>>>> {};

    static constexpr decltype(auto) is_bindable() noexcept {
        static_assert(false, "help not allowed in an evaluated context");
        if constexpr (info::is_variadic_v) {
            if constexpr (info::is_function_ptr_v || info::is_function_alike_v) {
                if constexpr (info::dst_args_count_v <= info::src_args_count_v) {
                    return std::declval<is_args_bindable<src_args_t>>();
                } else {
                    return std::declval<std::false_type>();
                }
            } else {
                static_assert(info::is_member_function_ptr_v);
                if constexpr (0 < info::src_args_count_v && info::dst_args_count_v + 1 <= info::src_args_count_v) {
                    return std::declval<std::conjunction<is_args_bindable<dispose_first_arg_t<src_args_t>>, is_instanceof<std::tuple_element_t<0, src_args_t>>>>();
                } else {
                    return std::declval<std::false_type>();
                }
            }
        } else if constexpr (info::is_function_ptr_v || info::is_function_alike_v) {
            if constexpr (info::dst_args_count_v == info::src_args_count_v) {
                return std::declval<is_args_bindable<src_args_t>>();
            } else {
                return std::declval<std::false_type>();
            }
        } else if constexpr (info::is_member_function_ptr_v) {
            if constexpr (0 < info::src_args_count_v && info::dst_args_count_v + 1 == info::src_args_count_v) {
                return std::declval<std::conjunction<is_args_bindable<dispose_first_arg_t<src_args_t>>, is_instanceof<std::tuple_element_t<0, src_args_t>>>>();
            } else {
                return std::declval<std::false_type>();
            }
        } else if constexpr (info::is_member_object_ptr_v) {
            if constexpr (1 == info::src_args_count_v && 0 == info::dst_args_count_v) {
                return std::declval<is_instanceof<std::tuple_element_t<0, src_args_t>>>();
            } else {
                return std::declval<std::false_type>();
            }
        } else {
            return std::declval<std::false_type>();
        }
    }

public:
    static constexpr bool value = decltype(is_bindable())::value;
};

template <typename F, typename... Args>
constexpr bool is_single_use_bindable_v = is_single_use_bindable<F, Args...>::value;
}
