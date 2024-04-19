#pragma once

#include <memory>
#include <mutex>
#include <list>
#include <limits>

namespace stdext {
template<class E, class Allocator, class CountingSemaphore>
class basic_blocking_deque {
public:
    basic_blocking_deque(std::size_t max_capacity = std::numeric_limits<int>::max());

    basic_blocking_deque(const Allocator &alloc, std::size_t max_capacity = std::numeric_limits<int>::max());

    basic_blocking_deque(const basic_blocking_deque &) = delete;
    basic_blocking_deque &operator=(const basic_blocking_deque &) = delete;

    ~basic_blocking_deque() = default;

    [[nodiscard]] static constexpr std::size_t max() noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    void push_back(std::shared_ptr<E> element);
    [[nodiscard]] bool try_push_back(std::shared_ptr<E> element);
    template<class Rep, class Period>
    [[nodiscard]] bool try_push_back_for(const std::chrono::duration<Rep, Period> &rel_time, std::shared_ptr<E> element);
    template<class Clock, class Duration>
    [[nodiscard]] bool try_push_back_until(const std::chrono::time_point<Clock, Duration> &abs_time, std::shared_ptr<E> element);

    void push_front(std::shared_ptr<E> element);
    [[nodiscard]] bool try_push_front(std::shared_ptr<E> element);
    template<class Rep, class Period>
    [[nodiscard]] bool try_push_front_for(const std::chrono::duration<Rep, Period> &rel_time, std::shared_ptr<E> element);
    template<class Clock, class Duration>
    [[nodiscard]] bool try_push_front_until(const std::chrono::time_point<Clock, Duration> &abs_time, std::shared_ptr<E> element);

    std::shared_ptr<E> push_back(const E &element);
    std::shared_ptr<E> push_back(E &&element);
    [[nodiscard]] std::shared_ptr<E> try_push_back(const E &element);
    [[nodiscard]] std::shared_ptr<E> try_push_back(E &&element);
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_push_back_for(const std::chrono::duration<Rep, Period> &rel_time, const E &element);
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_push_back_for(const std::chrono::duration<Rep, Period> &rel_time, E &&element);
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_push_back_until(const std::chrono::time_point<Clock, Duration> &abs_time, const E &element);
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_push_back_until(const std::chrono::time_point<Clock, Duration> &abs_time, E &&element);

    std::shared_ptr<E> push_front(const E &element);
    std::shared_ptr<E> push_front(E &&element);
    [[nodiscard]] std::shared_ptr<E> try_push_front(const E &element);
    [[nodiscard]] std::shared_ptr<E> try_push_front(E &&element);
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_push_front_for(const std::chrono::duration<Rep, Period> &rel_time, const E &element);
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_push_front_for(const std::chrono::duration<Rep, Period> &rel_time, E &&element);
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_push_front_until(const std::chrono::time_point<Clock, Duration> &abs_time, const E &element);
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_push_front_until(const std::chrono::time_point<Clock, Duration> &abs_time, E &&element);

    template<class... Args>
    std::shared_ptr<E> emplace_back(Args&&... args);
    template<class... Args>
    [[nodiscard]] std::shared_ptr<E> try_emplace_back(Args&&... args);
    template<class Rep, class Period, class... Args>
    [[nodiscard]] std::shared_ptr<E> try_emplace_back_for(const std::chrono::duration<Rep, Period> &rel_time, Args&&... args);
    template<class Clock, class Duration, class... Args>
    [[nodiscard]] std::shared_ptr<E> try_emplace_back_until(const std::chrono::time_point<Clock, Duration> &abs_time, Args&&... args);

    template<class... Args>
    std::shared_ptr<E> emplace_front(Args&&... args);
    template<class... Args>
    [[nodiscard]] std::shared_ptr<E> try_emplace_front(Args&&... args);
    template<class Rep, class Period, class... Args>
    [[nodiscard]] std::shared_ptr<E> try_emplace_front_for(const std::chrono::duration<Rep, Period> &rel_time, Args&&... args);
    template<class Clock, class Duration, class... Args>
    [[nodiscard]] std::shared_ptr<E> try_emplace_front_until(const std::chrono::time_point<Clock, Duration> &abs_time, Args&&... args);

    [[nodiscard]] std::shared_ptr<E> back() const;
    [[nodiscard]] std::shared_ptr<E> try_back() const noexcept;
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_back_for(const std::chrono::duration<Rep, Period> &rel_time) const;
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_back_until(const std::chrono::time_point<Clock, Duration> &abs_time) const;

    [[nodiscard]] std::shared_ptr<E> front() const;
    [[nodiscard]] std::shared_ptr<E> try_front() const noexcept;
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_front_for(const std::chrono::duration<Rep, Period> &rel_time) const;
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_front_until(const std::chrono::time_point<Clock, Duration> &abs_time) const;

    [[nodiscard]] std::shared_ptr<E> pop_back();
    [[nodiscard]] std::shared_ptr<E> try_pop_back() noexcept;
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_pop_back_for(const std::chrono::duration<Rep, Period> &rel_time);
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_pop_back_until(const std::chrono::time_point<Clock, Duration> &abs_time);

    [[nodiscard]] std::shared_ptr<E> pop_front();
    [[nodiscard]] std::shared_ptr<E> try_pop_front() noexcept;
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_pop_front_for(const std::chrono::duration<Rep, Period> &rel_time);
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_pop_front_until(const std::chrono::time_point<Clock, Duration> &abs_time);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

private:
    using AllocatorSharedPtr = typename std::allocator_traits<Allocator>::rebind_alloc<std::shared_ptr<E>>;

    template<class... Args>
    [[nodiscard]] std::shared_ptr<E> newElement(Args&&... args) const;

    enum class Position {
        BACK,
        FRONT,
    };

    void push(Position pos, std::shared_ptr<E> element);
    [[nodiscard]] bool try_push(Position pos, std::shared_ptr<E> element) noexcept;
    template<class Rep, class Period>
    [[nodiscard]] bool try_push_for(Position pos, const std::chrono::duration<Rep, Period> &rel_time, std::shared_ptr<E> element);
    template<class Clock, class Duration>
    [[nodiscard]] bool try_push_until(Position pos, const std::chrono::time_point<Clock, Duration> &abs_time, std::shared_ptr<E> element);

    [[nodiscard]] std::shared_ptr<E> pop(Position pos);
    [[nodiscard]] std::shared_ptr<E> try_pop(Position pos) noexcept;
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_pop_for(Position pos, const std::chrono::duration<Rep, Period> &rel_time);
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_pop_until(Position pos, const std::chrono::time_point<Clock, Duration> &abs_time);

    [[nodiscard]] std::shared_ptr<E> peek(Position pos) const;
    [[nodiscard]] std::shared_ptr<E> try_peek(Position pos) const noexcept;
    template<class Rep, class Period>
    [[nodiscard]] std::shared_ptr<E> try_peek_for(Position pos, const std::chrono::duration<Rep, Period> &rel_time) const;
    template<class Clock, class Duration>
    [[nodiscard]] std::shared_ptr<E> try_peek_until(Position pos, const std::chrono::time_point<Clock, Duration> &abs_time) const ;

    // MARK: fields
    Allocator m_alloc;
    mutable CountingSemaphore m_semPush;
    mutable CountingSemaphore m_semPop;
    mutable std::mutex m_mutex;
    std::list<std::shared_ptr<E>, AllocatorSharedPtr> m_deque;
};
}
