#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>

namespace ext {
class incomplete;

template <typename T> struct static_typeid {
private:
    template <typename U> static constexpr std::string_view get_full_name() {
        return __PRETTY_FUNCTION__;
    }

    static constexpr std::string_view incomplete_full_name = get_full_name<incomplete>();
    static constexpr std::size_t      name_head_size = incomplete_full_name.find("ext::incomplete");
    static constexpr std::string_view incomplete_rest =
        incomplete_full_name.substr(name_head_size + sizeof("ext::incomplete") - 1);
    static constexpr std::size_t      name_rest_size = incomplete_rest.size();
    static constexpr std::string_view full_name      = get_full_name<T>();

public:
    static constexpr std::string_view name =
        full_name.substr(name_head_size, full_name.size() - name_head_size - name_rest_size);
};

template <typename T> constexpr inline std::string_view static_typeid_name(T &&) {
    return static_typeid<std::decay_t<T>>::name;
}
} // namespace ext
