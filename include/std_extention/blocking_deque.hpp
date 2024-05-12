#pragma once

#include "bits/basic_blocking_deque/basic_blocking_deque.hpp"
#include "memory.hpp"
#include "semaphore.hpp"

namespace ext {
template <class E, class Allocator = ext::allocator<E>>
using blocking_deque = basic_blocking_deque<E, Allocator, counting_semaphore<>>;
}
