#include "std_extension/countdown_latch.hpp"

namespace ext {
countdown_latch::countdown_latch(std::size_t count)
    : m_count(count) {}

void countdown_latch::wait() const {
    std::unique_lock lock(m_mutex);
    m_cv.wait(lock, [this] { return 0 == m_count; });
}

void countdown_latch::countdown() noexcept {
    {
        std::lock_guard guard(m_mutex);
        if (0 == m_count || 0 != --m_count) {
            return;
        }
    }
    m_cv.notify_all();
}

std::size_t countdown_latch::count() const noexcept {
    std::lock_guard guard(m_mutex);
    return m_count;
}
} // namespace ext
