#include "std_extention/exception.hpp"

namespace ext {
exception::exception(std::string_view what_str)
    : exception(what_str, allocator<char>()) {}

const char *exception::what() const noexcept { return m_spore->what(); }

std::ostream &exception::print_stacktrace(std::ostream &out) const noexcept {
    return m_spore->print_stacktrace(out);
}

std::ostream &operator<<(std::ostream &out, const exception &e) {
    out << "What:\n" << e.what() << "\nStacktrace:\n";
    return e.print_stacktrace(out);
}
} // namespace ext
