#pragma once

#include <concepts>
#include <type_traits>

namespace ext {
template <std::invocable Func> class unexpected_deferred_task {
public:
    explicit unexpected_deferred_task(Func &&func) noexcept(
        std::is_nothrow_move_constructible_v<Func>);

    unexpected_deferred_task()                                            = delete;
    unexpected_deferred_task(const unexpected_deferred_task &)            = delete;
    unexpected_deferred_task &operator=(const unexpected_deferred_task &) = delete;

    ~unexpected_deferred_task();

private:
    int  m_uncaughtExceptions;
    Func m_func;
};
} // namespace ext
