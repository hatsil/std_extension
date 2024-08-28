#include "std_extension/interrupted_exception.hpp"

#include <sstream>
#include <string>
#include <thread>

using namespace std::string_literals;

namespace ext {
static std::string get_thread_id() {
    std::ostringstream out;
    out << std::this_thread::get_id();
    return out.str();
}

interrupted_exception::interrupted_exception()
    : exception("thread: "s + get_thread_id() + " interrupted") {}
} // namespace ext
