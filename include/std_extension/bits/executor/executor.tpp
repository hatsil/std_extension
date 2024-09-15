#pragma once

#include "std_extension/exception.hpp"
#include "synopsis.hpp"

#include <algorithm>
#include <limits>
#include <utility>

namespace ext {
template <class F, class... Args>
    requires std::invocable<F, Args...>
[[nodiscard]] std::future<std::invoke_result_t<F, Args...>> executor::emplace_back(F &&f,
                                                                                   Args &&...args) {
    return emplace(EmplaceAt::BACK, std::forward<F>(f), std::forward<Args>(args)...);
}

template <class F, class... Args>
    requires std::invocable<F, Args...>
[[nodiscard]] std::future<std::invoke_result_t<F, Args...>>
executor::emplace_front(F &&f, Args &&...args) {
    return emplace(EmplaceAt::FRONT, std::forward<F>(f), std::forward<Args>(args)...);
}

namespace detail {
template <typename T> decltype(auto) bind_forward(std::remove_reference_t<T> &t) {
    if constexpr (std::is_lvalue_reference_v<T>) {
        if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
            return std::cref(t);
        } else {
            return std::ref(t);
        }
    } else {
        return std::forward<T>(t);
    }
}
} // namespace detail

template <class F, class... Args>
    requires std::invocable<F, Args...>
[[nodiscard]] std::future<std::invoke_result_t<F, Args...>>
executor::emplace(EmplaceAt position, F &&f, Args &&...args) {
    long expected = 0;
    while (!m_activeness.compare_exchange_weak(expected, std::max(expected, expected + 1))) {
        if (0 > expected) {
            throw exception("executor is inactive");
        }
    }

    if (std::numeric_limits<long>::max() == expected) {
        throw exception("executor has reached its max capacity");
    }

    using Result = typename std::invoke_result_t<F, Args...>;
    std::packaged_task<Result()> task(
        std::bind(std::forward<F>(f), detail::bind_forward<Args>(args)...));

    auto res = task.get_future();
    if (EmplaceAt::BACK == position) {
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
    return res;
}
} // namespace ext
