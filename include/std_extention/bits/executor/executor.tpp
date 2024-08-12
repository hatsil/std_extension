#pragma once

#include "std_extention/exception.hpp"
#include "synopsis.hpp"

namespace ext {
template <class F, class... Args>
[[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
executor::emplace_back(F &&f, Args &&...args) {
    return emplace(EmplaceAt::BACK, std::forward<F>(f), std::forward<Args>(args)...);
}

template <class F, class... Args>
[[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
executor::emplace_front(F &&f, Args &&...args) {
    return emplace(EmplaceAt::FRONT, std::forward<F>(f), std::forward<Args>(args)...);
}

template <class F, class... Args>
[[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
executor::emplace(EmplaceAt position, F &&f, Args &&...args) {
    long expected = 0;
    while (!m_activeness.compare_exchange_weak(expected, expected + 1)) {
        if (0 > expected) {
            throw exception("executor is inactive");
        }
    }

    using Result = typename std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;
    auto task =
        std::packaged_task<Result()>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

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
