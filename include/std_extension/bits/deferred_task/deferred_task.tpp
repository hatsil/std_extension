#pragma once

#include "synopsis.hpp"

#include <functional>
#include <utility>

namespace ext {
template <std::invocable Func>
deferred_task<Func>::deferred_task(Func &&func) noexcept(std::is_nothrow_move_constructible_v<Func>)
    : m_func(std::move(func))
    , m_valid(true) {}

template <std::invocable Func>
deferred_task<Func>::deferred_task(deferred_task &&moved) noexcept(
    std::is_nothrow_move_constructible_v<Func>)
    : m_func(std::move(moved.m_func))
    , m_valid(true) {
    moved.m_valid = false;
}

template <std::invocable Func>
deferred_task<Func> &deferred_task<Func>::operator=(deferred_task &&moved) noexcept(
    std::is_nothrow_move_constructible_v<Func> && std::is_nothrow_move_assignable_v<Func>) {
    m_func        = std::move(moved.m_func);
    m_valid       = moved.m_valid;
    moved.m_valid = false;
    return *this;
}

template <std::invocable Func> deferred_task<Func>::~deferred_task() {
    if (m_valid) {
        std::invoke(std::move(m_func));
    }
}
} // namespace ext
