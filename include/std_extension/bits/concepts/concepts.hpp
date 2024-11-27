#pragma once

#include "std_extension/type_traits.hpp"

namespace ext {
template <typename F, typename... Args>
concept executable = is_executable_v<F, Args...>;
}
