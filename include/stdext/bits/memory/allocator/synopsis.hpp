#pragma once

#include <concepts>
#include <utility>
#include <cstdint>

namespace stdext {
template<class E> class allocator;

template<class E, class U>
constexpr bool operator==(const allocator<T>&, const allocator<U>&) noexcept;

template <std::destructible E>
constexpr void destroy_at(E *element);

template <class E, class... Args>
constexpr auto construct_at(E *p, Args &&... args)
noexcept(noexcept(::new(std::declval<void *>()) E(std::declval<Args>()...)))
-> decltype(::new(std::declval<void *>()) E(std::declval<Args>()...));

template <class E>
struct allocator {
public:
    using value_type                             = E;
    using size_type                              = std::size_t;
    using difference_type                        = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    constexpr allocator() noexcept = default;
    constexpr allocator(const allocator &) noexcept = default;

    template <class U>
    constexpr allocator(const allocator<U> &) noexcept;

    constexpr allocator& operator=(const allocator&) = default;

    constexpr ~allocator() = default;

    static constexpr size_type max_size() noexcept;

    [[nodiscard]] constexpr E *allocate(size_type n);
    constexpr void deallocate(E *p, size_type n);

    template <class U, class... Args>
    requires std::convertible_to<U *, E *>
    constexpr void construct(U *p, Args &&...args);

    template <class U>
    requires std::convertible_to<U *, E *>
    constexpr void destroy(U *p);

    template <class U>
    struct rebind {
        using other = allocator<U>;
    };

    using is_always_equal = true_type;
};
}
