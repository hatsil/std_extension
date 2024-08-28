#pragma once

#include "std_extention/exception.hpp"

namespace ext {
class interrupted_exception : public exception {
public:
    interrupted_exception();
};
} // namespace ext
