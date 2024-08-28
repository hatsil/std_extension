#pragma once

#include "std_extention/blocking_deque.hpp"
#include "std_extention/thread.hpp"

#include <atomic>
#include <concepts>
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
        requires std::invocable<F, Args...>
    [[nodiscard]] std::future<std::invoke_result_t<F, Args...>> emplace_back(F &&f, Args &&...args);

    template <class F, class... Args>
        requires std::invocable<F, Args...>
    [[nodiscard]] std::future<std::invoke_result_t<F, Args...>> emplace_front(F &&f,
                                                                              Args &&...args);

    void                      shutdown();
    void                      forced_shutdown();
    [[nodiscard]] std::size_t nthreads() const noexcept;

private:
    enum class EmplaceAt {
        BACK,
        FRONT,
    };

    enum class ShutdownPolicy {
        FORCED,
        GRACEFUL,
    };

    template <class F, class... Args>
        requires std::invocable<F, Args...>
    [[nodiscard]] std::future<std::invoke_result_t<F, Args...>> emplace(EmplaceAt position, F &&f,
                                                                        Args &&...args);

    void shutdown(ShutdownPolicy policy);

    enum class State {
        CONTINUE,
        STOP,
    };
    std::atomic_long                                 m_activeness;
    std::vector<thread>                              m_workers;
    blocking_deque<std::move_only_function<State()>> m_tasks;
};
} // namespace ext
