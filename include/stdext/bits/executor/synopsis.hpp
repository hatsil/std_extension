#pragma once

#include "../../blocking_deque.hpp"

#include <cstdint>
#include <future>
#include <type_traits>
#include <string>
#include <vector>
#include <thread>
#include <iostream>
#include <ostream>
#include <atomic>
#include <functional>

namespace stdext {
class executor {
public:
    explicit executor(std::size_t nthreads);

    executor(const executor &) = delete;
    executor &operator=(const executor &) = delete;

    ~executor();

    template<class F, class... Args>
    [[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    emplace_back(F &&f, Args &&...args);

    template<class F, class... Args>
    [[nodiscard]] std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    emplace_front(F &&f, Args &&...args);

    void shutdown();
    void forced_shutdown();

private:
    enum class EmplaceAt {
        BACK,
        FRONT,
    };

    template<class F, class... Args>
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
    std::atomic<std::ptrdiff_t> m_activeness;
    std::vector<std::thread> m_workers;
    blocking_deque<std::move_only_function<State()>> m_tasks;
};
}
