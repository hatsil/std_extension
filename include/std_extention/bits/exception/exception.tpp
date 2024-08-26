#pragma once

#include "synopsis.hpp"

namespace ext {
template <class Allocator>
exception::exception(std::string_view what_str, const Allocator &alloc)
    : m_spore(::ext::make_shared<Spore<Allocator>>(alloc, what_str, alloc)) {}

#ifdef USE_BOOST_STACKTRACE
template <class Allocator>
exception::Spore<Allocator>::StacktraceType
exception::Spore<Allocator>::Stacktrace::current(const AllocatorStacktraceEntry &alloc) {
    return StacktraceType(alloc);
}
#endif

template <class Allocator>
exception::Spore<Allocator>::Spore(std::string_view what_str, const Allocator &alloc)
    : m_what(what_str, alloc)
    , m_stacktrace(Stacktrace::current(AllocatorStacktraceEntry(alloc))) {}

template <class Allocator> const char *exception::Spore<Allocator>::what() const noexcept {
    return m_what.c_str();
}

template <class Allocator>
std::ostream &exception::Spore<Allocator>::print_stacktrace(std::ostream &out) const noexcept {
    try {
        out << m_stacktrace;
    } catch (...) { /* ignored */
    }
    return out;
}
} // namespace ext
