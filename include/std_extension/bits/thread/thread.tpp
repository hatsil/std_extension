#pragma once

#include "synopsis.hpp"

#include <mutex>
#include <utility>

namespace ext {
template <class F, class... Args>
thread::Spore::Spore(F &&f, Args &&...args)
    : m_interrupted(false)
    , m_cv(nullptr)
    , m_thread(std::forward<F>(f), std::forward<Args>(args)...) {}

template <class F, class... Args>
thread::thread(F &&f, Args &&...args)
    : m_spore(std::make_shared<Spore>(std::forward<F>(f), std::forward<Args>(args)...)) {
    auto           &threads = get_threads();
    std::lock_guard guard(get_mutex());
    threads[get_id()] = m_spore;
}
} // namespace ext
