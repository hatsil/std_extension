#include "std_extension/condition_variable.hpp"

#include <memory>
namespace ext {
condition_variable::Defer::Defer(thread::Spore &spore) noexcept
    : m_spore(spore) {}

void condition_variable::Defer::operator()() noexcept {
    std::lock_guard guard(m_spore.m_mutex);
    m_spore.m_cv_cv = nullptr;
}

deferred_task<condition_variable::Defer>
condition_variable::registerCV(thread::Spore &spore) noexcept {
    std::lock_guard guard(spore.m_mutex);
    spore.m_cv_cv = std::addressof(m_cv);
    return deferred_task(Defer(spore));
}

void condition_variable::checkInterrupted(thread::Spore &spore) {
    if (spore.m_interrupted.exchange(false)) {
        throw interrupted_exception();
    }
}

void condition_variable::notify_one() noexcept { m_cv.notify_one(); }

void condition_variable::notify_all() noexcept { m_cv.notify_all(); }

void condition_variable::wait(std::unique_lock<std::mutex> &lock) {
    std::shared_ptr<thread::Spore> spore = thread::get_spore();
    if (nullptr != spore) {
        auto defer = registerCV(*spore);
        checkInterrupted(*spore);
        m_cv.wait(lock);
        checkInterrupted(*spore);
    } else {
        m_cv.wait(lock);
    }
}

std::condition_variable::native_handle_type condition_variable::native_handle() {
    return m_cv.native_handle();
}
} // namespace ext
