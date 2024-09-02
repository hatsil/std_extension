#include "std_extension/thread.hpp"
#include "std_extension/condition_variable.hpp"

#include <system_error>

namespace ext {
namespace this_thread {
void yield() noexcept { std::this_thread::yield(); }

std::thread::id get_id() noexcept { return std::this_thread::get_id(); }
} // namespace this_thread

std::unordered_map<std::thread::id, std::shared_ptr<thread::Spore>> &
thread::get_threads() noexcept {
    static std::unordered_map<std::thread::id, std::shared_ptr<Spore>> threads;
    return threads;
}

std::shared_mutex &thread::get_mutex() noexcept {
    static std::shared_mutex mutex;
    return mutex;
}

std::shared_ptr<thread::Spore> thread::get_spore() {
    auto            &threads = get_threads();
    std::shared_lock lock(get_mutex());
    auto             it = threads.find(std::this_thread::get_id());
    return threads.end() == it ? nullptr : it->second;
}

thread::thread() noexcept
    : m_spore(nullptr) {}

void thread::swap(thread &other) noexcept { m_spore.swap(other.m_spore); }

bool thread::joinable() const noexcept {
    std::shared_ptr<Spore> spore = m_spore;
    return spore ? spore->m_thread.joinable() : false;
}

void thread::join() {
    std::shared_ptr<Spore> spore = m_spore;
    if (nullptr == spore) {
        throw std::system_error(std::make_error_code(std::errc::invalid_argument));
    }
    std::thread::id tid = spore->m_thread.get_id();
    spore->m_thread.join();
    auto           &threads = get_threads();
    std::lock_guard guard(get_mutex());
    threads.erase(tid);
}

void thread::detach() {
    std::shared_ptr<Spore> spore = m_spore;
    if (nullptr == spore) {
        throw std::system_error(std::make_error_code(std::errc::invalid_argument));
    }
    auto           &threads = get_threads();
    std::lock_guard guard(spore->m_mutex);
    threads.erase(spore->m_thread.get_id());
    spore->m_thread.detach();
}

std::thread::id thread::get_id() const noexcept {
    std::shared_ptr<Spore> spore = m_spore;
    if (nullptr == spore) {
        return std::thread::id();
    }
    return spore->m_thread.get_id();
}

std::thread::native_handle_type thread::native_handle() {
    std::shared_ptr<Spore> spore = m_spore;
    if (nullptr == spore) {
        throw std::system_error(std::make_error_code(std::errc::invalid_argument));
    }
    return spore->m_thread.native_handle();
}

void thread::interrupt() {
    std::shared_ptr<Spore> spore = m_spore;

    if (nullptr == spore || !spore->m_thread.joinable()) {
        throw std::system_error(std::make_error_code(std::errc::invalid_argument));
    }

    std::lock_guard guard(spore->m_mutex);
    spore->m_interrupted = true;
    if (nullptr != spore->m_cv) {
        // lock and then immediately unlock the cv's mutex, to make sure that the thread went to
        // sleep and hence released the mutex.
        spore->m_cv_mutex->lock();
        spore->m_cv_mutex->unlock();
        spore->m_cv->notify_all();
    }
}
} // namespace ext
