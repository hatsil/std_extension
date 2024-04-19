#pragma once

#include <utility>
#include <type_traits>

namespace stdext {
template <typename Func>
class deferred_task {
public:
    explicit deferred_task(Func &&func) noexcept(std::is_nothrow_move_constructible_v<std::decay_t<Func>>);

    deferred_task() = delete;
    deferred_task(const deferred_task &) = delete;
    deferred_task &operator=(const deferred_task &) = delete;

    void operator()() noexcept((std::declval<std::decay_t<Func>>())());

    ~deferred_task();
private:
    Func m_func;
};
}
