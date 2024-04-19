#pragma once

#include "fair_counting_semaphore.hpp"

#include <memory>
#include <algorithm>

namespace stdext {
template <std::size_t LeastMaxValue>
fair_counting_semaphore<LeastMaxValue>::UnsafeBinarySemaphore::UnsafeBinarySemaphore() : m_isReleased(false) {}

template <std::size_t LeastMaxValue>
void fair_counting_semaphore<LeastMaxValue>::UnsafeBinarySemaphore::release() noexcept {
    {
        std::lock_guard guard(m_mutex);
        m_isReleased = true;
    }
    m_cv.notify_one();
}

template <std::size_t LeastMaxValue>
void fair_counting_semaphore<LeastMaxValue>::UnsafeBinarySemaphore::acquire() {
    std::unique_lock lock(m_mutex);
    m_cv.wait(lock, [this] { return m_isReleased; });
}

template <std::size_t LeastMaxValue>
template<class Rep, class Period>
bool fair_counting_semaphore<LeastMaxValue>::UnsafeBinarySemaphore::try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time) {
    std::unique_lock lock(m_mutex);
    return m_cv.wait_for(lock, rel_time, [this] { return m_isReleased; });
}

template <std::size_t LeastMaxValue>
template<class Clock, class Duration>
bool fair_counting_semaphore<LeastMaxValue>::UnsafeBinarySemaphore::try_acquire_until(const std::chrono::time_point<Clock, Duration> &abs_time) {
    std::unique_lock lock(m_mutex);
    return m_cv.wait_until(lock, abs_time, [this] { return m_isReleased; });
}

template <std::size_t LeastMaxValue>
fair_counting_semaphore<LeastMaxValue>::ThreadBlocker::ThreadBlocker()
    : m_released(false)
    , m_next(nullptr)
    , m_prev(nullptr) {}

template <std::size_t LeastMaxValue>
constexpr std::size_t fair_counting_semaphore<LeastMaxValue>::max() noexcept {
    return std::numeric_limits<std::size_t>::max();
}

template <std::size_t LeastMaxValue>
fair_counting_semaphore<LeastMaxValue>::fair_counting_semaphore(std::size_t desired)
    : m_value(std::min(LeastMaxValue, desired))
    , m_waiting(0)
    , m_notified(0)
    , m_head(nullptr)
    , m_tail(nullptr) {}

template <std::size_t LeastMaxValue>
void fair_counting_semaphore<LeastMaxValue>::release(std::size_t update) {
    if (0 == update) {
        return;
    }

    ThreadBlocker *next = nullptr;
    {
        std::lock_guard guard(m_mutex);
        std::size_t minUpdate = std::min(m_waiting, update);
        m_waiting -= minUpdate;
        m_notified += minUpdate;
        m_value += std::min(LeastMaxValue - m_value, update - minUpdate);
        if (0 != minUpdate && minUpdate == m_notified) {
            next = dequeue();
        }
    }

    if (nullptr != next) {
        next->m_sem.release();
    }
}

template <std::size_t LeastMaxValue>
void fair_counting_semaphore<LeastMaxValue>::acquire() {
    ThreadBlocker threadBlocker;
    {
        std::lock_guard guard(m_mutex);
        if (0 != m_value) {
            --m_value;
            return;
        }

        if (max() == m_waiting) {
            //throw something...
        }

        ++m_waiting;
        enqueue(std::addressof(threadBlocker));
    }

    threadBlocker.m_sem.acquire();
    ThreadBlocker *next = nullptr;
    {
        std::lock_guard guard(m_mutex);
        if (0 != --m_notified) {
            next = dequeue();
        }
    }

    if (nullptr != next) {
        next->m_sem.release();
    }
}

template <std::size_t LeastMaxValue>
bool fair_counting_semaphore<LeastMaxValue>::try_acquire() noexcept {
    std::lock_guard guard(m_mutex);
    if (m_value > 0) {
        --m_value;
        return true;
    }
    return false;
}

template <std::size_t LeastMaxValue>
template<class Rep, class Period>
bool fair_counting_semaphore<LeastMaxValue>::try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time) {
    ThreadBlocker threadBlocker;
    {
        std::lock_guard guard(m_mutex);
        if (0 != m_value) {
            --m_value;
            return true;
        }

        if (max() == m_waiting) {
            //throw something...
        }

        ++m_waiting;
        enqueue(std::addressof(threadBlocker));
    }
    threadBlocker.m_sem.try_acquire_for(rel_time);
    ThreadBlocker *next = nullptr;
    {
        std::lock_guard guard(m_mutex);
        if (threadBlocker.m_released){
            if (0 != --m_notified) {
                next = dequeue();
            }
        } else {
            remove(std::addressof(threadBlocker));
            --m_waiting;
        }
    }

    if (nullptr != next) {
        next->m_sem.release();
    }

    return threadBlocker.m_released;
}

template <std::size_t LeastMaxValue>
template<class Clock, class Duration>
bool fair_counting_semaphore<LeastMaxValue>::try_acquire_until(const std::chrono::time_point<Clock, Duration> &abs_time) {
    ThreadBlocker threadBlocker;
    {
        std::lock_guard guard(m_mutex);
        if (0 != m_value) {
            --m_value;
            return true;
        }

        if (max() == m_waiting) {
            //throw something...
        }

        ++m_waiting;
        enqueue(std::addressof(threadBlocker));
    }
    threadBlocker.m_sem.try_acquire_until(abs_time);
    ThreadBlocker *next = nullptr;
    {
        std::lock_guard guard(m_mutex);
        if (threadBlocker.m_released){
            if (0 != --m_notified) {
                next = dequeue();
            }
        } else {
            remove(std::addressof(threadBlocker));
            --m_waiting;
        }
    }

    if (nullptr != next) {
        next->m_sem.release();
    }

    return threadBlocker.m_released;
}


template <std::size_t LeastMaxValue>
void fair_counting_semaphore<LeastMaxValue>::enqueue(ThreadBlocker *threadBlocker) noexcept {
    if (nullptr == m_tail) {
        m_head = m_tail = threadBlocker;
    } else {
        threadBlocker->m_prev = m_tail;
        m_tail->m_next = threadBlocker;
        m_tail = threadBlocker;
    }
}

template <std::size_t LeastMaxValue>
fair_counting_semaphore<LeastMaxValue>::ThreadBlocker *fair_counting_semaphore<LeastMaxValue>::dequeue() noexcept {
    ThreadBlocker *released = m_head;
    released->m_released = true;
    m_head = released->m_next;
    if (nullptr == m_head) {
        m_tail = nullptr;
    } else {
        m_head->m_prev = nullptr;
    }
    return released;
}

template <std::size_t LeastMaxValue>
void fair_counting_semaphore<LeastMaxValue>::remove(ThreadBlocker *threadBlocker) noexcept {
    if (nullptr != threadBlocker->m_prev) {
        threadBlocker->m_prev->m_next = threadBlocker->m_next;
    } else {
        m_head = threadBlocker->m_next;
    }

    if (nullptr != threadBlocker->m_next) {
        threadBlocker->m_next->m_prev = threadBlocker->m_prev;
    } else {
        m_tail = threadBlocker->m_prev;
    }
}
}
