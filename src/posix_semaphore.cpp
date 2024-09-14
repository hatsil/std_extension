#include "std_extension/semaphore.hpp"

#include <system_error>

namespace ext {
posix_semaphore::posix_semaphore(unsigned int desired) {
    if (::sem_init(&m_sem, 0, desired) != 0) {
        throw std::system_error(std::make_error_code(std::errc(errno)));
    }
}

void posix_semaphore::release() {
    if (::sem_post(&m_sem) != 0) {
        throw std::system_error(std::make_error_code(std::errc(errno)));
    }
}

void posix_semaphore::acquire() {
    if (::sem_wait(&m_sem) != 0) {
        throw std::system_error(std::make_error_code(std::errc(errno)));
    }
}

bool posix_semaphore::try_acquire() noexcept { return ::sem_trywait(&m_sem) == 0; }

posix_semaphore::~posix_semaphore() { ::sem_destroy(&m_sem); }
} // namespace ext
