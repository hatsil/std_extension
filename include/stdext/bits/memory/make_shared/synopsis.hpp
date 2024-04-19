#pragma once

#include <memory>

namespace stdext {
template <class E, class Allocator, class... Args>
std::shared_ptr<E> make_shared(const Allocator &alloc, Args &&...args);
}
