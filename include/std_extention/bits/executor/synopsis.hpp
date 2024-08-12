#pragma once

#include "std_extention/blocking_deque.hpp"

#include <atomic>
#include <functional>
#include <future>
#include <thread>
#include <type_traits>
#include <vector>

namespace ext {
class executor final {
public:
    executor(std::size_t nthreads = 1);

    executor(const executor &)            = delete;
    executor &operator=(const executor &) = delete;

    ~executor();

    template <class F, class... Args>
    [[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    emplace_back(F &&f, Args &&...args);

    template <class F, class... Args>
    [[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    emplace_front(F &&f, Args &&...args);

    void shutdown();
    void forced_shutdown();

private:
    enum class EmplaceAt {
        BACK,
        FRONT,
    };

    template <class F, class... Args>
    [[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    emplace(EmplaceAt position, F &&f, Args &&...args);

    enum class ShutdownPolicy {
        FORCED,
        GRACEFUL,
    };
    void shutdown(const ShutdownPolicy policy);

    enum class State {
        CONTINUE,
        STOP,
    };
    std::atomic_long                                 m_activeness;
    std::vector<std::thread>                         m_workers;
    blocking_deque<std::move_only_function<State()>> m_tasks;
};
} // namespace ext
