#pragma once

#include "std_extension/memory.hpp"
#include "synopsis.hpp"

#include <thread>
#include <utility>

namespace ext {
template <class E, class Allocator, class CountingSemaphore>
blocking_deque<E, Allocator, CountingSemaphore>::blocking_deque(std::size_t max_capacity)
    : blocking_deque(Allocator(), max_capacity) {}

template <class E, class Allocator, class CountingSemaphore>
blocking_deque<E, Allocator, CountingSemaphore>::blocking_deque(const Allocator &alloc,
                                                                std::size_t      max_capacity)
    : m_alloc(alloc)
    , m_maxCapacity(max_capacity)
    , m_semPush(max_capacity)
    , m_semPop(0)
    , m_deque(AllocatorSharedPtr(alloc)) {}

template <class E, class Allocator, class CountingSemaphore>
std::size_t blocking_deque<E, Allocator, CountingSemaphore>::size() const noexcept {
    std::lock_guard guard(m_mutex);
    return m_deque.size();
}

template <class E, class Allocator, class CountingSemaphore>
std::size_t blocking_deque<E, Allocator, CountingSemaphore>::capacity() const noexcept {
    return m_maxCapacity;
}

template <class E, class Allocator, class CountingSemaphore>
bool blocking_deque<E, Allocator, CountingSemaphore>::empty() const noexcept {
    std::lock_guard guard(m_mutex);
    return m_deque.empty();
}

template <class E, class Allocator, class CountingSemaphore>
void blocking_deque<E, Allocator, CountingSemaphore>::release(CountingSemaphore &sem) noexcept {
    for (;;) try {
            sem.release();
            return;
        } catch (...) {
            std::this_thread::yield();
        }
}

template <class E, class Allocator, class CountingSemaphore>
template <class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E>
blocking_deque<E, Allocator, CountingSemaphore>::newElement(Args &&...args) const {
    return ::ext::make_shared<U>(m_alloc, std::forward<Args>(args)...);
}

template <class E, class Allocator, class CountingSemaphore>
void blocking_deque<E, Allocator, CountingSemaphore>::push(Position           pos,
                                                           std::shared_ptr<E> element) {
    m_semPush.acquire();
    std::lock_guard guard(m_mutex);
    try {
        if (Position::BACK == pos) {
            m_deque.push_back(std::move(element));
        } else {
            m_deque.push_front(std::move(element));
        }
        release(m_semPop);
    } catch (...) {
        release(m_semPush);
        throw;
    }
}

template <class E, class Allocator, class CountingSemaphore>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push(Position           pos,
                                                               std::shared_ptr<E> element) {
    if (m_semPush.try_acquire()) {
        std::lock_guard guard(m_mutex);
        try {
            if (Position::BACK == pos) {
                m_deque.push_back(std::move(element));
            } else {
                m_deque.push_front(std::move(element));
            }
            release(m_semPop);
            return true;
        } catch (...) {
            release(m_semPush);
            throw;
        }
    }
    return false;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_until(
    Position pos, const std::chrono::time_point<Clock, Duration> &abs_time,
    std::shared_ptr<E> element) {
    if (m_semPush.try_acquire_until(abs_time)) {
        std::lock_guard guard(m_mutex);
        try {
            if (Position::BACK == pos) {
                m_deque.push_back(std::move(element));
            } else {
                m_deque.push_front(std::move(element));
            }
            release(m_semPop);
            return true;
        } catch (...) {
            release(m_semPush);
            throw;
        }
    }
    return false;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_for(
    Position pos, const std::chrono::duration<Rep, Period> &rel_time, std::shared_ptr<E> element) {
    return try_push_until(pos, std::chrono::steady_clock::now() + rel_time, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::pop(Position pos) {
    m_semPop.acquire();
    std::lock_guard    guard(m_mutex);
    std::shared_ptr<E> res = Position::BACK == pos ? m_deque.back() : m_deque.front();
    try {
        if (Position::BACK == pos) m_deque.pop_back();
        else m_deque.pop_front();
        release(m_semPush);
    } catch (...) {
        release(m_semPop);
        throw;
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop(Position pos) {
    std::shared_ptr<E> res = nullptr;
    if (m_semPop.try_acquire()) {
        std::lock_guard guard(m_mutex);
        res = Position::BACK == pos ? m_deque.back() : m_deque.front();
        try {
            if (Position::BACK == pos) m_deque.pop_back();
            else m_deque.pop_front();
            release(m_semPush);
        } catch (...) {
            release(m_semPop);
            throw;
        }
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_until(
    Position pos, const std::chrono::time_point<Clock, Duration> &abs_time) {
    std::shared_ptr<E> res = nullptr;
    if (m_semPop.try_acquire_until(abs_time)) {
        std::lock_guard guard(m_mutex);
        res = Position::BACK == pos ? m_deque.back() : m_deque.front();
        try {
            if (Position::BACK == pos) m_deque.pop_back();
            else m_deque.pop_front();
            release(m_semPush);
        } catch (...) {
            release(m_semPop);
            throw;
        }
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_for(
    Position pos, const std::chrono::duration<Rep, Period> &rel_time) {
    return try_pop_until(pos, std::chrono::steady_clock::now() + rel_time);
}

template <class E, class Allocator, class CountingSemaphore>
void blocking_deque<E, Allocator, CountingSemaphore>::push_back(std::shared_ptr<E> element) {
    push(Position::BACK, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_back(std::shared_ptr<E> element) {
    return try_push(Position::BACK, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_back_until(
    const std::chrono::time_point<Clock, Duration> &abs_time, std::shared_ptr<E> element) {
    return try_push_until(Position::BACK, abs_time, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_back_for(
    const std::chrono::duration<Rep, Period> &rel_time, std::shared_ptr<E> element) {
    return try_push_for(Position::BACK, rel_time, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
void blocking_deque<E, Allocator, CountingSemaphore>::push_front(std::shared_ptr<E> element) {
    push(Position::FRONT, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_front(std::shared_ptr<E> element) {
    return try_push(Position::FRONT, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_front_until(
    const std::chrono::time_point<Clock, Duration> &abs_time, std::shared_ptr<E> element) {
    return try_push_until(Position::FRONT, abs_time, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
bool blocking_deque<E, Allocator, CountingSemaphore>::try_push_front_for(
    const std::chrono::duration<Rep, Period> &rel_time, std::shared_ptr<E> element) {
    return try_push_for(Position::FRONT, rel_time, std::move(element));
}

template <class E, class Allocator, class CountingSemaphore>
template <class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::push_back(U &&element) {
    std::shared_ptr<E> res = newElement<U>(std::forward<U>(element));
    push_back(res);
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_push_back(U &&element) {
    std::shared_ptr<E> res = newElement<U>(std::forward<U>(element));
    if (!try_push_back(res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration, class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_push_back_until(
    const std::chrono::time_point<Clock, Duration> &abs_time, U &&element) {
    std::shared_ptr<E> res = newElement<U>(std::forward<U>(element));
    if (!try_push_back_until(abs_time, res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period, class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_push_back_for(
    const std::chrono::duration<Rep, Period> &rel_time, U &&element) {
    return try_push_back_until(std::chrono::steady_clock::now() + rel_time,
                               std::forward<U>(element));
}

template <class E, class Allocator, class CountingSemaphore>
template <class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::push_front(U &&element) {
    std::shared_ptr<E> res = newElement<U>(std::forward<U>(element));
    push_front(res);
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_push_front(U &&element) {
    std::shared_ptr<E> res = newElement<U>(std::forward<U>(element));
    if (!try_push_front(res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration, class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_push_front_until(
    const std::chrono::time_point<Clock, Duration> &abs_time, U &&element) {
    std::shared_ptr<E> res = newElement<U>(std::forward<U>(element));
    if (!try_push_front_until(abs_time, res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period, class U>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_push_front_for(
    const std::chrono::duration<Rep, Period> &rel_time, U &&element) {
    return try_push_front_until(std::chrono::steady_clock::now() + rel_time,
                                std::forward<U>(element));
}

template <class E, class Allocator, class CountingSemaphore>
template <class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::emplace_back(Args &&...args) {
    std::shared_ptr<E> res = newElement<U>(std::forward<Args>(args)...);
    push_back(res);
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E>
blocking_deque<E, Allocator, CountingSemaphore>::try_emplace_back(Args &&...args) {
    std::shared_ptr<E> res = newElement<U>(std::forward<Args>(args)...);
    if (!try_push_back(res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration, class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_emplace_back_until(
    const std::chrono::time_point<Clock, Duration> &abs_time, Args &&...args) {
    std::shared_ptr<E> res = newElement<U>(std::forward<Args>(args)...);
    if (!try_push_back_until(abs_time, res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period, class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_emplace_back_for(
    const std::chrono::duration<Rep, Period> &rel_time, Args &&...args) {
    return try_emplace_back_until<U>(std::chrono::steady_clock::now() + rel_time,
                                     std::forward<Args>(args)...);
}

template <class E, class Allocator, class CountingSemaphore>
template <class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::emplace_front(Args &&...args) {
    std::shared_ptr<E> res = newElement<U>(std::forward<Args>(args)...);
    push_front(res);
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E>
blocking_deque<E, Allocator, CountingSemaphore>::try_emplace_front(Args &&...args) {
    std::shared_ptr<E> res = newElement<U>(std::forward<Args>(args)...);
    if (!try_push_front(res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration, class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_emplace_front_until(
    const std::chrono::time_point<Clock, Duration> &abs_time, Args &&...args) {
    std::shared_ptr<E> res = newElement<U>(std::forward<Args>(args)...);
    if (!try_push_front_until(abs_time, res)) {
        return std::shared_ptr<E>(nullptr);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period, class U, class... Args>
    requires std::constructible_from<U, Args...>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_emplace_front_for(
    const std::chrono::duration<Rep, Period> &rel_time, Args &&...args) {
    return try_emplace_front_until<U>(std::chrono::steady_clock::now() + rel_time,
                                      std::forward<Args>(args)...);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::peek(Position pos) const {
    m_semPop.acquire();
    std::lock_guard    guard(m_mutex);
    std::shared_ptr<E> res = Position::BACK == pos ? m_deque.back() : m_deque.front();
    release(m_semPop);
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E>
blocking_deque<E, Allocator, CountingSemaphore>::try_peek(Position pos) const noexcept {
    std::shared_ptr<E> res = nullptr;
    if (m_semPop.try_acquire()) {
        std::lock_guard guard(m_mutex);
        res = Position::BACK == pos ? m_deque.back() : m_deque.front();
        release(m_semPop);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_peek_until(
    Position pos, const std::chrono::time_point<Clock, Duration> &abs_time) const {
    std::shared_ptr<E> res = nullptr;
    if (m_semPop.try_acquire_until(abs_time)) {
        std::lock_guard guard(m_mutex);
        res = Position::BACK == pos ? m_deque.back() : m_deque.front();
        release(m_semPop);
    }
    return res;
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_peek_for(
    Position pos, const std::chrono::duration<Rep, Period> &rel_time) const {
    return try_peek_until(pos, std::chrono::steady_clock::now() + rel_time);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::back() const {
    return peek(Position::BACK);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_back() const noexcept {
    return try_peek(Position::BACK);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_back_for(
    const std::chrono::duration<Rep, Period> &rel_time) const {
    return try_peek_for(Position::BACK, rel_time);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_back_until(
    const std::chrono::time_point<Clock, Duration> &abs_time) const {
    return try_peek_for(Position::BACK, abs_time);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::front() const {
    return peek(Position::FRONT);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_front() const noexcept {
    return try_peek(Position::FRONT);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_front_for(
    const std::chrono::duration<Rep, Period> &rel_time) const {
    return try_peek_for(Position::FRONT, rel_time);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_front_until(
    const std::chrono::time_point<Clock, Duration> &abs_time) const {
    return try_peek_for(Position::FRONT, abs_time);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::pop_back() {
    return pop(Position::BACK);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_back() {
    return try_pop(Position::BACK);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_back_for(
    const std::chrono::duration<Rep, Period> &rel_time) {
    return try_pop_for(Position::BACK, rel_time);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_back_until(
    const std::chrono::time_point<Clock, Duration> &abs_time) {
    return try_pop_until(Position::BACK, abs_time);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::pop_front() {
    return pop(Position::FRONT);
}

template <class E, class Allocator, class CountingSemaphore>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_front() {
    return try_pop(Position::FRONT);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Rep, class Period>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_front_for(
    const std::chrono::duration<Rep, Period> &rel_time) {
    return try_pop_for(Position::FRONT, rel_time);
}

template <class E, class Allocator, class CountingSemaphore>
template <class Clock, class Duration>
std::shared_ptr<E> blocking_deque<E, Allocator, CountingSemaphore>::try_pop_front_until(
    const std::chrono::time_point<Clock, Duration> &abs_time) {
    return try_pop_until(Position::FRONT, abs_time);
}
} // namespace ext
