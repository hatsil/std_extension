#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <limits>
#include <mutex>

namespace ext {
template <std::size_t LeastMaxValue = std::numeric_limits<std::size_t>::max()>
class fair_counting_semaphore {
    static_assert(LeastMaxValue > 0, "LeastMaxValue must be positive");

public:
    static constexpr std::size_t max() noexcept;

    explicit fair_counting_semaphore(std::size_t desired) noexcept;

    fair_counting_semaphore(const fair_counting_semaphore &)            = delete;
    fair_counting_semaphore &operator=(const fair_counting_semaphore &) = delete;

    ~fair_counting_semaphore() = default;

    void release(std::size_t update = 1);
    void acquire();
    bool try_acquire() noexcept;

    template <class Clock, class Duration>
    bool try_acquire_until(const std::chrono::time_point<Clock, Duration> &abs_time);

    template <class Rep, class Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time);

private:
    class UnsafeBinarySemaphore {
    public:
        UnsafeBinarySemaphore();

        UnsafeBinarySemaphore(const UnsafeBinarySemaphore &)            = delete;
        UnsafeBinarySemaphore &operator=(const UnsafeBinarySemaphore &) = delete;

        ~UnsafeBinarySemaphore() = default;

        void release() noexcept;
        void acquire();

        template <class Clock, class Duration>
        bool try_acquire_until(const std::chrono::time_point<Clock, Duration> &abs_time);

        template <class Rep, class Period>
        bool try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time);

    private:
        bool                    m_isReleased;
        std::mutex              m_mutex;
        std::condition_variable m_cv;
    };

    struct ThreadBlocker {
        ThreadBlocker();
        ~ThreadBlocker() = default;
        bool                  m_released;
        ThreadBlocker        *m_next;
        ThreadBlocker        *m_prev;
        UnsafeBinarySemaphore m_sem;
    };

    void           enqueue(ThreadBlocker *threadBlocker) noexcept;
    ThreadBlocker *dequeue() noexcept;
    void           remove(ThreadBlocker *threadBlocker) noexcept;

    std::size_t    m_value;
    std::size_t    m_waiting;
    std::size_t    m_notified;
    ThreadBlocker *m_head;
    ThreadBlocker *m_tail;
    std::mutex     m_mutex;
};

using fair_binary_semaphore = fair_counting_semaphore<1>;
} // namespace ext
