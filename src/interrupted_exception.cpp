#include "std_extension/interrupted_exception.hpp"

#include <sstream>
#include <string>
#include <thread>

using namespace std::string_literals;

namespace ext {
static std::string to_string(const std::thread::id &tid) {
    std::ostringstream out;
    out << tid;
    return out.str();
}

interrupted_exception::interrupted_exception()
    : exception("thread: "s + to_string(std::this_thread::get_id()) + " interrupted") {}
} // namespace ext
