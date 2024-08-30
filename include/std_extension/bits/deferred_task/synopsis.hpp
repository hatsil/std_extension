#pragma once

#include <concepts>
#include <type_traits>

namespace ext {
template <std::invocable Func> class deferred_task {
public:
    explicit deferred_task(Func &&func) noexcept(std::is_nothrow_move_constructible_v<Func>);

    deferred_task(deferred_task &&moved) noexcept(std::is_nothrow_move_constructible_v<Func>);
    deferred_task &
    operator=(deferred_task &&moved) noexcept(std::is_nothrow_move_constructible_v<Func> &&
                                              std::is_nothrow_move_assignable_v<Func>);

    deferred_task()                                 = delete;
    deferred_task(const deferred_task &)            = delete;
    deferred_task &operator=(const deferred_task &) = delete;

    ~deferred_task();

private:
    Func m_func;
    bool m_valid;
};
} // namespace ext
