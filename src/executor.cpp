#include "stdext/exception.hpp"
#include "stdext/executor.hpp"

namespace stdext {
executor::executor(std::size_t nthreads)
    : m_activeness(0) {
    if (0 == nthreads) {
        throw exception("nthreads == 0");
    }

    for (std::size_t i = 0; i < nthreads; i++) {
        m_workers.emplace_back([this] {
            for(;;) {
                if (auto task = m_tasks.pop_front(); State::STOP == (*task)()) {
                    m_tasks.emplace_front([]{ return State::STOP; });
                    return;
                }
            }
        });
    }
}

executor::~executor() {
    std::ptrdiff_t expected = -2;
    while (!m_activeness.compare_exchange_weak(expected, -2)) {
        if (0 <= expected) {
            std::cerr << "Error: " << "stdext::executor(" << m_workers.size() << ") has been destructed while it's still active.\n";
            std::cerr << "Call std::terminate();" << std::endl;
            std::terminate();
        }
        std::this_thread::yield();
        expected = -2;
    }
}

void executor::shutdown() {
    shutdown(ShutdownPolicy::GRACEFUL);
}

void executor::forced_shutdown() {
    shutdown(ShutdownPolicy::FORCED);
}

void executor::shutdown(const ShutdownPolicy policy) {
    std::ptrdiff_t expected = 0;
    while (!m_activeness.compare_exchange_weak(expected, -1)) {
        if (-2 == expected) {
            return;
        }
        expected = 0;
        std::this_thread::yield();
    }

    if (ShutdownPolicy::GRACEFUL == policy) {
        m_tasks.emplace_back([]{ return State::STOP; });
    } else {
        m_tasks.emplace_front([]{ return State::STOP; });
    }
    for (auto &&worker : m_workers) {
        worker.join();
    }
    m_activeness = -2;
}
}
