#pragma once

#include "std_extention/memory.hpp"

#include <exception>
#include <memory>
#include <ostream>
#include <stacktrace>
#include <string>
#include <string_view>

namespace ext {
class exception : public std::exception {
public:
    exception(std::string_view what_str = "");

    template <class Allocator = ext::allocator<char>>
    explicit exception(std::string_view what_str, const Allocator &alloc);

    exception(const exception &other) noexcept            = default;
    exception &operator=(const exception &other) noexcept = default;

    const char   *what() const noexcept override;
    std::ostream &print_stacktrace(std::ostream &out) const noexcept;

private:
    class SporeBase {
    public:
        virtual ~SporeBase()                                                     = default;
        virtual const char   *what() const noexcept                              = 0;
        virtual std::ostream &print_stacktrace(std::ostream &out) const noexcept = 0;
    };

    template <class Allocator = ext::allocator<char>> class Spore final : public SporeBase {
    public:
        explicit Spore(std::string_view what_str, const Allocator &alloc);

    private:
        const char   *what() const noexcept override;
        std::ostream &print_stacktrace(std::ostream &out) const noexcept override;

        using AllocatotrStacktraceEntry =
            typename std::allocator_traits<Allocator>::rebind_alloc<std::stacktrace_entry>;
        using StackTrace = std::basic_stacktrace<AllocatotrStacktraceEntry>;
        using String     = std::basic_string<char, std::char_traits<char>, Allocator>;

        String     m_what;
        StackTrace m_stacktrace;
    };

    std::shared_ptr<SporeBase> m_spore;
};

std::ostream &operator<<(std::ostream &out, const exception &e);
} // namespace ext
