#pragma once

#include "bits/basic_blocking_deque/basic_blocking_deque.hpp"
#include "memory.hpp"
#include "semaphore.hpp"

namespace ext {
template <class E, class Allocator = ext::allocator<E>>
using fair_blocking_deque = basic_blocking_deque<E, Allocator, fair_counting_semaphore<>>;
}
