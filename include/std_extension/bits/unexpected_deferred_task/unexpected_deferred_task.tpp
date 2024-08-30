#pragma once

#include "synopsis.hpp"

#include <exception>
#include <functional>
#include <utility>

namespace ext {
template <std::invocable Func>
unexpected_deferred_task<Func>::unexpected_deferred_task(Func &&func) noexcept(
    std::is_nothrow_move_constructible_v<Func>)
    : m_uncaughtExceptions(std::uncaught_exceptions())
    , m_func(std::move(func)) {}

template <std::invocable Func> unexpected_deferred_task<Func>::~unexpected_deferred_task() {
    if (std::uncaught_exceptions() > m_uncaughtExceptions) {
        std::invoke(std::move(m_func));
    }
}
} // namespace ext
