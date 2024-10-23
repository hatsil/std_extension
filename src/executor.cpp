#include "std_extension/executor.hpp"
#include "std_extension/exception.hpp"

#include <iostream>

namespace ext {
executor::executor(std::size_t nthreads_)
    : m_activeness(0) {
    if (0 == nthreads_) {
        throw exception("nthreads_ == 0");
    }

    try {
        for (std::size_t i = 0; i < nthreads_; i++) {
            m_workers.emplace_back([this] {
                for (;;) {
                    std::shared_ptr<std::move_only_function<State()>> task = m_tasks.pop_front();
                    State status = (*task)();
                    while (State::STOP == status) {
                        for (;;) try {
                            m_tasks.emplace_front([] { return State::STOP; });
                            return;
                        } catch (...) {
                            std::this_thread::yield(); // try again
                        }
                    }
                }
            });
        }
    } catch(...) {
        shutdown();
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

void executor::shutdown() noexcept { do_shutdown<ShutdownPolicy::GRACEFUL>(); }

void executor::forced_shutdown() noexcept { do_shutdown<ShutdownPolicy::FORCED>(); }

[[nodiscard]] std::size_t executor::nthreads() const noexcept { return m_workers.size(); }
} // namespace ext
