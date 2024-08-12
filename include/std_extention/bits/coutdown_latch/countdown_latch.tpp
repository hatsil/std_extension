#pragma once

#include "synopsis.hpp"

namespace ext {
template <class Clock, class Duration>
bool countdown_latch::wait_until(const std::chrono::time_point<Clock, Duration> &abs_time) const {
    std::unique_lock lock(m_mutex);
    return m_cv.wait_until(lock, abs_time, [this] { return 0 == m_count; });
}

template <class Rep, class Period>
bool countdown_latch::wait_for(const std::chrono::duration<Rep, Period> &rel_time) const {
    return wait_until(std::chrono::steady_clock::now() + rel_time);
}
} // namespace ext
