#pragma once
#include "misc/typedefs.hpp"

#include <string>

struct ConfigValue {
    std::string value;
};

using ConfigValuePtr = ResourcePtr<ConfigValue>;
