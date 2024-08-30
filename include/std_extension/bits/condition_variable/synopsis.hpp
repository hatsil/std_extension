#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace ext {
class condition_variable final {
public:
    condition_variable()  = default;
    ~condition_variable() = default;

    condition_variable(const condition_variable &)            = delete;
    condition_variable &operator=(const condition_variable &) = delete;

    void notify_one() noexcept;
    void notify_all() noexcept;
    void wait(std::unique_lock<std::mutex> &lock);

    template <class Pred> void wait(std::unique_lock<std::mutex> &lock, Pred pred);

    template <class Clock, class Duration>
    std::cv_status wait_until(std::unique_lock<std::mutex>                   &lock,
                              const std::chrono::time_point<Clock, Duration> &abs_time);

    template <class Clock, class Duration, class Pred>
    bool wait_until(std::unique_lock<std::mutex>                   &lock,
                    const std::chrono::time_point<Clock, Duration> &abs_time, Pred pred);

    template <class Rep, class Period>
    std::cv_status wait_for(std::unique_lock<std::mutex>             &lock,
                            const std::chrono::duration<Rep, Period> &rel_time);

    template <class Rep, class Period, class Pred>
    bool wait_for(std::unique_lock<std::mutex>             &lock,
                  const std::chrono::duration<Rep, Period> &rel_time, Pred pred);

    std::condition_variable::native_handle_type native_handle();

private:
    struct Defer final {
        explicit Defer(std::unique_lock<std::mutex> *lock, thread::Spore *spore) noexcept;

        Defer(const Defer &)            = default;
        Defer &operator=(const Defer &) = default;

        ~Defer() = default;

        void operator()() noexcept;

    private:
        std::unique_lock<std::mutex> *m_lock;
        thread::Spore                *m_spore;
    };

    void                               checkInterrupted(thread::Spore &spore) const;
    [[nodiscard]] deferred_task<Defer> registerCV(std::unique_lock<std::mutex> *lock,
                                                  thread::Spore                *spore) noexcept;

    std::condition_variable m_cv;
};
} // namespace ext
