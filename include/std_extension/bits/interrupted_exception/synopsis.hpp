#pragma once

#include "std_extension/exception.hpp"

namespace ext {
class interrupted_exception : public exception {
public:
    interrupted_exception();
};
} // namespace ext
