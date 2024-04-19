#pragma once

#include "semaphore.hpp"
#include "bits/basic_blocking_deque/basic_blocking_deque.hpp"
#include "memory.hpp"

namespace stdext {
template <class E, class Allocator = stdext::allocator<E>>
using blocking_deque = basic_blocking_deque<E, Allocator, counting_semaphore<>>;
}
