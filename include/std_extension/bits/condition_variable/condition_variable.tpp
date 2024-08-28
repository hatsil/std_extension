#pragma once

#include "std_extension/deferred_task.hpp"
#include "std_extension/interrupted_exception.hpp"
#include "std_extension/thread.hpp"
#include "synopsis.hpp"

#include <utility>

namespace ext {
template <class Pred> void condition_variable::wait(std::unique_lock<std::mutex> &lock, Pred pred) {
    std::shared_ptr<thread::Spore> spore = thread::get_spore();
    if (nullptr != spore) {
        deferred_task defer([&spore] {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = nullptr;
        });

        {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = this;
        }

        while (!pred()) {
            if (spore->m_interrupted.exchange(false)) {
                throw interrupted_exception();
            }
            m_cv.wait(lock);
        }
    } else {
        m_cv.wait(lock, std::move(pred));
    }
}

template <class Clock, class Duration>
std::cv_status
condition_variable::wait_until(std::unique_lock<std::mutex>                   &lock,
                               const std::chrono::time_point<Clock, Duration> &abs_time) {
    std::shared_ptr<thread::Spore> spore = thread::get_spore();
    if (nullptr != spore) {
        deferred_task defer([&spore] {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = nullptr;
        });

        {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = this;
        }

        if (spore->m_interrupted.exchange(false)) {
            throw interrupted_exception();
        }

        auto res = m_cv.wait_until(lock, abs_time);
        if (spore->m_interrupted.exchange(false)) {
            throw interrupted_exception();
        }
        return res;
    } else {
        return m_cv.wait_until(lock, abs_time);
    }
}

template <class Clock, class Duration, class Pred>
bool condition_variable::wait_until(std::unique_lock<std::mutex>                   &lock,
                                    const std::chrono::time_point<Clock, Duration> &abs_time,
                                    Pred                                            pred) {
    std::shared_ptr<thread::Spore> spore = thread::get_spore();
    if (nullptr != spore) {
        deferred_task defer([&spore] {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = nullptr;
        });

        {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = this;
        }

        std::cv_status status = std::cv_status::no_timeout;

        while (!pred() && std::cv_status::no_timeout == status) {
            if (spore->m_interrupted.exchange(false)) {
                throw interrupted_exception();
            }
            status = m_cv.wait_until(lock, abs_time);
        }
        return pred();
    } else {
        return m_cv.wait_until(lock, abs_time, std::move(pred));
    }
}

template <class Rep, class Period>
std::cv_status condition_variable::wait_for(std::unique_lock<std::mutex>             &lock,
                                            const std::chrono::duration<Rep, Period> &rel_time) {
    return wait_until(lock, std::chrono::steady_clock::now() + rel_time);
}

template <class Rep, class Period, class Pred>
bool condition_variable::wait_for(std::unique_lock<std::mutex>             &lock,
                                  const std::chrono::duration<Rep, Period> &rel_time, Pred pred) {
    return wait_until(lock, std::chrono::steady_clock::now() + rel_time, std::move(pred));
}
} // namespace ext
