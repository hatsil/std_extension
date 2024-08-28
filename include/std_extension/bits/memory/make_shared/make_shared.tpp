#pragma once

#include "std_extension/unexpected_deferred_task.hpp"
#include "synopsis.hpp"

#include <utility>

namespace ext {
template <class E, class Allocator, class... Args>
std::shared_ptr<E> make_shared(const Allocator &alloc, Args &&...args) {
    using AllocatorElement       = typename std::allocator_traits<Allocator>::rebind_alloc<E>;
    using AllocatorTraitsElement = typename std::allocator_traits<Allocator>::rebind_traits<E>;
    AllocatorElement         allocElement(alloc);
    E                       *element = AllocatorTraitsElement::allocate(allocElement, 1);
    unexpected_deferred_task unexpected(
        [element, &allocElement] { AllocatorTraitsElement::deallocate(allocElement, element, 1); });

    AllocatorTraitsElement::construct(allocElement, element, std::forward<Args>(args)...);
    return std::shared_ptr<E>(
        element,
        [allocElement](E *element) mutable {
            AllocatorTraitsElement::destroy(allocElement, element);
            AllocatorTraitsElement::deallocate(allocElement, element, 1);
        },
        allocElement);
}
} // namespace ext
