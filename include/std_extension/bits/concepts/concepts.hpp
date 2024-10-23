#pragma once

#include "std_extension/type_traits.hpp"

namespace ext {
template <typename F, typename... Args>
concept single_use_bindable = is_single_use_bindable_v<F, Args...>;
}
