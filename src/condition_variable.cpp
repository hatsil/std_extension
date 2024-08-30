#include "std_extension/condition_variable.hpp"

#include <memory>
namespace ext {
condition_variable::Defer::Defer(std::unique_lock<std::mutex> *lock, thread::Spore *spore) noexcept
    : m_lock(lock)
    , m_spore(spore) {}

void condition_variable::Defer::operator()() noexcept {
    std::lock_guard guard(m_spore->m_mutex);
    m_spore->m_cv       = nullptr;
    m_spore->m_cv_mutex = nullptr;
}

deferred_task<condition_variable::Defer>
condition_variable::registerCV(std::unique_lock<std::mutex> *lock, thread::Spore *spore) noexcept {
    std::lock_guard guard(spore->m_mutex);
    spore->m_cv       = std::addressof(m_cv);
    spore->m_cv_mutex = lock->mutex();
    return deferred_task(Defer(lock, spore));
}

void condition_variable::checkInterrupted(thread::Spore &spore) const {
    if (spore.m_interrupted.exchange(false)) {
        throw interrupted_exception();
    }
}

void condition_variable::notify_one() noexcept { m_cv.notify_one(); }

void condition_variable::notify_all() noexcept { m_cv.notify_all(); }

void condition_variable::wait(std::unique_lock<std::mutex> &lock) {
    std::shared_ptr<thread::Spore> spore = thread::get_spore();
    if (nullptr != spore) {
        auto defer = registerCV(std::addressof(lock), spore.get());
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
