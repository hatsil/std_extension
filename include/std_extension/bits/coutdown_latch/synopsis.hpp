#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>

namespace ext {
class countdown_latch final {
public:
    explicit countdown_latch(std::size_t count);
    ~countdown_latch() = default;

    countdown_latch(const countdown_latch &)            = delete;
    countdown_latch &operator=(const countdown_latch &) = delete;

    void wait() const;

    template <class Clock, class Duration>
    bool wait_until(const std::chrono::time_point<Clock, Duration> &abs_time) const;

    template <class Rep, class Period>
    bool wait_for(const std::chrono::duration<Rep, Period> &rel_time) const;

    void        countdown() noexcept;
    std::size_t count() const noexcept;

private:
    std::size_t                     m_count;
    mutable std::mutex              m_mutex;
    mutable std::condition_variable m_cv;
};
} // namespace ext
