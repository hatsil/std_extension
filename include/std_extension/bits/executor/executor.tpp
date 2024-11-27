#pragma once

#include "synopsis.hpp"

#include "std_extension/exception.hpp"
#include "std_extension/functional.hpp"

#include <algorithm>
#include <limits>
#include <utility>

namespace ext {
template <class... Args, executable<Args...> F>
[[nodiscard]] std::future<ext::invocable_result_t<F, Args...>> executor::emplace_back(F &&f, Args &&...args) {
    return emplace<false, EmplaceAt::BACK>(std::forward<F>(f), std::forward<Args>(args)...);
}

template <class... Args, executable<Args...> F>
void executor::emplace_back_discard_future(F &&f, Args &&...args) {
    [[maybe_unused]] auto future = emplace<true, EmplaceAt::BACK>(std::forward<F>(f), std::forward<Args>(args)...);
}

template <class... Args, executable<Args...> F>
[[nodiscard]] std::future<ext::invocable_result_t<F, Args...>> executor::emplace_front(F &&f, Args &&...args) {
    return emplace<false, EmplaceAt::FRONT>(std::forward<F>(f), std::forward<Args>(args)...);
}

template <class... Args, executable<Args...> F>
void executor::emplace_front_discard_future(F &&f, Args &&...args) {
    [[maybe_unused]] auto future = emplace<true, EmplaceAt::FRONT>(std::forward<F>(f), std::forward<Args>(args)...);
}

// namespace detail {
// template <typename, typename>
// struct arg_wrapper;

// template <typename FArg, typename Arg>
// struct arg_wrapper<FArg, Arg> {
//     template <decayed_same_as<Arg> UArg>
//     explicit arg_wrapper(UArg &&arg) : m_arg(std::forward<UArg>(arg)) {}

//     arg_wrapper(arg_wrapper &&) = default;

//     FArg get() {
//         return std::move(m_arg);
//     }

//     FArg m_arg;
// };

// template <typename FArg, typename Arg>
// struct arg_wrapper<FArg &, Arg> {
//     template <decayed_same_as<Arg> UArg>
//     explicit arg_wrapper(UArg &&arg) : m_arg(std::forward<UArg>(arg)) {}

//     arg_wrapper(arg_wrapper &&) = default;

//     FArg &get() noexcept {
//         return m_arg;
//     }

//     FArg m_arg;
// };

// template <typename FArg>
// struct arg_wrapper<FArg &, FArg &> {
//     explicit arg_wrapper(FArg &arg) noexcept : m_arg(arg) {}

//     arg_wrapper(arg_wrapper &&) = default;

//     FArg &get() noexcept {
//         return m_arg;
//     }

//     FArg &m_arg;
// };

// template <typename FArg>
// struct arg_wrapper<const FArg &, FArg &> {
//     explicit arg_wrapper(FArg &arg) noexcept : m_arg(arg) {}

//     arg_wrapper(arg_wrapper &&) = default;

//     const FArg &get() const noexcept {
//         return m_arg;
//     }

//     const FArg &m_arg;
// };

// template <typename FArg, typename Arg>
// struct arg_wrapper<FArg &&, Arg> {
//     template <decayed_same_as<Arg> UArg>
//     arg_wrapper(UArg &&arg) : m_arg(std::forward<UArg>(arg)) {}

//     arg_wrapper(arg_wrapper &&) = default;

//     FArg &&get() {
//         return std::move(m_arg);
//     }

//     FArg m_arg;
// };

// template<std::size_t, typename...>
// struct skip_first_n;

// template<typename... Args>
// struct skip_first_n<0, Args...> {
//     using type = std::tuple<std::remove_reference_t<Args...>>;
// };

// template<std::size_t N, typename SkippedArg, typename... RestArgs>
// struct skip_first_n<N, SkippedArg, RestArgs...> : skip_first_n<N-1, RestArgs...> {};

// template<std::size_t N, typename... Args>
// using skip_first_n_t = typename skip_first_n<N, Args...>::type;

// template <class C, class Pointed, class Object, class WrappedArgs, std::size_t... I>
// decltype(auto) invoke_mem_func(Pointed C:: *mem_func, Object &object, WrappedArgs &wrapped_args, std::index_sequence<I...>) {
//     return object. *mem_func(std::get<I>(wrapped_args).get()...);
// }

// template <class F, class WrappedArgs, std::size_t... I>
// decltype(auto) invoke_func(F &f, WrappedArgs &wrapped_args, std::index_sequence<I...>) {
//     return f(std::get<I>(wrapped_args).get()...);
// }

// template <typename desired, typename actual, size_t... I, typename... Args>
// decltype(auto) wrap_args_helper(std::index_sequence<I...>, Args &&...args) {
//     return std::tuple(arg_wrapper<std::tuple_element_t<I, desired>, std::tuple_element_t<I, actual>>(std::forward<Args>(args))...);
// }

// template <typename F, typename... Args>
// decltype(auto) wrap_args(Args &&...args) {
//     using actual = std::tuple<Args...>;
//     if constexpr (ext::is_variadic_v<F>) {
//         if constexpr (std::is_void_v<ext::callable_param_n_t<F>>) {
//             using desired = std::tuple<std::remove_reference_t<Args>...>;
//             wrap_args_helper<desired, actual>(std::make_index_sequence(sizeof...(Args)), std::forward<Args>(args)...);
//         } else {
//             using first_args_tup_t = decltype(std::tuple_cat(std::declval<ext::callable_first_args_t<F>>(), std::declval<std::tuple<std::remove_reference_t<ext::callable_param_n_t<F>>>>()));
//             using rest_args_tup_t = skip_first_n_t<std::tuple_size_v<first_args_tup_t>, Args...>;
//             using desired = decltype(std::tuple_cat(std::declval<first_args_tup_t>(), std::declval<rest_args_tup_t>()));
//             return wrap_args_helper<desired, actual>(std::make_index_sequence(sizeof...(Args)), std::forward<Args>(args)...);
//         }
//     } else {
//         using desired = ext::callable_args_t<F>;
//         return wrap_args_helper<desired, actual>(std::make_index_sequence(sizeof...(Args)), std::forward<Args>(args)...);
//     }
// }

// template <class C, class Pointed, class Object, class... Args>
// decltype(auto) pack_member_function(Pointed C:: *mem_func, Object &&object, Args &&...args) {
//     if constexpr (ext::is_variadic_v<Pointed>) {
//         static_assert(sizeof...(Args) >= ext::args_count_v<Pointed>);
//     } else {
//         static_assert(sizeof...(Args) == ext::args_count_v<Pointed>);
//     }
//     using object_t = std::remove_cvref_t<Object>;
//     constexpr bool is_wrapped = ext::is_reference_wrapper_v<object_t>;
//     constexpr bool is_derived_object = std::is_same_v<C, object_t> || std::is_base_of_v<C, object_t>;
//     constexpr bool is_rvalue_ref = std::is_rvalue_reference_v<Object &&>;
//     if constexpr (is_wrapped) {
//         return pack_member_function(mem_func, object.get(), std::forward<Args>(args)...);
//     } else if constexpr(is_derived_object) {
//         if constexpr (is_rvalue_ref) {
//             return std::packaged_task([mem_func, object = std::forward<Object>(object), wrapped_args = wrap_args<Pointed>(std::forward<Args>(args)...)] mutable {
//                 return invoke_mem_func(mem_func, object, wrapped_args, std::make_index_sequence(sizeof...(Args)));
//             });
//         } else {
//             return std::packaged_task([mem_func, &object, wrapped_args = wrap_args<Pointed>(std::forward<Args>(args)...)] mutable {
//                 return invoke_mem_func(mem_func, object, wrapped_args, std::make_index_sequence(sizeof...(Args)));
//             });
//         }
//     } else {
//         return std::packaged_task([mem_func, object = std::forward<Object>(object), wrapped_args = wrap_args<Pointed>(std::forward<Args>(args)...)] mutable {
//             return invoke_mem_func(mem_func, *object, wrapped_args, std::make_index_sequence(sizeof...(Args)));
//         });
//     }
// }

// template <class C, class Pointed, class Object>
// decltype(auto) pack_member_object(Pointed C:: *member, Object &&object) {
//     using object_t = std::remove_cvref_t<Object>;
//     constexpr bool is_wrapped = ext::is_reference_wrapper_v<object_t>;
//     constexpr bool is_derived_object = std::is_same_v<C, object_t> || std::is_base_of_v<C, object_t>;
//     constexpr bool is_rvalue_ref = std::is_rvalue_reference_v<Object &&>;
//     if constexpr (is_wrapped) {
//         return pack_member_object(member, object.get());
//     } else if constexpr(is_derived_object) {
//         if constexpr (is_rvalue_ref) {
//             return std::packaged_task([member, object = std::forward<Object>(object)] mutable {
//                 return object. *member;
//             });
//         } else {
//             return std::packaged_task([member, &object] mutable {
//                 return object. *member;
//             });
//         }
//     } else {
//         return std::packaged_task([member, object = std::forward<Object>(object)] mutable {
//             return (*object). *member;
//         });
//     }
// }

// template <class F, class... Args>
// decltype(auto) pack_invocable(F &&f, Args &&...args) {
//     if constexpr (ext::is_variadic_v<F>) {
//         static_assert(sizeof...(Args) >= ext::args_count_v<F>);
//     } else {
//         static_assert(sizeof...(Args) == ext::args_count_v<F>);
//     }

//     constexpr bool is_rvalue_ref = std::is_rvalue_reference_v<F &&>;
//     if constexpr (is_rvalue_ref) {
//         return std::packaged_task([f = std::forward<F>(f), wrapped_args = wrap_args<F>(std::forward<Args>(args)...)] mutable {
//             return invoke_func(f, wrapped_args, std::make_index_sequence(sizeof...(Args)));
//         });
//     } else {
//         return std::packaged_task([&f, wrapped_args = wrap_args<F>(std::forward<Args>(args)...)] mutable {
//             return invoke_func(f, wrapped_args, std::make_index_sequence(sizeof...(Args)));
//         });
//     }
// }

// template <class... Args, ext::bindable<Args...> F>
// decltype(auto) pack(F &&f, Args &&...args) {
//     using U = std::decay_t<F>;
//     if constexpr (std::is_member_function_pointer_v<U>) {
//         return pack_member_function(std::forward<F>(f), std::forward<Args>(args)...);
//     } else if constexpr (std::is_member_object_pointer_v<U>) {
//         static_assert(sizeof...(Args) == 1, "member object doesn't take any arguments");
//         return pack_member_object(std::forward<F>(f), std::forward<Args>(args)...);
//     } else {
//         return pack_invocable(std::forward<F>(f), std::forward<Args>(args)...);
//     }
// }
// } // namespace detail

template <bool DiscardFuture, executor::EmplaceAt position, class... Args, executable<Args...> F>
std::future<ext::invocable_result_t<F, Args...>> executor::emplace(F &&f, Args &&...args) {
    long expected = 0;
    while (!m_activeness.compare_exchange_weak(expected, std::max(expected, expected + 1))) {
        if (0 > expected) {
            throw exception("executor is inactive");
        }
    }

    if (std::numeric_limits<long>::max() == expected) {
        throw exception("executor has reached its max capacity");
    }

    auto task = make_executable(std::forward<F>(f), std::forward<Args>(args)...);
    std::future<ext::invocable_result_t<F, Args...>> future;
    if constexpr (!DiscardFuture) {
        future = task.get_future();
    }

    if constexpr (EmplaceAt::BACK == position) {
        m_tasks.emplace_back([task = std::move(task)]() mutable {
            task();
            return State::CONTINUE;
        });
    } else {
        m_tasks.emplace_front([task = std::move(task)]() mutable {
            task();
            return State::CONTINUE;
        });
    }

    --m_activeness;
    return future;
}

template <executor::ShutdownPolicy policy>
void executor::do_shutdown() noexcept try {
    for (long expected = 0; !m_activeness.compare_exchange_weak(expected, -1); expected = 0) {
        if (-2 == expected) {
            return;
        }
        std::this_thread::yield();
    }

    for (;;) try {
            if constexpr (ShutdownPolicy::GRACEFUL == policy) {
                m_tasks.emplace_back([] { return State::STOP; });
            } else {
                m_tasks.emplace_front([] { return State::STOP; });
            }
            break;
        } catch (...) {
            std::this_thread::yield(); // try again
        }

    for (std::thread &worker : m_workers) {
        worker.join();
    }
    m_activeness = -2;
} catch(...) {
    std::terminate();
}
} // namespace ext
