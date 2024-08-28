#include "std_extention/thread.hpp"
#include "std_extention/condition_variable.hpp"

#include <system_error>

namespace ext {
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

thread::Spore::Spore() noexcept
    : m_interrupted(false)
    , m_cv(nullptr) {}

thread::thread()
    : m_spore(std::make_shared<Spore>()) {}

thread::~thread() {
    auto           &threads = get_threads();
    std::lock_guard guard(get_mutex());
    threads.erase(get_id());
}

void thread::swap(thread &other) noexcept { m_spore.swap(other.m_spore); }

bool thread::joinable() const noexcept { return m_spore->m_thread.joinable(); }

void thread::join() {
    std::shared_ptr<Spore> spore = m_spore;
    std::thread::id        tid   = spore->m_thread.get_id();
    spore->m_thread.join();
    auto           &threads = get_threads();
    std::lock_guard guard(get_mutex());
    threads.erase(tid);
}

void thread::detach() {
    auto                  &threads = get_threads();
    std::shared_ptr<Spore> spore   = m_spore;
    std::lock_guard        guard(spore->m_mutex);
    threads.erase(spore->m_thread.get_id());
    spore->m_thread.detach();
}

std::thread::id thread::get_id() const noexcept { return m_spore->m_thread.get_id(); }

std::thread::native_handle_type thread::native_handle() {
    return m_spore->m_thread.native_handle();
}

void thread::interrupt() {
    std::shared_ptr<Spore> spore = m_spore;
    spore->m_interrupted         = true;
    std::shared_lock lock(spore->m_mutex);
    if (!spore->m_thread.joinable()) {
        throw std::system_error(std::make_error_code(std::errc::invalid_argument));
    }
    if (nullptr != spore->m_cv) {
        spore->m_cv->notify_all();
    }
}
} // namespace ext
