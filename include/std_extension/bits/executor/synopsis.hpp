#pragma once

#include "std_extension/blocking_deque.hpp"
#include "std_extension/concepts.hpp"
#include "std_extension/type_traits.hpp"

#include <atomic>
#include <functional>
#include <future>
#include <thread>
#include <vector>

namespace ext {
class executor final {
public:
    executor(std::size_t nthreads_ = 1);

    executor(const executor &)            = delete;
    executor &operator=(const executor &) = delete;

    ~executor();

    template <class... Args, executable<Args...> F>
    [[nodiscard]] std::future<ext::invocable_result_t<F, Args...>> emplace_back(F &&f, Args &&...args);

    template <class... Args, executable<Args...> F>
    void emplace_back_discard_future(F &&f, Args &&...args);

    template <class... Args, executable<Args...> F>
    [[nodiscard]] std::future<ext::invocable_result_t<F, Args...>> emplace_front(F &&f, Args &&...args);

    template <class... Args, executable<Args...> F>
    void emplace_front_discard_future(F &&f, Args &&...args);

    void                      shutdown() noexcept;
    void                      forced_shutdown() noexcept;
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

    template <bool DiscardFuture, EmplaceAt position, class... Args, executable<Args...> F>
    [[nodiscard]] std::future<ext::invocable_result_t<F, Args...>> emplace(F &&f, Args &&...args);

    template <ShutdownPolicy policy>
    void do_shutdown() noexcept;

    enum class State {
        CONTINUE,
        STOP,
    };

    std::atomic_long                                 m_activeness;
    std::vector<std::thread>                         m_workers;
    blocking_deque<std::move_only_function<State()>> m_tasks;
};
} // namespace ext
