#pragma once

#include "std_extension/deferred_task.hpp"
#include "std_extension/interrupted_exception.hpp"
#include "synopsis.hpp"

#include <utility>

namespace ext {
namespace this_thread {
template <class Clock, class Duration>
void sleep_until(const std::chrono::time_point<Clock, Duration> &sleep_time) {
    std::shared_ptr<thread::Spore> spore = thread::get_spore();
    if (nullptr != spore) {
        deferred_task defer([&spore] {
            {
                std::lock_guard guard(spore->m_mutex);
                spore->m_cv_cv = nullptr;
            }
            spore->m_cv.notify_all();
        });

        std::condition_variable cv;
        std::mutex              mutex;
        std::unique_lock        lock(mutex);

        {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv_cv = std::addressof(cv);
        }

        if (spore->m_interrupted.exchange(false)) {
            throw interrupted_exception();
        }
        std::cv_status status = std::cv_status::no_timeout;
        while (std::cv_status::no_timeout == status) {
            status = cv.wait_until(lock, sleep_time);
            if (spore->m_interrupted.exchange(false)) {
                throw interrupted_exception();
            }
        }
    } else {
        std::this_thread::sleep_until(sleep_time);
    }
}

template <class Rep, class Period>
void sleep_for(const std::chrono::duration<Rep, Period> &sleep_duration) {
    sleep_until(std::chrono::steady_clock::now() + sleep_duration);
}
} // namespace this_thread

template <class F, class... Args>
thread::Spore::Spore(F &&f, Args &&...args)
    : m_interrupted(false)
    , m_cv_cv(nullptr)
    , m_thread(std::forward<F>(f), std::forward<Args>(args)...) {}

template <class F, class... Args>
thread::thread(F &&f, Args &&...args)
    : m_spore(std::make_shared<Spore>(std::forward<F>(f), std::forward<Args>(args)...)) {
    auto           &threads = get_threads();
    std::lock_guard guard(get_mutex());
    threads[get_id()] = m_spore;
}
} // namespace ext
