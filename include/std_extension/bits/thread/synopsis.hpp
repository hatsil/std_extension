#pragma once

#include "std_extension/exception.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

namespace ext {
namespace this_thread {
void            yield() noexcept;
std::thread::id get_id() noexcept;

template <class Clock, class Duration>
void sleep_until(const std::chrono::time_point<Clock, Duration> &sleep_time);

template <class Rep, class Period>
void sleep_for(const std::chrono::duration<Rep, Period> &sleep_duration);
} // namespace this_thread
class thread final {
public:
    thread() noexcept;

    template <class F, class... Args> explicit thread(F &&f, Args &&...args);

    thread(thread &&moved) noexcept            = default;
    thread &operator=(thread &&moved) noexcept = default;

    thread(const thread &)            = delete;
    thread &operator=(const thread &) = delete;

    ~thread() = default;

    void                            swap(thread &other) noexcept;
    bool                            joinable() const noexcept;
    void                            join();
    void                            detach();
    std::thread::id                 get_id() const noexcept;
    std::thread::native_handle_type native_handle();
    void                            interrupt();

private:
    friend class condition_variable;

    template <class Clock, class Duration>
    friend void
    this_thread::sleep_until(const std::chrono::time_point<Clock, Duration> &sleep_time);

    struct Spore {
        template <class F, class... Args> explicit Spore(F &&f, Args &&...args);

        Spore(const Spore &)            = delete;
        Spore &operator=(const Spore &) = delete;

        std::atomic_bool         m_interrupted;
        std::condition_variable *m_cv;
        std::mutex              *m_cv_mutex;
        std::thread              m_thread;
        std::mutex               m_mutex;
    };

    static std::unordered_map<std::thread::id, std::shared_ptr<Spore>> &get_threads() noexcept;
    static std::shared_mutex                                           &get_mutex() noexcept;
    static std::shared_ptr<Spore>                                       get_spore();

    std::shared_ptr<Spore> m_spore;
};
} // namespace ext
