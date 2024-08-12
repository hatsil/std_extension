#include "std_extention/executor.hpp"
#include "std_extention/exception.hpp"

#include <iostream>

namespace ext {
executor::executor(std::size_t nthreads)
    : m_activeness(0) {
    if (0 == nthreads) {
        throw exception("nthreads == 0");
    }

    for (std::size_t i = 0; i < nthreads; i++) {
        m_workers.emplace_back([this] {
            for (;;) {
                if (auto task = m_tasks.pop_front(); State::STOP == (*task)()) {
                    m_tasks.emplace_front([] { return State::STOP; });
                    return;
                }
            }
        });
    }
}

executor::~executor() {
    if (0 <= m_activeness) {
        std::cerr << "Error: " << "ext::executor(" << m_workers.size()
                  << ") has been destructed while it's still active.\n"
                  << "Call std::terminate();" << std::endl;
        std::terminate();
    }
    while (-2 != m_activeness) {
        std::this_thread::yield();
    }
}

void executor::shutdown() { shutdown(ShutdownPolicy::GRACEFUL); }

void executor::forced_shutdown() { shutdown(ShutdownPolicy::FORCED); }

void executor::shutdown(const ShutdownPolicy policy) {
    for (long expected = 0; !m_activeness.compare_exchange_weak(expected, -1); expected = 0) {
        if (-2 == expected) {
            return;
        }
        std::this_thread::yield();
    }

    if (ShutdownPolicy::GRACEFUL == policy) {
        m_tasks.emplace_back([] { return State::STOP; });
    } else {
        m_tasks.emplace_front([] { return State::STOP; });
    }
    for (std::thread &worker : m_workers) {
        worker.join();
    }
    m_activeness = -2;
}
} // namespace ext
