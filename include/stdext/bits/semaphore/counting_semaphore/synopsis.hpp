#pragma once

#include <cstdint>
#include <limits>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace stdext {
template<std::size_t LeastMaxValue = std::numeric_limits<std::size_t>::max()>
class counting_semaphore {
    static_assert(LeastMaxValue > 0, "LeastMaxValue must be positive");
public:
    static constexpr std::size_t max() noexcept;

    explicit counting_semaphore(std::size_t desired) noexcept;

    ~counting_semaphore() = default;

    counting_semaphore(const counting_semaphore &) = delete;
    counting_semaphore& operator=(const counting_semaphore &) = delete;

    void release(std::size_t update = 1) noexcept;
    void acquire();
    bool try_acquire() noexcept;

    template<class Rep, class Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time);

    template<class Clock, class Duration>
    bool try_acquire_until(const std::chrono::time_point<Clock, Duration>& abs_time);

private:
    std::size_t m_value;
    std::size_t m_waiting;
    std::size_t m_notified;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

using binary_semaphore = counting_semaphore<1>;
}
