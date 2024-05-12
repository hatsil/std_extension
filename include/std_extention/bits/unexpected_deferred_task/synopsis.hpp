#pragma once

#include <type_traits>
#include <utility>

namespace ext {
template <typename Func> class unexpected_deferred_task {
public:
    explicit unexpected_deferred_task(Func &&func) noexcept(
        std::is_nothrow_move_constructible_v<std::decay_t<Func>>);

    unexpected_deferred_task()                                            = delete;
    unexpected_deferred_task(const unexpected_deferred_task &)            = delete;
    unexpected_deferred_task &operator=(const unexpected_deferred_task &) = delete;

    void operator()() noexcept((std::declval<std::decay_t<Func>>())());

    ~unexpected_deferred_task();

private:
    int  m_uncaughtExceptions;
    Func m_func;
};
} // namespace ext
