#pragma once

#include "synopsis.hpp"

namespace ext {
template <typename Func>
deferred_task<Func>::deferred_task(Func &&func) noexcept(
    std::is_nothrow_move_constructible_v<std::decay_t<Func>>)
    : m_func(std::move(func)) {}

template <typename Func>
void deferred_task<Func>::operator()() noexcept((std::declval<std::decay_t<Func>>())()) {
    m_func();
}

template <typename Func> deferred_task<Func>::~deferred_task() { m_func(); }
} // namespace ext
