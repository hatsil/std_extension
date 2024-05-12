#pragma once

#include "synopsis.hpp"

#include <exception>

namespace ext {
template <typename Func>
unexpected_deferred_task<Func>::unexpected_deferred_task(Func &&func) noexcept(
    std::is_nothrow_move_constructible_v<std::decay_t<Func>>)
    : m_uncaughtExceptions(std::uncaught_exceptions())
    , m_func(std::move(func)) {}

template <typename Func>
void unexpected_deferred_task<Func>::operator()() noexcept((std::declval<std::decay_t<Func>>())()) {
    m_func();
}

template <typename Func> unexpected_deferred_task<Func>::~unexpected_deferred_task() {
    if (std::uncaught_exceptions() > m_uncaughtExceptions) {
        m_func();
    }
}
} // namespace ext
