#pragma once

#include "std_extension/type_traits.hpp"
#include "std_extension/concepts.hpp"

#include <utility>

namespace ext {
namespace detail {
template <typename Fun, typename ArgsTuple>
struct packaged_executable_helper;
}

template <typename F, typename... Args>
class packaged_executable : public detail::packaged_executable_helper<detail::to_fun_t<F>, detail::packed_args_t<F, Args...>> {
private:
    using base = detail::packaged_executable_helper<detail::to_fun_t<F>, detail::packed_args_t<F, Args...>>;

public:
    template <typename First, typename... Rest>
    constexpr explicit packaged_executable(First &&f, Rest &&...args) noexcept(noexcept(base(std::declval<First>(), std::declval<Rest>()...))) : base(std::forward<First>(f), std::forward<Rest>(args)...) {}
};

template <typename... Args, executable<Args...> F>
constexpr packaged_executable<F, Args...> make_executable(F &&f, Args &&...args) noexcept(noexcept(packaged_executable<F, Args...>(std::declval<F>(), std::declval<Args>()...)));
}
