#pragma once

#include "synopsis.hpp"

#include <array>
#include <limits>
#include <memory>
#include <type_traits>

namespace ext {
template <std::destructible E> constexpr void destroy_at(E *element) {
    using EType = typename std::remove_all_extents_t<E>;
    if constexpr (std::is_array_v<E>) {
        using ElementType       = typename std::remove_extent_t<E>;
        constexpr std::size_t N = sizeof(E) / sizeof(ElementType);
        auto array = static_cast<std::array<ElementType, N> *>(static_cast<void *>(element));
        for (auto it = array->rbegin(); it != array->rend(); ++it) {
            ext::destroy_at(std::addressof(*it));
        }
    } else if constexpr (!std::is_standard_layout_v<EType> || !std::is_trivial_v<EType>) {
        element->~E();
    }
}

namespace detail {
constexpr inline void *voidify(const volatile void *ptr) noexcept {
    return const_cast<void *>(ptr);
}
} // namespace detail

template <class E, class... Args>
constexpr auto construct_at(E *p, Args &&...args) noexcept(noexcept(::new(std::declval<void *>())
                                                                        E(std::declval<Args>()...)))
    -> decltype(::new(std::declval<void *>()) E(std::declval<Args>()...)) {
    return ::new (detail::voidify(p)) E(std::forward<Args>(args)...);
}

template <class E>
template <class U>
constexpr allocator<E>::allocator(const allocator<U> &) noexcept {}

template <class E> constexpr allocator<E>::size_type allocator<E>::max_size() noexcept {
    return std::numeric_limits<size_type>::max() / sizeof(E);
}

template <class E> [[nodiscard]] constexpr E *allocator<E>::allocate(size_type n) {
    static_assert(sizeof(E) > 0, "cannot allocate incomplete types");

    if (n > max_size()) {
        throw std::bad_array_new_length();
    }

    if constexpr (alignof(E) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
        return static_cast<E *>(::operator new(n * sizeof(E), std::align_val_t(alignof(E))));
    } else {
        return static_cast<E *>(::operator new(n * sizeof(E)));
    }
}

template <class E> constexpr void allocator<E>::deallocate(E *p, size_type n) {
    if constexpr (alignof(E) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
        ::operator delete(p, n * sizeof(E), std::align_val_t(alignof(E)));
    } else {
        ::operator delete(p, n * sizeof(E));
    }
}

template <class E>
template <class U, class... Args>
    requires std::convertible_to<U *, E *>
constexpr void allocator<E>::construct(U *p, Args &&...args) {
    ext::construct_at(p, std::forward<Args>(args)...);
}

template <class E>
template <class U>
    requires std::convertible_to<U *, E *>
constexpr void allocator<E>::destroy(U *p) {
    ext::destroy_at(p);
}

template <class E> class allocator<const E> {
public:
    using value_type = E;

    template <class U> constexpr allocator(const allocator<U> &) noexcept {}
};

template <class E> class allocator<volatile E> {
public:
    using value_type = E;

    template <class U> constexpr allocator(const allocator<U> &) noexcept {}
};

template <class E> class allocator<const volatile E> {
public:
    using value_type = E;

    template <class U> constexpr allocator(const allocator<U> &) noexcept {}
};

template <class E, class U>
constexpr bool operator==(const allocator<E> &, const allocator<U> &) noexcept {
    return true;
}
} // namespace ext
