#include "std_extension/executor.hpp"
#include "std_extension/exception.hpp"

#include <iostream>

namespace ext {
executor::executor(std::size_t nthreads)
    : m_activeness(0) {
    if (0 == nthreads) {
        throw exception("nthreads == 0");
    }

    for (std::size_t i = 0; i < nthreads; i++) try {
            m_workers.emplace_back([this] {
                for (;;) {
                    std::shared_ptr<std::move_only_function<State()>> task(nullptr);
                    try {
                        task = m_tasks.pop_front();
                    } catch (...) {
                        return;
                    }
                    State status = (*task)();
                    while (State::STOP == status) try {
                            m_tasks.emplace_front([] { return State::STOP; });
                            return;
                        } catch (...) {
                            std::this_thread::yield(); // try again
                        }
                }
            });
        } catch (...) {
            m_activeness = -2;
            for (thread &worker : m_workers) {
                worker.interrupt();
            }
            for (thread &worker : m_workers) {
                worker.join();
            }
            throw;
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

[[nodiscard]] std::size_t executor::nthreads() const noexcept { return m_workers.size(); }

void executor::shutdown(ShutdownPolicy policy) {
    for (long expected = 0; !m_activeness.compare_exchange_weak(expected, -1); expected = 0) {
        if (-2 == expected) {
            return;
        }
        std::this_thread::yield();
    }

    for (;;) try {
            if (ShutdownPolicy::GRACEFUL == policy) {
                m_tasks.emplace_back([] { return State::STOP; });
            } else {
                m_tasks.emplace_front([] { return State::STOP; });
            }
            break;
        } catch (...) {
            std::this_thread::yield(); // try again
        }

    for (thread &worker : m_workers) {
        worker.join();
    }
    m_activeness = -2;
}
} // namespace ext
