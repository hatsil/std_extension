#include "std_extension/condition_variable.hpp"

namespace ext {
void condition_variable::notify_one() noexcept { m_cv.notify_one(); }

void condition_variable::notify_all() noexcept { m_cv.notify_all(); }

void condition_variable::wait(std::unique_lock<std::mutex> &lock) {
    std::shared_ptr<thread::Spore> spore = thread::get_spore();
    if (nullptr != spore) {
        deferred_task defer([&spore] {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = nullptr;
        });

        {
            std::lock_guard guard(spore->m_mutex);
            spore->m_cv = this;
        }

        if (spore->m_interrupted.exchange(false)) {
            throw interrupted_exception();
        }

        m_cv.wait(lock);

        if (spore->m_interrupted.exchange(false)) {
            throw interrupted_exception();
        }
    } else {
        m_cv.wait(lock);
    }
}

std::condition_variable::native_handle_type condition_variable::native_handle() {
    return m_cv.native_handle();
}
} // namespace ext
