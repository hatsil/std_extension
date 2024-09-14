#pragma once

#include <semaphore.h>

namespace ext {
class posix_semaphore final {
public:
    posix_semaphore(unsigned int desired);

    posix_semaphore(const posix_semaphore &)            = delete;
    posix_semaphore &operator=(const posix_semaphore &) = delete;

    void release();
    void acquire();
    bool try_acquire() noexcept;

    ~posix_semaphore();

private:
    ::sem_t m_sem;
};
} // namespace ext
