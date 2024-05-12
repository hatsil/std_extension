#pragma once

#include "std_extention/exception.hpp"
#include "synopsis.hpp"

#include <algorithm>

namespace ext {
template <std::size_t LeastMaxValue>
constexpr std::size_t counting_semaphore<LeastMaxValue>::max() noexcept {
    return std::numeric_limits<std::size_t>::max();
}

template <std::size_t LeastMaxValue>
counting_semaphore<LeastMaxValue>::counting_semaphore(std::size_t desired) noexcept
    : m_value(std::min(LeastMaxValue, desired))
    , m_waiting(0)
    , m_notified(0) {}

template <std::size_t LeastMaxValue>
void counting_semaphore<LeastMaxValue>::release(std::size_t update) {
    if (0 == update) {
        return;
    }

    std::size_t minUpdate = 0;
    {
        std::lock_guard guard(m_mutex);
        minUpdate = std::min(m_waiting, update);
        if (max() - minUpdate < m_notified) {
            throw exception("m_notified overflow");
        }
        m_waiting -= minUpdate;
        m_notified += minUpdate;
        m_value += std::min(LeastMaxValue - m_value, update - minUpdate);
    }
    if (0 != minUpdate) {
        m_cv.notify_one();
    }
}

template <std::size_t LeastMaxValue> void counting_semaphore<LeastMaxValue>::acquire() {
    std::size_t remainingNotifications = 0;
    {
        std::unique_lock lock(m_mutex);
        if (0 != m_value) {
            --m_value;
            return;
        }

        if (max() == m_waiting) {
            throw exception("m_waiting overflow");
        }

        ++m_waiting;
        m_cv.wait(lock, [this] { return 0 != m_notified; });
        remainingNotifications = --m_notified;
    }

    if (0 != remainingNotifications) {
        m_cv.notify_one();
    }
}

template <std::size_t LeastMaxValue>
bool counting_semaphore<LeastMaxValue>::try_acquire() noexcept {
    std::lock_guard guard(m_mutex);
    if (0 != m_value) {
        --m_value;
        return true;
    }
    return false;
}

template <std::size_t LeastMaxValue>
template <class Clock, class Duration>
bool counting_semaphore<LeastMaxValue>::try_acquire_until(
    const std::chrono::time_point<Clock, Duration> &abs_time) {
    std::size_t remainingNotifications = 0;
    bool        res                    = true;
    {
        std::unique_lock lock(m_mutex);
        if (0 != m_value) {
            --m_value;
            return res;
        }

        if (max() == m_waiting) {
            throw exception("m_waiting overflow");
        }

        ++m_waiting;
        res = m_cv.wait_until(lock, abs_time, [this] { return 0 != m_notified; });
        if (res) {
            remainingNotifications = --m_notified;
        } else {
            --m_waiting;
        }
    }

    if (0 != remainingNotifications) {
        m_cv.notify_one();
    }

    return res;
}

template <std::size_t LeastMaxValue>
template <class Rep, class Period>
bool counting_semaphore<LeastMaxValue>::try_acquire_for(
    const std::chrono::duration<Rep, Period> &rel_time) {
    return try_acquire_until(std::chrono::steady_clock::now() + rel_time);
}
} // namespace ext
